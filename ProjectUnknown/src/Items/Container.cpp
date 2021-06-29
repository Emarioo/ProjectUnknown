#include "Container.h"

#include "Engine/Keyboard.h"

#include <iostream>

#include <GLFW/glfw3.h>

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
				totalItemCount += diff;
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
						totalItemCount += item->maxCount;
					} else {
						itemArray[i] = item;
						totalItemCount += item->count;
						return true;
					}
				}
			}
		} else
			return false;
	} else
		return true;
	return false;
}
bool Container::AddItemAt(Item* item, int slotX, int slotY) {
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
Item* Container::TakeItem(ItemName name) {
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
Item* Container::TakeItemAt(int slotX, int slotY) {
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
Item* Container::GetItemAt(int slotX, int slotY) {
	if (slotWidth > slotX && slotHeight > slotY) {
		int index = slotY * slotWidth + slotX;
		return itemArray[index];
	}
	return nullptr;
}
Item** Container::GetItemPointerAt(int slotX, int slotY) {
	if (slotWidth > slotX && slotHeight > slotY) {
		int index = slotY * slotWidth + slotX;
		return &itemArray[index];
	}
	return nullptr;
}
// 0 - left, 1 - right, 2 - mid
void Container::SwitchItem(Item** a, Item** b,int button, int action) {
	if (a == nullptr || b == nullptr) {
		//bug::out < "SwitchItemLocation pointer is nullptr" < bug::end;
		return;
	}
	if (engine::IsKey(GLFW_KEY_LEFT_SHIFT)) {
		Item** temp = a;
		a = b;
		b = temp;
	}
	if (button==0&&action==1) {// Left click - switch 
		if (*a != nullptr && *b != nullptr) {
			if ((*a)->GetName() == (*b)->GetName()) {
				int bNeed = (*b)->maxCount - (*b)->count;
				if (bNeed != 0) {
					if ((*a)->count > bNeed) {
						(*b)->count = (*b)->maxCount;
						(*a)->count -= bNeed;
					}
					else {
						(*b)->count += (*a)->count;
						(*a) = nullptr;
						//delete (*a); LEAK!!!
					}
					return;
				}
			}
		}
		Item* temp = *a;
		*a = *b;
		*b = temp;
	} else if(button==1&&action==1) { // Right click - split
		if (*a!=nullptr&&*b==nullptr) {
			if ((*a)->count > 1) {
				*b = new Item((*a)->GetName(), (*a)->count / 2.f);
				(*a)->count -= (*a)->count / 2.f;
			}
		} else if (*a == nullptr && *b != nullptr) {
			if ((*b)->count > 1) {
				*a = new Item((*b)->GetName(), (*b)->count / 2);
				(*b)->count -= (*b)->count / 2;
			}
		}
	}
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
int Container::GetSlotWidth(){
	return slotWidth;
}
int Container::GetSlotHeight() {
	return slotHeight;
}