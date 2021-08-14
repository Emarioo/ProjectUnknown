#include "propch.h"

#include "Container.h"

#include "Keybindings.h"

#include "ItemHandler.h"

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
		}else if (itemArray[i]->GetName() == item->GetName()) {
			int diff = itemArray[i]->type.maxCount - itemArray[i]->count;
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
					if (item->count>item->type.maxCount) {
						itemArray[i] = new Item(item->type.name, item->type.maxCount);// TODO: ERROR: COPY META DATA!
						item->count -= item->type.maxCount;
						totalItemCount += item->type.maxCount;
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
		}
		else if (itemArray[index]->GetName() == item->GetName()) {
			int need = itemArray[index]->type.maxCount - itemArray[index]->count;
			if (need < item->count){
				itemArray[index]->count += need;
				item->count -= need;
				totalItemCount += need;
				return false;
			}
			else {
				itemArray[index]->count += item->count;
				item->count = 0;
				totalItemCount += item->count;
				return true;
			}
		}
	}
	return false;
}
bool Container::HasItem(const std::string& name, int count) {
	for (int i = 0; i < slotWidth * slotHeight; i++) {
		if (itemArray[i] != nullptr) {
			if (itemArray[i]->GetName() == name) {
				count -= itemArray[i]->count;
				if (count < 1)
					return true;
			}
		}
	}
	return false;
}
int Container::GetItemCount(const std::string& name) {
	int out=0;
	for (int i = 0; i < slotWidth * slotHeight; i++) {
		if (itemArray[i] != nullptr) {
			if (itemArray[i]->GetName() == name) {
				out += itemArray[i]->count;
			}
		}
	}
	return out;
}
bool Container::RemoveItem(const std::string& name, int count) {
	for (int i = 0; i < slotWidth * slotHeight;i++) {
		if (itemArray[i]!=nullptr) {
			if (itemArray[i]->GetName() == name) {
				totalItemCount -= itemArray[i]->count;
				int canRemove = count - itemArray[i]->count;
				if (canRemove < 0) {
					itemArray[i]->count -= count;
					return true;
				}
				else {
					count -= itemArray[i]->count;
					itemArray[i] = nullptr;
				}
			}
		}
	}
	return false;
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
Item* Container::GetItemAt(int index) {
	if (0 < index && index < itemArray.size()) {
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
	if (engine::IsKeyActionDown(KeySwitchItem)) {
		Item** temp = a;
		a = b;
		b = temp;
	}
	if (button==0&&action==1) {// Left click - switch 
		if (*a != nullptr && *b != nullptr) {
			if ((*a)->GetName() == (*b)->GetName()) {
				int bNeed = (*b)->type.maxCount - (*b)->count;
				if (bNeed != 0) {
					if ((*a)->count > bNeed) {
						(*b)->count = (*b)->type.maxCount;
						(*a)->count -= bNeed;
					}
					else {
						(*b)->count += (*a)->count;
						delete (*a);
						(*a) = nullptr;
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
				*b = new Item((*a)->GetName(), 1);
				(*a)->count -= 1;
			}
			else {
				*b = *a;
				*a = nullptr;
			}
		} else if (*a == nullptr && *b != nullptr) {
			if ((*b)->count > 1) {
				*a = new Item((*b)->GetName(), (*b)->count / 2);
				(*b)->count -= (*b)->count / 2;
			}
		}
		else if (*a != nullptr && *b != nullptr) {
			if ((*a)->GetName() == (*b)->GetName()) {
				if ((*a)->count > 1) {
					(*b)->count++;
					(*a)->count--;
				}
				else {
					(*b)->count++;
					delete *a;
					*a = nullptr;
				}
			}
		}
	}
}
int Container::EmptySlots() {
	int slots=0;
	for (int i = 0; i < slotWidth * slotHeight;i++) {
		if (itemArray[i] == nullptr)
			slots++;
	}
	return slots;
}
int Container::GetSlotWidth(){
	return slotWidth;
}
int Container::GetSlotHeight() {
	return slotHeight;
}