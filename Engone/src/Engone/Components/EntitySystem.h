#pragma once

#include "Entity.h"
#include "System.h"

namespace engone {

	// Entity stack holds a specific entities components
	class EntityStack : public ComponentMask {
	private:
		ComponentMask operator|(ComponentEnum b) {};
	public:
		//EntityStack() = default;
		EntityStack(ComponentMask me) : ComponentMask(me) {
			//memset((char*)componentSizes,0,32*sizeof(int));
		}
		~EntityStack() {
			entityMax = 0;
			entitySize = 0;
			entityCount = 0;
			if (data)
				free(data);
		}
		size_t componentSizes[32];
		size_t entityCount = 0, entityMax=0;
		size_t entitySize = 0;
		std::vector<Entity*> entities;
		std::vector<size_t> deletedSpots;
		char* data = nullptr; // an array of specific components in a specific order
		// example: transform, physics | transform, physics...
		
		bool add(Entity* entity);
		bool remove(Entity* entity);

		template <class T>
		T* getComponent(int index) {
			if (has(T::ID)) {
				size_t typeIndex = ((size_t)T::ID) - 1;
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
	/*
		Iterator can be reused if the iteration wasn't interrupted. If it was use revert().
	*/
	class EntityIterator {
	public:
		EntityIterator(StackCollection* stack) : collection(stack) {};

		bool next() {
			if (collection->stacks.size()==0)
				return false;
			
			while (true) {
				index++;
				bool cont = false;
				for (size_t i = 0; i < collection->stacks[stackIndex]->deletedSpots.size(); i++) {
					size_t ind = collection->stacks[stackIndex]->deletedSpots[i];
					if (index == ind) {
						cont = true;
						break;
					}
				}
				if (cont) continue;
				
				if (index >= collection->stacks[stackIndex]->entityCount) {
					index = 0;
					stackIndex++;
				}
				if (stackIndex >= collection->stacks.size()) {
					index = -1; // Reset values for reusability
					stackIndex = 0;
					return false;
				}
				return true;
			}
			return true;
		}
		operator bool() {
			return next();
		}
		// Start the iterator from the beginning (index=0)
		void revert() {
			index = -1;
			stackIndex = 0;
		}

		// will crash if next hasn't been called before
		template<class T>
		T* get() {
			return collection->stacks[stackIndex]->getComponent<T>(index);
		}
		// will crash if next hasn't been called before
		Entity getEntity() {
			return {collection,collection->stacks[stackIndex]->componentSizes,
				collection->stacks[stackIndex]->data + 
				(collection->stacks[stackIndex]->entityCount - 1) * collection->stacks[stackIndex]->entitySize };
		}
	private:
		size_t index = -1;
		size_t stackIndex = 0;
		StackCollection* collection = nullptr;
	};

	// Adds the entity to the entity component system, also allocates memory for entity
	bool AddEntity(Entity* entity);
	bool RemoveEntity(Entity* entity);
	// Adds the system to entity component system.
	void AddSystem(System* system);
	bool RemoveSystem(System* system);
		
	void RunUpdateSystems(float delta);
	void RunCollisionSystems(Collider& c1, Collider& c2);

	// Get a collection of component stacks which have the specified components
	EntityIterator GetEntityIterator(ComponentMask mask);

}