
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
	static int sizeOfComponents[32]{ sizeof(Transform), sizeof(Physics), sizeof(ModelRenderer), sizeof(Animator), sizeof(MeshRenderer), sizeof(Collision) };
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
				}
			}

			entityMax = 5;
			data = (char*)malloc(entitySize*entityMax);
			
			if (!data) {
				log::out << log::RED << "Error with entity allocation\n";
				return false;
			}
			ZeroMemory(data, entitySize * entityMax);
		}
		
		if (deletedSpots.size() > 0) {
			int index = deletedSpots.back();
			//log::out << "filled spot " << index << "\n";
			deletedSpots.pop_back();

			entities[index] = entity;
			entity->stackPtr = data + (index)*entitySize;
			ZeroMemory(entity->stackPtr, entitySize);
		} else {
			// increase size if needed
			if (entityCount == entityMax) {
				entityMax *= 2;
				char* ptr = (char*)realloc(data, entitySize * entityMax);

				if (!ptr) {
					log::out << log::RED << "Error with entity allocation\n";
					return false;
				}
				data = ptr;
				ZeroMemory(data + entitySize * entityMax / 2, entitySize * entityMax / 2);

				for (int i = 0; i < entities.size(); i++) {
					Entity* ent = entities[i];
					ent->stackPtr = data + i * entitySize;
				}
			}

			entities.push_back(entity);
			entity->stackPtr = data + (entityCount)*entitySize;
			entityCount++;
		}

		entity->componentSizes = componentSizes;
		
		//-- Component specific
		if (entity->has(Transform::ID)) {
			entity->getComponent<Transform>()->scale = {1,1,1};
		}
		if (entity->has(ModelRenderer::ID)) {
			entity->getComponent<ModelRenderer>()->visible = true;
		}
		if (entity->has(MeshRenderer::ID)) {
			entity->getComponent<MeshRenderer>()->visible = true;
		}
		if (entity->has(Physics::ID)) {
			entity->getComponent<Physics>()->gravity = -9.81;
		}
		
		entity->Init();
		return true;
	}
	bool EntityStack::remove(Entity* entity) {
		
		int index = (int)(entity->stackPtr-data) / entitySize;
		//log::out << "deleted entity index: " << index << "\n";

		deletedSpots.push_back(index);
		entities[index] = nullptr;

		delete entity;
		return true;
	}

	//-- Functions
	bool AddEntity(Entity* entity) {
		if (entity->entityId != 0) {
			log::out << log::RED << "Cannot add existing entity\n";
			return false;
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
			
			for (int i = 0; i < stackCollections.size(); i++) {
				if (stackCollections[i]->has(stack)) {
					stackCollections[i]->push(stack);
				}
			}
		}

		return stack->add(entity);
	}
	bool RemoveEntity(Entity* entity) {
		EntityStack* stack = nullptr;
		for (int i = 0; i < entityStacks.size(); i++) {
			if (entityStacks[i]->same(entity)) {
				stack = entityStacks[i];
				break;
			}
		}
		if (stack != nullptr) {
			if (stack->remove(entity)) {
				return true;
			}
		}
		log::out << log::RED << "Cannot delete entity\n";
		return false;
	}
	void AddSystem(System* system) {
		systems.push_back(system);
	}
	bool RemoveSystem(System* system) {
		for (int i = 0; i < systems.size(); i++) {
			if (systems[i] == system) 				{
				systems.erase(systems.begin()+i);
				return true;
			}
		}
		return false;
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