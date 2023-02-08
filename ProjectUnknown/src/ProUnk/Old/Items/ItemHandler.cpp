#include "gonpch.h"

#include "ItemHandler.h"

//#include "Engone/U/FileHandler.h"
#include "Engone/Engone.h"
#ifdef gone
std::vector<CraftingCategory> craftingCategories;
CraftingCategory* GetCategory(size_t index) {
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
	std::vector<std::string> list;
	engone::log::out << engone::log::RED << " InitItemList is broken\n";
	//engone::FileReport err = engone::ReadTextFile("assets/items/itemlist.dat", list);
	//if(err==engone::FileReport::NotFound)
	//	return;

	size_t group = -1;
	size_t index = 0;
	for (size_t i = 0; i < list.size();++i) {
		std::string str = list[i];
		std::vector<std::string> split = engone::SplitString(list[i], ",");
		if (str[0] == '#') {
			engone::AddAsset<engone::Texture>("items/"+str.substr(1));
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
	std::vector<std::string> list;
	//engone::FileReport err =engone::ReadTextFile("assets/items/craftingrecipes.dat", list);
	//if (err == engone::FileReport::NotFound)
	//	return;

	size_t index=0;
	CraftingCategory* lastCat=nullptr;
	for (size_t i = 0; i < list.size(); ++i) {
		std::string str = list[i];
		std::vector<std::string> split = engone::SplitString(list[i], ",");
		if (str[0]=='#') {
			craftingCategories.push_back(CraftingCategory(str.substr(1)));
			lastCat = &craftingCategories.back();
		} else if (str[0] == '/') {
			break;
		} else {
			if (lastCat == nullptr) {
				std::cout << "ItemHandler.cpp : InitCraftingRecipes - lastCat is nullptr\n";
			}
			CraftingRecipe* recipe = lastCat->AddRecipe(split[0], std::stoi(split[1]));
			for (size_t i = 1; i < split.size()/2; ++i) {
				recipe->AddInput(split[2*i], std::stoi(split[2*i+1]));
			}
		}
	}
}
ItemType GetItemType(const std::string& name) {
	for (size_t i = 0; i < itemList.size(); ++i) {
		if (itemList[i].name == name) {
			return itemList[i];
		}
	}
	return ItemType(0,0,"Unknown",999);
}
//engone::TriangleBuffer itemContainer;
void InitItemHandler() {

	InitItemList();
	InitCraftingRecipes();

	unsigned int ind[]{ 0,1,2,2,3,0 };

	//itemContainer.Init(true, nullptr, 16, ind, 6);
	//itemContainer.SetAttrib(0, 4, 4, 0);
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

	if (textureGroups.size() > (size_t)type.textureGroup) {
		short index = type.textureIndex;
		engone::GetAsset<engone::Texture>(textureGroups[type.textureGroup])->bind();

		float u = (index % itemWidth) * size;
		float v = ((int)((itemWidth - 1) - index / itemWidth)) * size;

		y -= h; // offsetting

		engone::Shader* gui = engone::GetAsset<engone::Shader>("gui");

		gui->setVec4("uColor", 1, 1, 1, 1);
		gui->setVec2("uPos", {x,y});
		gui->setVec2("uSize", {w,h});
		//engone::DrawUVRect(u, v, size, size);

		if (!text.empty()) {
			gui->setVec2("uPos", {x+w/6,y+h/4});
			gui->setVec2("uSize", { 0.75f,0.75f });
			gui->setVec4("uColor", r, g, b, a);
			engone::DrawString(engone::GetAsset<engone::Font>("consolas"), text, true, h * 0.6f,w,h,-1);
		}
	}
}
#endif