#pragma once
#include "Engone/Utilities/Typedefs.h"

#include <unordered_map>

#include "Engone/Utilities/rp3d.h"

#include "Engone/PlatformModule/PlatformLayer.h"

namespace engone{
    class GameMemory;
    #ifdef ENGONE_PHYSICS
    class CustomAllocator : public rp3d::MemoryAllocator {
    public:
        CustomAllocator() { }    
        ~CustomAllocator() override {}
        void* allocate(size_t size) override;
        void release(void* pointer, size_t size) override;
    };
    #endif
    // typedef u64 GameMemoryID;
    class GameMemory {
    public:
        GameMemory() = default;
        ~GameMemory(){cleanup();}
        void cleanup();
        
        bool init(void* baseAddress, u64 bytes, bool noPhysics);
        bool save(const std::string& path);
        bool load(const std::string& path);

        void* allocate(u64 bytes, bool heap=false);
        void* reallocate(void* ptr, u64 bytes);
        void free(void* ptr, bool heap=false);
        
        // Static functions will use the active game memory. (thread based)
        // void makeActive();
        // void* Allocate(u64 bytes);
        // void* Reallocate(void* ptr, iuint64nt bytes);
        // void Free(void* ptr);
#ifdef ENGONE_PHYSICS
        rp3d::PhysicsCommon* getCommon();
#endif
        u64 getUsedMemory();

        void getPointers(std::vector<void*>& ptrs);
        struct Block {
            u64 start;
            u64 size;
        };
        std::vector<Block>& getFreeBlocks();
        std::vector<Block>& getUsedBlocks();

        void* getBaseAdress();
        u64 getMaxMemory();

        void print();

    private:
    #ifdef ENGONE_PHYSICS
        rp3d::PhysicsCommon* common=nullptr;
        CustomAllocator* allocator=nullptr;
    #endif
        Mutex mutex;

        // GameMemoryID allocate(int bytes);
        // GameMemoryID reallocate(GameMemoryID, int bytes);
        // void free(GameMemoryID id);
        // void* aquire(GameMemoryID id);
        // void* release(GameMemoryID id);
        
        // static GameMemoryID Allocate(int bytes);
        // static GameMemoryID Reallocate(GameMemoryID id, int bytes);
        // void Free(GameMemoryID id);
        // void* Aquire(GameMemoryID id);
        // void* Release(GameMemoryID id);
        // struct GamePointer {
        //     GameMemoryID id;
        //     void* ptr;
        //     int size;
        //     bool inUse;
        // };
        // std::unordered_map<GameMemoryID,GamePointer> pointerMap;
        
        // Memory stuff
        void* baseAllocation=nullptr;
        u64 maxSize=0;
        u64 usedMemory=0;
        
        std::vector<Block> freeBlocks;
        std::vector<Block> usedBlocks;
        
        void insertFreeBlock(Block block);
        void insertUsedBlock(Block block);
        
    };
    GameMemory* GetGameMemory();
    void SetGameMemory(GameMemory* memory);
    void GameMemoryTest();
}