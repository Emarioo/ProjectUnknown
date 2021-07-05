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
	Add 'item' to an empty spot in the container
	return true if all items was put into container. False if item still has items
	
	*/
	bool AddItem(Item* item);
	/*
	Add 'item' to the specified slot
	return true if successful (false could mean slot is non-existent or not empty)
	*/
	bool AddItemAt(Item* item, int slotX, int slotY);
	/*
	Return nullptr if slot is empty
	*/
	Item* GetItemAt(int slotX, int slotY);
	/*
	Return nullpptr if slot is empty
	*/
	Item* GetItemAt(int index);
	Item** GetItemPointerAt(int slotX, int slotY);
	/*
	Self explanatory
	*/
	//Item* GetItem(ItemType type);
	bool HasItem(const std::string& name, int count);
	int GetItemCount(const std::string& name);
	/*
	Remove first found item with specified name
	*/
	bool RemoveItem(const std::string& name, int count);
	/*
	Remove item from slot
	return nullpointer if slot is empty or non-existent
	*/
	Item* TakeItemAt(int slotX, int slotY);
	/*
	Items in 'a' is moved to 'b' (this is important logic, find out why in the function)
	'button' and 'action' determines how items should be moved
	left click switches 'a' and 'b'
	right click puts half of 'a' into 'b'
	holding shift key switches 'a' and 'b' (b is moved into a)
	*/
	void SwitchItem(Item** a, Item** b,int button, int action);
	int EmptySlots();
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

	// Change to normal array?
	std::vector<Item*> itemArray;

	int totalItemCount=0;

};