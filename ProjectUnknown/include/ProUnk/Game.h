#pragma once

#include "Engone/Engone.h"

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
        engone::AppInstance* app=0;
        engone::Window* mainWindow=0;
        
        prounk::Session* session=0;
        prounk::PlayerController playerController;
        
        prounk::PanelHandler panelHandler;
    };
}

extern "C" {
    int GAMEAPI GameInit(void* engone, void* app, void* extra);
    int GAMEAPI GamePreUpdate(engone::LoopInfo* info);
    int GAMEAPI GamePostUpdate(engone::LoopInfo* info);
}