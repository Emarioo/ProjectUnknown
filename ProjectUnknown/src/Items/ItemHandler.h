#pragma once

#include "Item.h"
#include "CraftingCategory.h"

#include "Engone/Handlers/AssetManager.h"

CraftingCategory* GetCategory(int index);
int GetCategoriesSize();
void InitItemList();
void InitCraftingRecipes();
ItemType GetItemType(const std::string& item);
void InitItemHandler();
// Use GuiColor to change color of text. Text is automatically the count
void DrawItem(Item* item, float x, float y, float w, float h, float r, float g, float b, float a);
// Use GuiColor to change color of text. Customize the text
void DrawItem(ItemType& type, float x, float y, float w, float h, float r, float g, float b, float a, const std::string& text);
