#include "Item.h"

#include "ItemHandler.h"

Item::Item() {

}
Item::Item(const std::string& name)
	: type(GetItemType(name)), count(1) {
	
}
Item::Item(const std::string& name, int count)
	: type(GetItemType(name)), count(count) {

}
void Item::SetItem(const std::string& name, int count) {
	type = GetItemType(name);
	count = count;
}
std::string Item::GetName() {
	return type.name;
}