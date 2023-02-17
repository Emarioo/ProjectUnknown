
// #define WIN32
#ifdef WIN32

#include "Engone/PlatformModule/GameMemory.h"
#include "Engone/Win32Includes.h"

#include "Engone/Utilities/RandomUtility.h"
#include "Engone/PlatformLayer.h"

namespace engone {
    void GameMemory::cleanup(){
        // delete common;
        // delete allocator;
    }
    bool GameMemory::init(){
        // allocator = new CustomAllocator(this);
        // common = new rp3d::PhysicsCommon(allocator);
        
        baseAllocation = (void*)GigaBytes(1000);
        // baseAllocation = (void*)GigaBytes(1);
        // baseAllocation = (void*)MegaBytes(900);
        maxSize = 500;
        // GigaBytes(1);
        baseAllocation = VirtualAlloc(baseAllocation,maxSize,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
        if(!baseAllocation){
            int err = GetLastError();
            printf("GameMemory : Virtual alloc failed %d!\n",err);   
        }else{
            freeBlocks.push_back({0,maxSize});
        }
        return allocator && common && baseAllocation;
    }
    bool GameMemory::save(){
        return false;
    }
    bool GameMemory::load(){
        return false;
    }
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
    void* GameMemory::allocate(uint64 bytes){
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
        
        if(index==-1)
            return nullptr;
        
        Block& freeBlock = freeBlocks[index];
        Block newBlock = {freeBlocks[index].start,bytes};
        
        freeBlock.start += bytes;
        freeBlock.size -= bytes;
        
        insertUsedBlock(newBlock);
        return (void*)((uint64)baseAllocation + (uint64)newBlock.start);
    }
    void* GameMemory::reallocate(void* ptr, uint64 bytes){
        // Todo: bytes==0, bytes<used.size, bytes==used.size
        
        if(bytes==0){
            GameMemory::free(ptr);
            return nullptr;
        }
        
        uint64 start = (uint64)ptr-(uint64)baseAllocation;
        int index=-1;
        for(int i=0;i<usedBlocks.size();i++){
            Block& block = usedBlocks[i];
            if(block.start==start){
                index = i;
                break;
            }
        }
        if(index==-1)
            return nullptr; // ptr doesn't exist
        
        Block& used = usedBlocks[index];
        
        if(used.size==bytes)
            return ptr;
        
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
                Block freeBlock = {used.start+bytes,used.size-bytes};
                insertFreeBlock(freeBlock);
                used.size = bytes;
                return ptr;
            }else{
                // new allocation
                uint64 size = used.size; // copy value because used becomes invalid
                void* newPtr = GameMemory::allocate(bytes);
                // Todo: check failure
                if(newPtr){
                    memcpy(newPtr,ptr,size);
                    GameMemory::free(ptr);
                    return newPtr;
                }
                return nullptr;
            }
        }else{
            Block& freeBlock = freeBlocks[index];
            if(bytes<used.size){
                freeBlock.start = used.start + bytes;
                freeBlock.size += used.size - bytes; 
                used.size = bytes;
                return ptr;
            } else if(freeBlock.size+used.size==bytes){
                freeBlocks.erase(freeBlocks.begin()+index);
                used.size=bytes;
            }else{
                freeBlock.start = used.start+bytes;
                freeBlock.size -= bytes-used.size;
                used.size = bytes;
            }
            return ptr;
        }
    }
    void GameMemory::free(void* ptr){
        uint64 start = (uint64)ptr-(uint64)baseAllocation;
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
        if(index==-1)
            return;
        
        Block block = usedBlocks[index];
        usedBlocks.erase(usedBlocks.begin()+index);
        
        insertFreeBlock(block);
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
            if(type) log::out<<" --- ";
            else log::out << " ### ";
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
    void GameMemoryTest(){
        GameMemory memory{};
        memory.init();
        
        std::vector<void*> ptrs;
        int attempts=3;
        while(attempts>0){
            int type = GetRandom()*3;
            uint64 size = GetRandom()*40;
            int ind = GetRandom()*ptrs.size();
            if(type==0){
                void* ptr = memory.allocate(size);
                ptrs.push_back(ptr);
            }else if (type==1){
                if(ptrs.size()==0)
                    continue;
                void* ptr = ptrs[ind];
                memory.free(ptr);
                ptrs.erase(ptrs.begin()+ind);
            }else {
                if(ptrs.size()==0)
                    continue;
                void* ptr = ptrs[ind];
                ptrs[ind] = memory.reallocate(ptr,size);
                if(size==0){
                    ptrs.erase(ptrs.begin()+ind);
                }
            }
            attempts++;
        }
         
        memory.print();
    }
}
#endif