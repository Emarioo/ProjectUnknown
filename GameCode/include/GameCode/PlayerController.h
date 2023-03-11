#define NOMINMAX
#include "Engone/LoopInfo.h"
#include "Engone/EngineObject.h"

#ifdef LIVE_EDITING
#define ENGONE_EXPORT extern __declspec(dllexport)
extern "C"{
    void ENGONE_EXPORT UpdatePlayerMovement(void* _info, void* _plr);
}
#else
#define ENGONE_EXPORT
void UpdatePlayerMovement(void* _info, void* _plr);
#endif


// void PlayerDropInventory(engone::EngineObject* plr);
// void PlayerSetFlight(engone::EngineObject* plr, bool yes);
// extern "C"{
//     void extern __declspec(dllexport) UpdatePlayerMovement(void* info, void* plr);
//     // void extern __declspec(dllexport) UpdatePlayerMovement(engone::LoopInfo* info, engone::EngineObject* plr);
// }