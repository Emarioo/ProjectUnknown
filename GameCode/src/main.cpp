
#include <stdio.h>

#include <windows.h>

//extern __declspec(dllexport) void HelloDLL();

extern "C" {
    //extern __declspec(dllexport) int HelloWorld(int num);
    extern __declspec(dllexport) void HelloWorld();

}
//int HelloWorld(int num){
//    printf("HELLO DLL\n");
//    return num + 3;
//}
void HelloWorld() {
    printf("HELLO DLL\n");
    //return num + 3;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved
)
{
    printf("Loaded GameCode %llu, %u, %llu\n",hModule,ul_reason_for_call,lpReserved);

    return TRUE;
}