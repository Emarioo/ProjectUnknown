
#include "Engone/Components/EntitySystem.h"

#include "Engone/Logger.h"

namespace engone {

	//-- Data
	static std::vector<EntityStack*> entityStacks; // a list of all components data
	// previously gathered stacks which can be reused unless a new entity or component has entered the system
	static std::vector<StackCollection*> stackCollections;
	// entities that exist
	static std::vector<Entity*> allEntities;
	static uint64_t entityCount;
	
	//-- Components
	static size_t sizeOfComponents[8]{ sizeof(Animator), sizeof(CollisionBody),sizeof(MeshRenderer),sizeof(ModelRenderer),sizeof(RigidBody)};
	static size_t componentEnumCount=8;

	//-- System
	//static std::vector<System*> systems;
	
	//-- Methods
	bool EntityStack::add(Entity* entity) {
		if (data == nullptr) {
			ComponentMask eMask = entity->getMask();
			for (size_t i = 0; i < componentEnumCount;i++) {
				componentSizes[i] = entitySize;
				if (eMask&(1<<(i))) {
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
			size_t index = deletedSpots.back();
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

				for (size_t i = 0; i < entities.size(); i++) {
					Entity* ent = entities[i];
					ent->stackPtr = data + i * entitySize;
				}
			}

			entities.push_back(entity);
			entity->stackPtr = data + (entityCount)*entitySize;
			entityCount++;
		}

		entity->componentSizes = componentSizes;
		
		entity->Init();
		return true;
	}
	bool EntityStack::remove(Entity* entity) {
		
		size_t index = (size_t)(entity->stackPtr-data) / entitySize;
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
		for (size_t i = 0; i < entityStacks.size();++i) {
			if (entityStacks[i]->getMask()==entity->getMask()) {
				stack = entityStacks[i];
				break;
			}
		}
		// create stack
		if (stack==nullptr) {
			stack = new EntityStack(entity->getMask());
			entityStacks.push_back(stack);
			
			for (size_t i = 0; i < stackCollections.size(); ++i) {
				if (stackCollections[i]->getMask()&stack->getMask()) {
					stackCollections[i]->push(stack);
				}
			}
		}
		bool yes = stack->add(entity);
		return yes;
	}
	bool RemoveEntity(Entity* entity) {
		EntityStack* stack = nullptr;
		for (size_t i = 0; i < entityStacks.size(); ++i) {
			if (entityStacks[i]->getMask()==entity->getMask()) {
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
	//void AddSystem(System* system) {
	//	systems.push_back(system);
	//}
	//bool RemoveSystem(System* system) {
	//	for (size_t i = 0; i < systems.size(); ++i) {
	//		if (systems[i] == system) 				{
	//			systems.erase(systems.begin()+i);
	//			return true;
	//		}
	//	}
	//	return false;
	//}
	//void RunUpdateSystems(float delta) {
	//	for (System* system : systems) {
	//		system->OnUpdate(delta);
	//	}
	//}
	EntityIterator GetEntityIterator(ComponentMask mask){
		// If collection exists
		for (size_t i = 0; i < stackCollections.size(); ++i) {
			if (stackCollections[i]->getMask()==mask) {
				return stackCollections[i];
			}
		}
		// Make new collection
		
		stackCollections.push_back(new StackCollection(mask));

		// Fill collection
		for (size_t i = 0; i < entityStacks.size(); ++i) {
			EntityStack* stack = entityStacks[i];
			if (stack->getMask()&mask) {
				stackCollections.back()->push(stack);
			}
		}
		return stackCollections.back();
	}
}