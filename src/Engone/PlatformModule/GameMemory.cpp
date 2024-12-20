
// #define WIN32
#ifdef OS_WINDOWS

#include "Engone/PlatformModule/GameMemory.h"
#include "Engone/Win32Includes.h"

#include "Engone/Utilities/RandomUtility.h"
#include "Engone/PlatformLayer.h"

//#define FORCE_HEAP

//#define DEBUG_GAME_MEMORY(x) x
#define DEBUG_GAME_MEMORY(x)

namespace engone {
    void GameMemory::cleanup(){
        // delete common;
        // delete allocator;
    }
    #ifdef ENGONE_PHYSICS
    void* CustomAllocator::allocate(size_t size) {
        //printf("RP3D: ");
        return GetGameMemory()->allocate(size, false);
    }
    void CustomAllocator::release(void* pointer, size_t size) {
        //printf("RP3D ");
        GetGameMemory()->free(pointer,false);
    }
    #endif
    bool GameMemory::init(void* baseAddress, u64 bytes, bool noPhysics){
        if (baseAllocation)
            return true; // returns true since the memory is initialized
        mutex.lock();
        if(!GetGameMemory()){
            SetGameMemory(this);
        }
        // baseAllocation = (void*)GigaBytes(1);
        // baseAllocation = (void*)MegaBytes(900);
        // maxSize = 500;
        maxSize = bytes;
        // GigaBytes(1);
        baseAllocation = VirtualAlloc(baseAddress,maxSize,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
        if(!baseAllocation){
            int err = GetLastError();
            printf("GameMemory : Virtual alloc failed %d!\n",err);
            mutex.unlock();
            return false;
        }
        freeBlocks.push_back({0,maxSize});
        
        mutex.unlock();
        #ifdef ENGONE_PHYSICS
        if (!noPhysics) {
            allocator = (CustomAllocator*)allocate(sizeof(CustomAllocator), false);
            new(allocator)CustomAllocator();
            common = (rp3d::PhysicsCommon*)allocate(sizeof(rp3d::PhysicsCommon), false);
            new(common)rp3d::PhysicsCommon(allocator);
        }
        #endif
        //log::out << "size: " << sizeof(rp3d::PhysicsCommon) << "\n";
        //common = new rp3d::PhysicsCommon(allocator);
        return true;
    }
    void* GameMemory::getBaseAdress() {
        return baseAllocation;
    }
    bool GameMemory::save(const std::string& path){
        auto file = FileOpen(path, FILE_CLEAR_AND_WRITE, nullptr);
        if (!file) {
            log::out << log::RED << "Engone : Failed saving game memory\n";
            return false;
        }
        // Use the start of the last free block as bytes to write instead of max memory.
        // It will be faster since it requires less bytes to write.
        
        //Block* emptyBlock = 0;
        u64 memory = getMaxMemory();
        if (getUsedBlocks().size() != 0) {
            Block& block = getUsedBlocks().back();
            memory = block.start + block.size;
        }
        
        log::out << "Saving game memory ("<<memory<<" bytes)... ";
        log::out.flush();
        FileWrite(file, getBaseAdress(), memory);
        log::out << "done\n";
        FileClose(file);
        return true;
    }
    bool GameMemory::load(const std::string& path){
        u64 fileSize;
        auto file = FileOpen(path, FILE_READ_ONLY, &fileSize);
        if (!file) {
            log::out << log::RED << "Engone : Failed loading game memory\n";
            return false;
        }
        // Check errors?
        log::out << "Loading game memory ("<<fileSize<<" bytes)... ";
        log::out.flush();
        FileRead(file, getBaseAdress(), fileSize);
        log::out << "done\n";
        FileClose(file);
        return true;
    }
    #ifdef ENGONE_PHYSICS
    rp3d::PhysicsCommon* GameMemory::getCommon() {
        return common;
    }
    #endif
    void GameMemory::insertFreeBlock(Block block){
        //-- Find spot for block
        int index = freeBlocks.size();
        for(int i=0;i<freeBlocks.size();i++){
            Block& b = freeBlocks[i];
            if(b.start>block.start){
                index = i;
                break;
            }
        }
        
        //-- Get adjacent blocks
        Block* prev=nullptr;
        Block* next=nullptr;
        if(index-1>=0){
            prev = &freeBlocks[index-1];
        }
            if(index<freeBlocks.size()){
            next = &freeBlocks[index];
        }
        
        //-- Combine blocks
        bool replacePrev = false;
        bool replaceNext = false;
        if(prev){
            if(prev->start+prev->size==block.start){
                block.start = prev->start;
                block.size = block.size + prev->size;
                replacePrev = true;
            }
        }
        if(next){
            if(block.start+block.size==next->start){
                block.size = block.size + next->size;
                replaceNext = true;
            }
        }
        
        //-- Replace/remove or create new blocks
        if(replacePrev){
            *prev = block;
            if(replaceNext)
                freeBlocks.erase(freeBlocks.begin()+index);
        }else if(replaceNext) {
            *next = block;
        }else{
            freeBlocks.insert(freeBlocks.begin()+index,block);
        }
    }
    void GameMemory::insertUsedBlock(Block block){
        for(int i=0;i<usedBlocks.size();i++){
            Block& b = usedBlocks[i];
            if(b.start>block.start){
                usedBlocks.insert(usedBlocks.begin()+i,block);  
                return; 
            }
        }
        usedBlocks.push_back(block); 
    }
    void* GameMemory::allocate(u64 bytes, bool heap){
#ifndef FORCE_HEAP
        if (heap)
#endif
            return Allocate(bytes);

        if(bytes==0) return nullptr;
        mutex.lock();
        int index = -1;
        //Todo: start searching from random position to allocate more uniformly.
        //      Will also use less loops.
        for(int i=0;i<freeBlocks.size();i++){
            Block& block = freeBlocks[i];
            if(block.size>=bytes){
                index = i;
                break;
            }
        }
        
        if (index == -1) {
            mutex.unlock();
            DEBUG_GAME_MEMORY(log::out << "Failed allocate\n";)
            return nullptr;
        }
        Block& freeBlock = freeBlocks[index];
        Block newBlock = {freeBlocks[index].start,bytes};
        if(freeBlock.size == bytes){
              freeBlocks.erase(freeBlocks.begin()+index); 
        }else{
            freeBlock.start += bytes;
            freeBlock.size -= bytes;
        }
        usedMemory+=newBlock.size;
        insertUsedBlock(newBlock);
        mutex.unlock();
        DEBUG_GAME_MEMORY(log::out<<"Alloc {"<<newBlock.start<<","<<newBlock.size<<"}\n";)
        return (void*)((u64)baseAllocation + (u64)newBlock.start);
    }
    void* GameMemory::reallocate(void* ptr, u64 bytes){
#ifdef FORCE_HEAP
        return Reallocate(ptr, bytes, bytes);
#endif
        // Todo: bytes==0, bytes<used.size, bytes==used.size
        
        // Todo: ptr is 0?
        
        if(bytes==0){
            GameMemory::free(ptr);
            return nullptr;
        }
        mutex.lock();
        u64 start = (u64)ptr-(u64)baseAllocation;
        
        //-- Find the pointer/block
        int index=-1;
        for(int i=0;i<usedBlocks.size();i++){
            Block& block = usedBlocks[i];
            if(block.start==start){
                index = i;
                break;
            }
        }
        if (index == -1) {
            mutex.unlock();
            Assert(("GameMemory: pointer doesn't exist", false));
            return nullptr; // ptr doesn't exist
        }
        Block& used = usedBlocks[index];
        //log::out << "Realloc old->{" << used.start << "," << used.size << "}\n";
        if (used.size == bytes) {
            mutex.unlock();
            DEBUG_GAME_MEMORY(log::out << "Cannot reallocate same size\n";)
            return ptr;
        }
        
        //-- Find adjacent free block
        index=-1;
        for(int i=0;i<freeBlocks.size();i++){
            Block& block = freeBlocks[i];
            if(used.start+used.size == block.start){
                if(used.size+block.size>=bytes)
                    index = i;
                break;
            }
            if(block.start>used.start)
                break;
        }
        if(index==-1){
            if(bytes<used.size){
                usedMemory+=bytes-used.size;
                Block freeBlock = {used.start+bytes,used.size-bytes};
                insertFreeBlock(freeBlock);
                used.size = bytes;
                mutex.unlock();
                return ptr;
            }else{
                // new allocation
                u64 size = used.size; // copy value because used becomes invalid
                void* newPtr = GameMemory::allocate(bytes);
                // Todo: check failure
                if(newPtr){
                    memcpy(newPtr,ptr,size);
                    // Todo: free will find the used block but we already know it so if we could
                    //      Give it to the function or write the function again but based on what we already know
                    //      We could save time performance.
                    mutex.unlock();
                    GameMemory::free(ptr);
                    return newPtr;
                }
                mutex.unlock();
                return nullptr;
            }
        }else{
            usedMemory+=bytes-used.size;
            Block& freeBlock = freeBlocks[index];
            if(bytes<used.size){
                freeBlock.start = used.start + bytes;
                freeBlock.size += used.size - bytes;
                used.size = bytes;
            } else if(freeBlock.size+used.size==bytes){
                freeBlocks.erase(freeBlocks.begin()+index);
                used.size=bytes;
            }else{
                freeBlock.start = used.start+bytes;
                freeBlock.size -= bytes-used.size;
                used.size = bytes;
            }
            mutex.unlock();
            return ptr;
        }
    }
    void GameMemory::free(void* ptr,bool heap){
#ifndef FORCE_HEAP
        if (heap)
#endif
            return Free(ptr,0);
        mutex.lock();
        u64 start = (u64)ptr-(u64)baseAllocation;
        int index=-1;
        for(int i=0;i<usedBlocks.size();i++){
            Block& block = usedBlocks[i];
            if(block.start==start){
                index = i;
                break;
            }
            if(block.start>start)
                break;
        }
        if (index == -1) {
            mutex.unlock();
            DEBUG_GAME_MEMORY(log::out << "Cannot free empty pointer\n";)
            return;
        }
        
        Block block = usedBlocks[index];
        usedMemory-=block.size;
        usedBlocks.erase(usedBlocks.begin()+index);
        insertFreeBlock(block);
        mutex.unlock();
        DEBUG_GAME_MEMORY(log::out << "Free {" << block.start << "," << block.size << "}\n";)
    }
    u64 GameMemory::getMaxMemory(){
        return maxSize;
    }
    u64 GameMemory::getUsedMemory(){
        return usedMemory;
    }
    std::vector<GameMemory::Block>& GameMemory::getFreeBlocks(){
        return freeBlocks;
    }
    std::vector<GameMemory::Block>& GameMemory::getUsedBlocks(){
        return usedBlocks;
    }
    void GameMemory::getPointers(std::vector<void*>& ptrs){
        ptrs.clear();
        for(Block& block : usedBlocks){
            ptrs.push_back((void*)(block.start + (u64)baseAllocation));
        }
    }
    void GameMemory::print(){
        int address=0;
        int freeIndex = 0;
        int usedIndex = 0;
        
        while(true){
            Block* f = nullptr;
            Block* u = nullptr;
            if(freeIndex<freeBlocks.size())
                f = &freeBlocks[freeIndex];
            if(usedIndex<usedBlocks.size())
                u = &usedBlocks[usedIndex];
            
            bool done=false;
            int type = 0;
            if(u){
                if(u->start==address){
                    address+=u->size;   
                    usedIndex++;
                    done=true;
                    type=0;
                }
            }
            if(f&&!done){
                if(f->start==address){
                    address+=f->size;
                    freeIndex++;
                    done=true;
                    type=1;
                }
            }
            if(!done){
                if(u||f)
                    printf("ERROR with print: BLOCKS ARE LEFT!\n");
                break;
            }
            
            // Todo: KB/MB formatting?
            Block* pr = type?f:u;
            log::out << " [ "<<pr->start;
            if(type) log::out<<" - "<< pr->size<<" - ";
            else log::out << " # "<<pr->size<<" # ";
            log::out<<(pr->start+pr->size-1)<<"]\n";
            // printf("|%llu - %llu ",f->start,f->start+f->size-1);
        }
        // log::out << "\n";
        // for(Block& b : freeBlocks){
        //      printf("[%llu _ %llu]",b.start,b.start+b.size);
        // }
        // printf("\n");
        // for(Block& b : usedBlocks){
        //      printf("[%llu - %llu]",b.start,b.start+b.size);
        // }
        // printf("\n");
    }
    static GameMemory* s_gameMemory=0;
    GameMemory* GetGameMemory() {
        // if(!s_gameMemory){
        //     s_gameMemory = ALLOC_NEW(GameMemory);
        //     s_gameMemory->init((void*)GigaBytes(1000), MegaBytes(10),false);
        // }
        return s_gameMemory;
    }
    void SetGameMemory(GameMemory* memory){
        if(!s_gameMemory || !memory) {
            s_gameMemory = memory;
        } else if (s_gameMemory != memory) {
            printf("SetGameMemory : Cannot overwrite global variable\n");   
        }
    }
    void TestActions(GameMemory& mem, int count){
        for(int i=0;i<count;i++){
            mem.allocate(10+GetRandom()*10);
        }
    }
    void WriteResult(GameMemory& memory, const char* path){
        auto file = FileOpen(path,FILE_CLEAR_AND_WRITE, nullptr);
        
        char buffer[10000];
        int bytes=0;
        auto fb = memory.getFreeBlocks();
        auto ub = memory.getUsedBlocks();
        char varName[100];
        strcpy(varName,path);
        int nameBegin = 0;
        int length = strlen(path);
        for(int i=length-1;i>=0;i--){
            char chr = path[i];
            if(chr=='/'){
                nameBegin = i+1;
                // printf("%s\n",varName+nameBegin);
                break;
            }
            if(chr=='.'){
                varName[i] = 0;
            }
        }
        // printf("%s\n",varName+nameBegin);
        
        bytes+=sprintf(buffer+bytes,"var %s=[[",varName+nameBegin);
        for(int i=0;i<fb.size();i++){
            auto& b = fb[i];
            bytes += sprintf(buffer+bytes,"%llu,%llu,",b.start,b.size);
        }
        bytes+=sprintf(buffer+bytes,"],[");
        for(int i=0;i<ub.size();i++){
            auto& b = ub[i];
            bytes += sprintf(buffer+bytes,"%llu,%llu,",b.start,b.size);
        }
        bytes+=sprintf(buffer+bytes,"]]");
        
        // const char* test="var state0=[[0,7],[8,2]]";
        // FileWrite(file,test,strlen(test));
        
        FileWrite(file,buffer,bytes);
        FileClose(file);
    }
    void TestRandomActions(){
        GameMemory memory{};
        memory.init((void*)GigaBytes(1000), 1000, true);
        
        std::vector<void*> ptrs;
        int attempts=0;
        
        char buffer[256];
        int states = 0;
        int fails = 0;
        
        float chances[]{0.4,0.2,0.4};
        
        while(attempts<100){
            attempts++;
            
            float chance = GetRandom();
            float total = 0;
            int type = 0;
            while(type<3){
                total+=chances[type];
                if(chance<total)
                    break;
                
                type++;
            }
            
            u64 size = GetRandom()*80;
            int ind = GetRandom()*ptrs.size();
            
            if(type!=0&&ptrs.size()==0){
                fails++;
                continue;
            }
            
            log::out << "Attempt "<<attempts<<" "<<states<<"\n";
            
            u64 memBefore = memory.getUsedMemory();

            if(type==0){
                log::out << "alloc "<<size<<"\n";
                void* ptr = memory.allocate(size);
                if(ptr)
                    ptrs.push_back(ptr);
                else
                    fails++;
            }else if (type==1){
                if(ptrs.size()==0)
                    continue;
                log::out << "free\n";
                void* ptr = ptrs[ind];
                memory.free(ptr);
                ptrs.erase(ptrs.begin()+ind);
            }else {
                if(ptrs.size()==0)
                    continue;
                log::out << "realloc "<<size<<"\n";
                void* ptr = memory.reallocate(ptrs[ind],size);
                if(ptr){
                    ptrs[ind]=ptr;
                }else if(size==0){
                    ptrs.erase(ptrs.begin()+ind);
                } else {
                    printf("hum\n");
                }
            }
            if (memBefore == memory.getUsedMemory()) {
                log::out << "No change\n";
            }
            if(attempts%1==0){
                sprintf(buffer,"../Tests/GameMemory/states/state%d.js",states);
                WriteResult(memory,buffer);
                states++;
            }
            //memory.print();
        }
        sprintf(buffer,"../Tests/GameMemory/states/state%d.js",states);
        states++;
        WriteResult(memory,buffer);
        log::out << "Fails "<<fails<<", States "<<states<<"\n";
    }
    void SpecialTest(){
        GameMemory& memory = *GetGameMemory();
        
        TestActions(memory,13);
        
        log::out << "Stage 1\n";
        // memory.print();
        WriteResult(memory,"Tests/GameMemory/states/state0.js");
        
        std::vector<void*> ptrs;
        memory.getPointers(ptrs);
        
        for(int i=0;i<5;i++){
            int index = GetRandom()*ptrs.size();
            
            memory.free(ptrs[index]);
            ptrs.erase(ptrs.begin()+index);
        }
        log::out << "Stage 2\n";
        // memory.print();
        WriteResult(memory,"Tests/GameMemory/states/state1.js");
        
        TestActions(memory,13);
        
        log::out << "Stage 3\n";
        // memory.print();
        WriteResult(memory,"Tests/GameMemory/states/state2.js");
    }
    void GameMemoryTest(){
        // Todo: If you want to be fancy you could write some html and js to view the memory stages.
        //      You could write the memory state to a result.js file and then use it in some other js script.
        
        log::out << "Run GameMemoryTest\n";
        TestRandomActions();
    }
}
#endif