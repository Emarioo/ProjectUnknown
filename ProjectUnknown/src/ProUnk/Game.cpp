#include "ProUnk/Game.h"

#include "Engone/Engone.h"

int GameInit(void* _engone, void* _app){
    using namespace engone;
    Engone* engone = (Engone*)_engone;
    AppInstance* app = (AppInstance*)_engone;
    
    app->instanceData = GetGameMemory().allocate(sizeof(GameInstance));
    GameInstance* instance = (GameInstance*) app->instanceData;
    
    // instance->mainWindow = engone->createWindow();
    return 0;
}
int GameUpdate(void* engone, void* app){
    return 0;
}