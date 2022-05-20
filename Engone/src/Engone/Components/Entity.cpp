
#include "Entity.h"
//#include "EntitySystem.h"

namespace engone {
	Entity::Entity(ComponentMask mask) : ComponentMask(mask) {
		
	}
	Entity::Entity(ComponentMask mask, size_t* sizes, char* ptr) : ComponentMask(mask), componentSizes(sizes), stackPtr(ptr) {
		
	}
}