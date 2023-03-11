// #define NOMINMAX
#include <stdio.h>

// #include <windows.h>
#include "Engone/Win32Includes.h"
#include "Engone/PlatformModule/GameMemory.h"

#include "GameCode/PlayerController.h"

extern "C" {
   void ENGONE_EXPORT GameCodeInit(void* memory, void* logger);
}
void GameCodeInit(void* gameMemory, void* logger){
    engone::SetGameMemory((engone::GameMemory*)gameMemory);
    // engone::SetLogger(logger);
    
    // engone::Logger* wee;
    
    // *wee << "Hello";
    engone::log::out.setRealLogger((engone::Logger*)logger);
    
    // engone::log::out << "Hello";
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if(DLL_PROCESS_ATTACH==ul_reason_for_call){
        printf("Loaded GameCode\n");
    }else if(DLL_PROCESS_ATTACH==ul_reason_for_call){
        printf("Unloaded GameCode\n");
    }
    return TRUE;
}