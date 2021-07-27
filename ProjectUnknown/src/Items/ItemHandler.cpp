#include "ItemHandler.h"

#include "Engine/Utility/Utilities.h"

std::vector<CraftingCategory> craftingCategories;
CraftingCategory* GetCategory(int index) {
	if(-1<index&&index<craftingCategories.size())
		return &craftingCategories[index];
	return nullptr;
}
int GetCategoriesSize() {
	return craftingCategories.size();
}
std::vector<ItemType> itemList;
std::vector<std::string> textureGroups;
void InitItemList() {
	engine::FileReport err;
	std::vector<std::string> list = engine::ReadTextFile("assets/items/itemlist.dat",&err);
	if(err==engine::FileReport::NotFound)
		return;

	int group = -1;
	int index = 0;
	for (int i = 0; i < list.size();i++) {
		std::string str = list[i];
		std::vector<std::string> split = engine::SplitString(list[i], ",");
		if (str[0] == '#') {
			engine::AddTextureAsset("items/"+str.substr(1));
			textureGroups.push_back(str.substr(1));
			group++;
			index = 0;
		}
		else if (str[0]=='/') {
			break;
		}
		else {
			itemList.push_back(ItemType(group,index,split[0], std::stoi(split[1])));
			index++;
		}
	}

}
void InitCraftingRecipes() {
	engine::FileReport err;
	std::vector<std::string> list = engine::ReadTextFile("assets/items/craftingrecipes.dat", &err);
	if (err == engine::FileReport::NotFound)
		return;

	int index=0;
	CraftingCategory* lastCat=nullptr;
	for (int i = 0; i < list.size(); i++) {
		std::string str = list[i];
		std::vector<std::string> split = engine::SplitString(list[i], ",");
		if (str[0]=='#') {
			craftingCategories.push_back(CraftingCategory(str.substr(1)));
			lastCat = &craftingCategories.back();
		} else if (str[0] == '/') {
			break;
		} else {
			if (lastCat == nullptr)
				bug::out < "ItemHandler.cpp : InitCraftingRecipes - lastCat is nullptr\n";
			CraftingRecipe* recipe = lastCat->AddRecipe(split[0], std::stoi(split[1]));
			for (int i = 1; i < split.size()/2; i++) {
				recipe->AddInput(split[2*i], std::stoi(split[2*i+1]));
			}
		}
	}
}
ItemType GetItemType(const std::string& name) {
	for (int i = 0; i < itemList.size(); i++) {
		if (itemList[i].name == name) {
			return itemList[i];
		}
	}
	return ItemType(0,0,"Unknown",999);
}
engine::BufferContainer itemContainer;
void InitItemHandler() {

	InitItemList();
	InitCraftingRecipes();

	unsigned int ind[]{ 0,1,2,2,3,0 };

	itemContainer.Setup(true, nullptr, 16, ind, 6);
	itemContainer.SetAttrib(0, 4, 4, 0);
}
int imageSize=512;
int itemWidth=8;
float size=1.f/itemWidth;
void DrawItem(Item* item, float x, float y,float w, float h, float r, float g, float b, float a) {
	if (item == nullptr)
		return;

	if (item->count>1) {
		DrawItem(item->type, x, y, w, h,r,g,b,a, std::to_string(item->count));
	}
	else {
		DrawItem(item->type, x, y, w, h,r,g,b,a, "");
	}
	
}
void DrawItem(ItemType& type, float x, float y, float w, float h, float r, float g, float b, float a,const std::string& text) {

	if (textureGroups.size() > type.textureGroup) {
		short index = type.textureIndex;
		engine::BindTexture(0, "items/" + textureGroups[type.textureGroup]);

		float u = (index % itemWidth) * size;
		float v = ((int)((itemWidth - 1) - index / itemWidth)) * size;

		y -= h; // offsetting

		engine::SetColor(1, 1, 1, 1);
		engine::SetSize(1, 1);
		engine::DrawUVRect(x, y, w, h, u, v, size, size);

		if (!text.empty()) {
			engine::SetTransform(x + w / 6, y + h / 4);
			engine::SetColor(r, g, b, a);
			engine::SetSize(0.75, 0.75);
			engine::DrawString("consolas42", text, true, h * 0.6, -1);
		}
	}
}