#pragma once

#include <vector>
#include <string>
#include "Engine/Engine.h"
#include "Item.h"

enum ItemType : char { // Which texture to draw
	UnkownType,
	ResourceType,
	RefinedResourceType,
	ToolType,
	WeaponType,
	MagicalType
};
enum ItemName : short { // What index in texture
	FiberItem,
	FlintItem,
	StickItem,

	RefinedWoodItem=64,
	
	CrudeAxe=64*2,
	CrudePickaxe,

	CrudeSword=64*3,
	FineSword,

	MagicStaff=64*4
};

void InitItemHandler();
void DrawItem(Item* item, float x, float y, float w, float h);
void FillItemWithData(Item* item);
