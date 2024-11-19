#pragma once

#include "Engone/Engone.h"
#include "Engone/Driver.h"

#include "ProUnk/World/Session.h"
#include "ProUnk/PlayerController.h"
#include "ProUnk/UI/PanelHandler.h"
#include "ProUnk/UI/PanelFuncs.h"

#ifdef LIVE_EDITING
    #define GAMEAPI __declspec(dllexport) __cdecl
#else
    #define GAMEAPI
#endif

namespace prounk {
    struct GameInstance {
        engone::AppInstance* app = nullptr;
        engone::Window* mainWindow = nullptr;
        engone::Engone* engine = nullptr;
        
        prounk::Session* session = nullptr;
        prounk::PlayerController playerController;
        
        prounk::PanelHandler panelHandler;
    };
}

prounk::GameInstance* init_game(prounk::GameInstance* state);
void update_game(prounk::GameInstance* game);
extern "C" {
    void GAMEAPI core_update(CoreEvent* event);
    
    int GAMEAPI GameInit(void* engone, void* app, void* extra);
    int GAMEAPI GamePreUpdate(engone::LoopInfo* info);
    int GAMEAPI GamePostUpdate(engone::LoopInfo* info);
}