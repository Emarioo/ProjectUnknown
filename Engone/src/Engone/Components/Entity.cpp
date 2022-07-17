
#include "Engone/Components/Entity.h"
//#include "EntitySystem.h"

namespace engone {
	Entity::Entity(ComponentMask mask) : m_mask(mask) {
		
	}
	Entity::Entity(int mask) : m_mask((ComponentMask)mask) {

	}
	Entity::Entity(ComponentMask mask, size_t* sizes, char* ptr) : m_mask(mask), componentSizes(sizes), stackPtr(ptr) {
		
	}
}