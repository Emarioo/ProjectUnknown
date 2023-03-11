garb.cpp

// Engine should deal with Animation blending/matrices, physics
// Basic rendering objects.

void main(){
    
    Engone {
        Object[]
        Assets[]
        UIModule
        RenderPipeline
        
        GameMemory
        
        FunctionsAndUtilities
    }
    
    Engone engone;
    engone.gameMemory = new GameMemory()
    
    GameMemory memory;
    auto ptr = memory.allocate(sizeof(GameMemory*));
    *ptr = &memory;

    *ptr = &memory

    Array arr(int,&engone)
    arr.add(memory,ptr);
    
    Game {
        Registries[]
        PlayerController
        
    }
    Game* memory = engone.gameMemory.allocate(sizeof(Game));    
    engone.createApplication(UpdateAndRender,memory);
    
    int appId, window?
    
    Update thread (game logic, supplies stuff for render pipline)
    Render thread (renders based on a pipeline filled by update thread)
    
    
    
    engone.start(UpdateAndRender);
    
    Better thing may be everything from engine uses GameMemory
    GameMemory can use one large allocation or heap
}
void UpdateAndRender(LoopInfo& info){
    info.userData
}