#pragma once

#include "Engone/EngineObject.h"
#include "Engone/Networking/NetworkModule.h"
#include "Engone/ParticleModule.h"

namespace engone {

	class EngineWorld;
	// supposed to be safe from multi-threading
	// it's not at the moment
	// Also be careful when deleting objects within the while loop when using the iterator.
	// deleting the returned object of next in a sub function and then trying to use the object
	// in the while loop will cause a crash.
	class EngineObjectIterator {
	public:
		// list CANNOT be nullptr. crash is bound to happen.
		EngineObjectIterator(EngineWorld* world) : m_world(world) {}

		EngineObject* next();
		void restart();

	private:
		int m_index = 0;
		EngineWorld* m_world = 0;

		friend class EngineWorld;
	};

	class Application;
	// Template for game ground.
	// you need to make a class which derives from this and add a list which stores game objects.
	// The game objcets may be a custom class but has to inherit EngineObject.
	class EngineWorld {
	public:
		EngineWorld();
		EngineWorld(Application* app);
		~EngineWorld();
		void cleanup();

		//void addObject(EngineObject* object);

		// Creates a new object with rigidbody and sets the rigidbody's user data to the engine object.
		// Always use this function when making objects.
		EngineObject* createObject(UUID);
		
		// Remeber to delete iterator with deleteIterator
		// Allocation could be done with an Arena Allocator
		EngineObjectIterator* createIterator();
		void deleteIterator(EngineObjectIterator* iterator);
		// Should return nullptr if out of bounds
		EngineObject* getObjectAt(int index);
		EngineObject* getObject(UUID uuid);
		int getObjectCount();

		void deleteObject(EngineObject* object);
		void deleteObjectAt(int index);

		void addParticleGroup(ParticleGroupT* group);

		inline std::vector<ParticleGroupT*>& getParticleGroups() { return m_particleGroups; };

		void update(LoopInfo& info);

		// does a raycast and returns an array with hit objects.
		// object limit determines how many objects.
		// ignoreObjectType will skip objects of that type. -1 can be seen as skipping no objects
		// Altough if an object has that has type then it will be skipped.
		std::vector<EngineObject*> raycast(rp3d::Ray ray, int objectLimit = 1, int ignoreObjectType=-1);

		rp3d::PhysicsCommon* getPhysicsCommon();
		rp3d::PhysicsWorld* getPhysicsWorld();

		Application* getApp() { return m_app; }

		void* getUserData() { return m_userData; }
		void setUserData(void* ptr) { m_userData=ptr; }

		DepthMutex m_mutex; // mutex for objects
		void lock() {
			m_mutex.lock();
		}
		void unlock() {
			m_mutex.unlock();
		}

	protected:
#ifdef ENGONE_PHYSICS
		rp3d::PhysicsWorld* m_physicsWorld = nullptr;
#endif

		std::vector<EngineObjectIterator*> m_iterators;

		// you may want to make an entity component system or something
		std::vector<ParticleGroupT*> m_particleGroups;
		std::vector<EngineObject*> m_objects;

		Application* m_app=nullptr;

		void* m_userData=nullptr;

		friend class Application;
	};
}

