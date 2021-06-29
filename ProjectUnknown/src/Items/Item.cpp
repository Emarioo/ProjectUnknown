#include "Item.h"

#include "ItemHandler.h"

Item::Item() {

}
Item::Item(ItemName name)
	: name(name), count(1) {
	FillItemWithData(this);
}
Item::Item(ItemName name, int count)
	: name(name), count(count) {
	FillItemWithData(this);
}
ItemName Item::GetName() {
	return name;
}