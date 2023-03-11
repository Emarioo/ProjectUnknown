#pragma once

#include "Engone/Engone.h"

struct GameInstance {
    engone::Window* mainWindow;
    
};

int GameInit(void* engone, void* app);
int GameUpdate(void* engone, void* app);