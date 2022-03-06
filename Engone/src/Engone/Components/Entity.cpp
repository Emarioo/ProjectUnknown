#include "gonpch.h"

#include "Entity.h"
//#include "EntitySystem.h"

namespace engone {
	Entity::Entity(ComponentMask mask) : ComponentMask(mask) {
		
	}
	Entity::Entity(ComponentMask mask, int* sizes, char* ptr) : ComponentMask(mask), componentSizes(sizes), stackPtr(ptr) {
		
	}
}