#include "gonpch.h"

#include "CraftingRecipe.h"

#include "ItemHandler.h"

#ifdef gone
CraftingIngredient::CraftingIngredient(const std::string& name, int count) 
: name(name),count(count) {

}
CraftingRecipe::CraftingRecipe(const std::string& name, int count)
: output(name,count){

}
void CraftingRecipe::AddInput(const std::string& name, int count) {
	inputs.push_back(CraftingIngredient(name,count));
}/*
void CraftingRecipe::SetOutput(const std::string& name, int count) {
	output.name = name;
	output.count = count;
}*/
int CraftingRecipe::GetPossibleCraftingAttempts(Container* inventory) {
	int craftingAttempts = 999;
	for (int i = 0; i < inputs.size();i++) {
		int count = inventory->GetItemCount(inputs[i].name)/inputs[i].count;
		if (craftingAttempts>count)
			craftingAttempts = count;
	}
	return craftingAttempts;
}
int CraftingRecipe::AttemptCraft(Container* inventory, int count) {
	int craftCount = 0;
	
	ItemType outputType = GetItemType(output.name);

	// Is there already output
	int emptySpace = 0;
	for (int i = 0; i < inventory->GetSlotWidth() * inventory->GetSlotHeight();i++) {
		Item* item = inventory->GetItemAt(i);
		if (item!=nullptr) {
			if (item->GetName() == output.name) {
				emptySpace += item->type.maxCount - item->count;
				if (output.count*count <= emptySpace) {
					craftCount = +count;
					count -= count;
					break;
				}
			}
		}
	}
	// Is there empty slots
	if (count!=0) {
		craftCount += emptySpace / output.count;
		count -= emptySpace / output.count;
		
		emptySpace = inventory->EmptySlots()*outputType.maxCount;
		if (emptySpace / output.count >= count) {
			craftCount += count;
			count -= count;
		} else {
			craftCount += emptySpace / output.count;
			count -= emptySpace / output.count;
		}
	}
	// Will there be empty slots when input items are removed
	/*
	if (count!=0) {
		//bug::out < "CraftingRecipe.cpp : AttempCraft - No space for output" < bug::end;
		emptySpace = 0;
		for (int i = 0; i < inputs.size(); i++) {
			emptySpace += inventory->GetItemCount(inputs[i].name);
			if (emptySpace/output.count>=count) {
				craftCount += count;
				count -= count;
				break;
			}
		}
		if (count != 0) {
			craftCount += emptySpace / output.count;
			count -= emptySpace / output.count;
		}
	}
	*/
	if (craftCount>0) {
		for (int i = 0; i < inputs.size();i++) {
			inventory->RemoveItem(inputs[i].name, inputs[i].count*craftCount);
		}
		inventory->AddItem(new Item(output.name, output.count*craftCount));
	}
	return craftCount;
}
#endif