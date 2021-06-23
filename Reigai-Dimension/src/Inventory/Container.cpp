#include "Container.h"

Container::Container() {

}
Container::Container(const std::string& name,int slotW, int slotH)
: name(name), slotWidth(slotW), slotHeight(slotH) {
	itemArray.resize(slotW * slotH);
}
Container::~Container() {
	// Is destroying items bad?
	for (int i = 0; i < slotWidth * slotHeight;i++) {
		if(itemArray[i]!=nullptr)
			delete itemArray[i];
	}
	itemArray.clear();
}
bool Container::AddItem(Item* item) {
	int emptySlot = -1;
	for (int i = 0; i < slotWidth * slotHeight;i++) {
		if (itemArray[i] == nullptr) {
			if (emptySlot == -1) 
				emptySlot = i;
		}else if (itemArray[i]->GetName() == item->GetName() && itemArray[i]) {
			int diff = itemArray[i]->maxCount - itemArray[i]->count;
			if (diff >= item->count) {
				itemArray[i]->count += item->count;
				item->count = 0;
				totalItemCount += item->count;
				return true;
			} else {
				itemArray[i]->count += diff;
				item->count -= diff;
			}
		}
	}
	if (item->count != 0) {
		if (emptySlot != -1) {
			for (int i = emptySlot; i < slotWidth * slotHeight; i++) {
				if (itemArray[i] == nullptr) {
					if (item->count>item->maxCount) {
						itemArray[i] = new Item(item->name, item->maxCount);// TODO: ERROR: COPY META DATA!
						item->count -= item->maxCount;
					} else {
						itemArray[i] = item;
						return true;
					}
				}
			}
		} else return false;
	} else return true;
	return false;
}
bool Container::AddItem(Item* item, int slotX, int slotY) {
	if (slotWidth > slotX && slotHeight > slotY) {
		int index = slotY * slotWidth + slotX;
		if (itemArray[index] == nullptr) {
			itemArray[index] = item;
			totalItemCount += item->count;
			return true;
		} else if (itemArray[index]->GetName() == item->GetName()) {
			itemArray[index]->count += item->count;
			totalItemCount += item->count;
			return true;
		}
	}
	return false;
}
Item* Container::TakeItem(const std::string& name) {
	for (int i = 0; i < slotWidth * slotHeight;i++) {
		if (itemArray[i]!=nullptr) {
			if (itemArray[i]->GetName() == name) {
				totalItemCount -= itemArray[i]->count;
				Item* temp = itemArray[i];
				itemArray[i] = nullptr;
				return temp;
			}
		}
	}
	return nullptr;
}
Item* Container::TakeItem(int slotX, int slotY) {
	if (slotWidth > slotX && slotHeight > slotY) {
		int index = slotY * slotWidth + slotX;
		if (itemArray[index] != nullptr) {
			totalItemCount -= itemArray[index]->count;
			Item* temp = itemArray[index];
			itemArray[index] = nullptr;
			return temp;
		}
	}
	return nullptr;
}
Item* Container::ItemAt(int slotX, int slotY) {
	if (slotWidth > slotX && slotHeight > slotY) {
		int index = slotY * slotWidth + slotX;
		if (itemArray[index] != nullptr) {
			return itemArray[index];
		}
	}
	return nullptr;
}
bool Container::IsEmpty(int slotX, int slotY) {
	if (slotWidth > slotX && slotHeight > slotY) {
		return itemArray[slotY * slotWidth + slotX] == nullptr;
	} else {
		return false;
	}
}
bool Container::IsFull() {
	for (int i = 0; i < slotWidth * slotHeight;i++) {
		if(itemArray[i]==nullptr)
			return false;
	}
	return true;
}
void Container::Draw() {

}
int Container::GetSlotWidth(){
	return slotWidth;
}
int Container::GetSlotHeight() {
	return slotHeight;
}