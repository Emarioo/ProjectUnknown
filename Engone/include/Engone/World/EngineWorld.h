#pragma once

#include "Engone/EngineObject.h"
#include "Engone/Networking/NetworkModule.h"
#include "Engone/ParticleModule.h"

namespace engone {

	class EngineWorld;
	// supposed to be safe from multi-threading
	// it's not at the moment
	class EngineObjectIterator {
	public:
		// list CANNOT be nullptr. crash is bound to happen.
		EngineObjectIterator(EngineWorld* world) : m_world(world) {}

		EngineObject* next();
		void popCurrent();

	private:
		int m_index = 0;
		EngineWorld* m_world = 0;
	};

	class Application;
	// Template for game ground.
	// you need to make a class which derives from this and add a list which stores game objects.
	// The game objcets may be a custom class but has to inherit EngineObject.
	class EngineWorld {
	public:
		EngineWorld();
		//EngineWorld(Application* app);
		virtual ~EngineWorld();
		virtual void cleanup();

		void addObject(EngineObject* object);
		
		EngineObjectIterator getIterator();
		// Should return nullptr if out of bounds
		EngineObject* getObjectAt(int index);
		EngineObject* getObject(UUID uuid);
		int getObjectCount();

		void deleteObject(EngineObject* object);
		void deleteObjectAt(int index);

		void addParticleGroup(ParticleGroupT* group);

		inline std::vector<ParticleGroupT*>& getParticleGroups() { return m_particleGroups; };

		virtual void update(LoopInfo& info);

		Application* getApp() { return m_app; }

#ifdef ENGONE_PHYSICS
		rp3d::PhysicsWorld* m_pWorld = nullptr;
		rp3d::PhysicsCommon* m_pCommon = nullptr;
#endif

		DepthMutex m_mutex; // mutex for objects
		void lock() {
			m_mutex.lock();
		}
		void unlock() {
			m_mutex.unlock();
		}

	protected:
		// you may want to make an entity component system or something
		std::vector<ParticleGroupT*> m_particleGroups;
		std::vector<EngineObject*> m_objects;

		Application* m_app=nullptr;
	};
}

