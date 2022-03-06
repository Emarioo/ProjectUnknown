#include "gonpch.h"

#include "EntitySystem.h"

#include "../Logger.h"

namespace engone {

	//-- Data
	static std::vector<EntityStack*> entityStacks; // a list of all components data
	// previously gathered stacks which can be reused unless a new entity or component has entered the system
	static std::vector<StackCollection*> stackCollections;
	// entities that exist
	static std::vector<Entity*> allEntities;
	static uint64_t entityCount;
	
	//-- Components
	static int sizeOfComponents[32]{ sizeof(Transform), sizeof(Physics), sizeof(ModelRenderer), sizeof(Animator), sizeof(MeshRenderer) };
	static int componentEnumCount=32;

	//-- System
	static std::vector<System*> systems;
	
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
			//log::out << "malloc " << (void*)data<< "\n";
			
			if (!data) {
				log::out << log::RED << "Error with entity stack memory\n";
				return false;
			}
			ZeroMemory(data, entitySize * entityMax);

			//log::out <<"data ptr "<<(void*)data << "\n";
		}
		
		// increase size if needed
		if (entityCount == entityMax) {
			entityMax *= 2;
			
			char* ptr  = (char*)realloc(data, entitySize*entityMax);

			log::out << (void*)ptr << "\n";
			if (!ptr) {
				log::out << log::RED << "Error with entity stack memory\n";
				return false;
			}
			data = ptr;
			ZeroMemory(data + entitySize * entityMax/2, entitySize * entityMax / 2);

			for (int i = 0; i < entities.size(); i++) {
				Entity* ent = entities[i];
				ent->stackPtr = data + i * entitySize;
			}
		}
		
		entities.push_back(entity);

		entity->componentSizes = componentSizes;
		entity->stackPtr = data + (entityCount) * entitySize;
		
		//-- Component specific
		if (entity->has(ComponentEnum::ModelRenderer)) {
			entity->getComponent<ModelRenderer>()->visible = true;
		}
		if (entity->has(ComponentEnum::MeshRenderer)) {
			entity->getComponent<MeshRenderer>()->visible = true;
		}
		if (entity->has(ComponentEnum::Physics)) {
			entity->getComponent<Physics>()->gravity = -9.81;
		}
		

		entity->Init();

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

		// find entity stack
		EntityStack* stack=nullptr;
		for (int i = 0; i < entityStacks.size();i++) {
			if (entityStacks[i]->same(entity)) {
				stack = entityStacks[i];
				break;
			}
		}
		// create stack
		if (stack==nullptr) {
			stack = new EntityStack(entity);
			entityStacks.push_back(stack);
			//log::out << "new stack "<<entity->componentMask << " " << entityStacks.back().componentMask << "\n";
			//log::out <<"new stack ptr "<< (void*)stack << "\n";
			
			/*
			for (int i = 0; i < stackCollections.size(); i++) {
				if (stackCollections[i].has(entity)) {
					stackCollections[i].push(stack);
				}
			}*/
		}

		//log::out << "data ptr " << (void*)stack->data << "\n";
		//if (stack->data)
			//stack->data=(char*)realloc(stack->data,100);
		stack->add(entity);
	}
	void AddSystem(System* system) {
		systems.push_back(system);
	}
	void RunUpdateSystems(float delta) {
		for (System* system : systems) {
			system->OnUpdate(delta);
		}
	}
	void RunCollisionSystems(Collider& c1, Collider& c2) {
		for (System* system : systems) {
			system->OnCollision(c1,c2);
		}
	}
	EntityIterator GetEntityIterator(ComponentMask mask){
		// If collection exists
		for (int i = 0; i < stackCollections.size(); i++) {
			if (stackCollections[i]->same(mask)) {
				return stackCollections[i];
			}
		}
		// Make new collection
		
		stackCollections.push_back(new StackCollection(mask));

		// Fill collection
		for (int i = 0; i < entityStacks.size(); i++) {
			EntityStack* stack = entityStacks[i];
			if (stack->has(mask)) {
				stackCollections.back()->push(stack);
			}
		}
		return stackCollections.back();
	}
}