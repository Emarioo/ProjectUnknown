#include "Item.h"

Item::Item() {

}
Item::Item(const std::string& name)
	: name(name), count(1) {
	
}
Item::Item(const std::string& name, int count)
	: name(name), count(count) {

}
const std::string& Item::GetName() {
	return name;
}