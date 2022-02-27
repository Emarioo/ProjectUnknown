#include "gonpch.h"

#include "EntitySystem.h"

#include "../Logger.h"

namespace engone {

	//-- Data
	static std::vector<EntityStack> entityStacks; // a list of all components data
	// previously gathered stacks which can be reused unless a new entity or component has entered the system
	static std::vector<StackCollection> stackCollections;
	// entities that exist
	static std::vector<Entity*> allEntities;
	static uint64_t entityCount;

	int sizeOfComponents[32]{ sizeof(Transform), sizeof(Physics),sizeof(Scriptable), sizeof(Model), sizeof(Animator) };
	int componentEnumCount=32;
	
	//-- Methods
	bool EntityStack::add(Entity* entity) {
		if (data == nullptr) {
			for (int i = 0; i < componentEnumCount;i++) {
				componentSizes[i] = entitySize;
				if (has((ComponentEnum)(i+1))) {
					entitySize += sizeOfComponents[i];
					//log::out << "csize "<<i<<" "<<componentSizes[i] << "\n";
				}
			}

			entityMax = 5;
			data = (char*)malloc(entitySize*entityMax);
			ZeroMemory(data, entitySize * entityMax);

			//log::out <<"data size "<< entitySize << "\n";
		}
		// increase size if needed
		if (entityCount == entityMax) {
			entityMax *= 2;
			char* ptr  = (char*)realloc(data, entitySize * entityMax);
			if (!ptr)
				return false;

			data = ptr;
			ZeroMemory(data + entitySize * entityMax/2, entitySize * entityMax / 2);
		}

		entityCount++;
		return true;
	}

	//-- Functions
	void AddEntity(Entity* entity) {
		if (entity->entityId != 0) {
			log::out << log::RED << "Cannot add existing entity\n";
			return;
		}

		entity->entityId = ++entityCount;
		allEntities.push_back(entity);

		//log::out << "new entity " << entityCount << "\n";

		// find entity stack
		EntityStack* stack=nullptr;
		for (int i = 0; i < entityStacks.size();i++) {
			if (entityStacks[i].same(entity)) {
				stack = &entityStacks[i];
				break;
			}
		}

		if (stack==nullptr) {
			entityStacks.push_back(EntityStack(entity));
			//log::out << "new stack "<<entity->componentMask << " " << entityStacks.back().componentMask << "\n";
			stack = &entityStacks.back();
			
			for (int i = 0; i < stackCollections.size(); i++) {
				if (stackCollections[i].has(entity)) {
					stackCollections[i].push(stack);
				}
			}
		}

		bool yes = stack->add(entity);
		if (yes) {
			entity->componentSizes = stack->componentSizes;
			entity->stackPtr = stack->data + (stack->entityCount - 1)* stack->entitySize;

			//-- Component specific
			if (entity->has(ComponentEnum::Scriptable)) {
				log::out << "id " << entity->entityId << "\n";
				entity->getComponent<Scriptable>()->entity = entity;
			}

			entity->Init();
		} else {
			log::out << log::RED << "Error with entity stack memory\n";
		}

	}
	EntityIterator GetEntityIterator(ComponentMask mask){
		int index=-1;

		// If collection exists
		for (int i = 0; i < stackCollections.size(); i++) {
			if (stackCollections[i].same(mask)) {
				return &stackCollections[i];
			}
		}
		// Make new collection
		
		stackCollections.push_back(StackCollection(mask));

		// Fill collection
		for (int i = 0; i < entityStacks.size(); i++) {
			EntityStack* stack = &entityStacks[i];
			if (stack->has(mask)) {
				stackCollections.back().push(stack);
			}
		}
		return &stackCollections.back();
	}
}