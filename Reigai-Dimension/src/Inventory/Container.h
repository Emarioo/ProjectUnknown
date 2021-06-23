#pragma once

#include "Item.h"
#include <vector>

/*
The container will destroy it's contents when destroyed. It is therefore importent to
not have a refrence to any items in the container or at least be very careful with it.
*/
class Container {
public:
	Container();
	Container(const std::string& name, int slotW, int slotH);
	~Container();
	/*
	Draw container background and all items in the container
	*/
	void Draw();
	/*
	Add 'item' to an empty spot in the container
	return true if all items was put into container. False if some remain in itemstack
	remember to remove mouse held reference to item.
	*/
	bool AddItem(Item* item);
	/*
	Add 'item' to the specified slot
	return true if successful (false could mean slot is non-existent or not empty)
	*/
	bool AddItem(Item* item, int slotX, int slotY);
	/*
	Return nullptr if slot is empty
	*/
	Item* ItemAt(int slotX, int slotY);
	/*
	Remove first found item with specified name
	*/
	Item* TakeItem(const std::string& name);
	/*
	Remove item from slot
	return nullpointer if slot is empty or non-existent
	*/
	Item* TakeItem(int slotX, int slotY);
	/*
	Will also return false if slot is outside the array
	*/
	bool IsEmpty(int slotX, int slotY);
	/*
	true if all slots are occupied
	*/
	bool IsFull();
	/*
	Get slotWidth
	*/
	int GetSlotWidth();
	/*
	Get slotHeight
	*/
	int GetSlotHeight();

private:

	std::string name;

	int slotWidth=0;
	int slotHeight=0;

	std::vector<Item*> itemArray;

	int totalItemCount=0;

};