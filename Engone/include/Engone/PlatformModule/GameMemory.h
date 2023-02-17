#pragma once
#include "Engone/Utilities/Typedefs.h"

#include <unordered_map>

#include "Engone/Utilities/rp3d.h"

namespace engone{
    class GameMemory;
    class CustomAllocator : public rp3d::MemoryAllocator {
    public:
        CustomAllocator(GameMemory* memory) : gameMemory(memory) { }    
        void* allocate(size_t size) override {
            return nullptr;
            // gameMemory->allocate(size);
        }
        void release(void* pointer, size_t size) override {
            
        }
        GameMemory* gameMemory=nullptr;
    };
    // typedef uint64 GameMemoryID;
    class GameMemory {
    public:
        GameMemory() = default;
        ~GameMemory(){cleanup();}
        void cleanup();
        
        bool init();
        bool save();
        bool load(); // this will invalidate any pointers

        void* allocate(uint64 bytes);
        void* reallocate(void* ptr, uint64 bytes);
        void free(void* ptr);
        
        // Static functions will use the active game memory. (thread based)
        // void makeActive();
        // void* Allocate(uint64 bytes);
        // void* Reallocate(void* ptr, iuint64nt bytes);
        // void Free(void* ptr);


        rp3d::PhysicsCommon* common=nullptr;
        CustomAllocator* allocator=nullptr;

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
        uint64 maxSize=0;
        
        struct Block {
            uint64 start;
            uint64 size;
        };
        std::vector<Block> freeBlocks;
        std::vector<Block> usedBlocks;
        
        void insertFreeBlock(Block block);
        void insertUsedBlock(Block block);
        
        void print();
    };
    void GameMemoryTest();
}