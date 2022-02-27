#pragma once

#include "Entity.h"

namespace engone {

	// Entity stack holds a specific entities components
	class EntityStack : public ComponentMask {
	private:
		ComponentMask operator|(ComponentEnum b) {};
	public:
		EntityStack() = default;
		EntityStack(ComponentMask me) : ComponentMask(me) {}
		~EntityStack() {
			entityMax = 0;
			entitySize = 0;
			entityCount = 0;
			free(data);
		}
		int componentCount = 0;
		int componentSizes[32];
		int entityCount = 0, entityMax=0;
		int entitySize = 0;
		char* data=nullptr;// not an array of components. it is an array of specific components in a specific order
		// example: transform, physics | transform, physics...
		
		bool add(Entity* entity);

		template <class T>
		T* getComponent(int index) {
			//log::out << componentMask << " - " << (1<<((int)T::MASK-1)) << "\n";
			if (has(T::MASK)) {
				int typeIndex = ((int)T::MASK) - 1;
				return (T*)(data + index * entitySize + componentSizes[typeIndex]);
			}
			return nullptr;
		}
	};
	class StackCollection : public ComponentMask {
	private:
		ComponentMask operator|(ComponentEnum b) {};
	public:
		StackCollection() = default;
		StackCollection(ComponentMask me) : ComponentMask(me) {}

		std::vector<EntityStack*> stacks;

		void push(EntityStack* entity) {
			stacks.push_back(entity);
		}
		void clear() {
			stacks.clear();
		}
		
	};
	class EntityIterator {
	public:
		EntityIterator(StackCollection* stack) : collection(stack) {};

		int index = -1;
		int stackIndex = 0;
		StackCollection* collection=nullptr;

		bool next() {
			if (collection->stacks.size()==0)
				return false;
			index++;
			if (index == collection->stacks[stackIndex]->entityCount) {
				index = 0;
				stackIndex++;
			}
			if (stackIndex == collection->stacks.size())
				return false;
			return true;
		}
		operator bool() {
			return next();
		}

		template<class T>
		T* get() {
			return collection->stacks[stackIndex]->getComponent<T>(index);
		}
		Entity getEntity() {
			return {collection,collection->stacks[stackIndex]->componentSizes,
				collection->stacks[stackIndex]->data + 
				(collection->stacks[stackIndex]->entityCount - 1) * collection->stacks[stackIndex]->entitySize };
		}
	};

	// Adds the entity to the entity component system, also allocates memory for entity
	void AddEntity(Entity* entity);
	
	// Get a collection of component stacks which have the specified components
	EntityIterator GetEntityIterator(ComponentMask mask);
}