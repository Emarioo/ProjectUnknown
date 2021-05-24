#include "ItemHandler.h"

#include <unordered_map>

Texture itemTexture;
engine::BufferContainer itemContainer;

void InitItemHandler() {
	itemTexture.LoadTex("assets/textures/items");
	itemContainer.Setup();

}
/*
void AddItemTexture(const std::string& name, const std::string& path) {
	if (engine::FileExist(path)) {
		itemTextures[name] = Texture(path);
	} else {
		bug::out < "Cannot find Texture '" < path < "'\n";
	}
}
Texture* GetTextureFromItem(const std::string& name) {
	if (itemTextures.count(name) == 0) {
		bug::out < bug::RED < "Cannot find Texture '" < name < "'\n";
	} else
		return &itemTextures[name];
	return nullptr;
}*/

void DrawItem(const std::string& name, float x, float y,float w, float h) {

}