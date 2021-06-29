#include "GameHandler.h"

Player* global_player;
Player* GetPlayer() {
	return global_player;
}
void SetPlayer(Player* plr) {
	global_player = plr;
}