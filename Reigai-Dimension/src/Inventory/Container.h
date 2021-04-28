#pragma once

#include "Item.h"
#include <vector>

namespace Inventory {
	/*
	The container will destroy it's contents when destroyed. It is therefore importent to
	not have a refrence to any items in the container or at least be very careful with it.
	*/
	class Container {
	public:
		Container(const std::string& name, int slotW, int slotH);
		~Container();
		/*
		Draw container background and all items in the container
		*/
		void Draw();

		/*
		Add 'item' to an empty spot in the container
		return true if successful (false could mean inventory is full)
		*/
		bool AddItem(Item* item);
		/*
		Add 'item' to the specified slot
		return true if successful (false could mean slot is non-existent or not empty)
		*/
		bool AddItem(Item* item, int slotX, int slotY);

		/*
		Remove first found item with specified name
		*/
		Item* RemoveItem(const std::string& name);

		/*
		Return nullptr if slot is empty
		*/
		Item* ItemAt(int slotX, int slotY);

		/*
		Remove item from slot
		return nullpointer if slot is empty or non-existent
		*/
		Item* RemoveItem(int slotX, int slotY);

		/*
		Will also return false if slot is outside the array
		*/
		bool IsEmpty(int slotX, int slotY);
		/*
		true if all slots are occupied
		*/
		bool IsFull();

	private:
		
		std::string name;

		int slotWidth;
		int slotHeight;

		std::vector<Item*> itemArray;

		int totalItemCount;

	};
}