#include "PlayerData.h"

PlayerData::PlayerData(std::string data) {
	LoadData(data);
}
std::string PlayerData::GetData() {
	return "name/p:/"+name+"/p-/world/p:/" + world;
}
void PlayerData::LoadData(std::string data) {
	std::vector<std::string> split = SplitString(data, "/p-/");
	for (std::string line : split) {
		std::vector<std::string> set = SplitString(line, "/p:/");
		if (set[0] == "name") {
			name = set[1];
		}
		else if (set[0] == "world") {
			world = set[1];
		}
	}
}