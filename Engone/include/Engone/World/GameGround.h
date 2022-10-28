#pragma once

#include "Engone/GameObject.h"
#include "Engone/Networking/NetworkModule.h"
#include "Engone/ParticleModule.h"

namespace engone {
	// contains players, worlds, monsters
	// two types, one which updates objects, one which talks with the main
	// filter what should be specific and not specific

	class Application;
	class GameGround {
	public:
		GameGround() = default;
		GameGround(Application* app);
		virtual ~GameGround();
		virtual void cleanup();

		void addParticleGroup(ParticleGroupT* group);
		// GameGround would be responsible for deleting GameObject. (you should not call delete on object)
		void addObject(GameObject* object);
		//void removeObject(GameObject* object);

		GameObject* getObject(UUID uuid);

		inline std::vector<GameObject*>& getObjects() { return m_objects; };
		inline std::vector<ParticleGroupT*>& getParticleGroups() { return m_particleGroups; };

		virtual void update(UpdateInfo& info);

#ifdef ENGONE_PHYSICS
		rp3d::PhysicsWorld* m_pWorld = nullptr;
		rp3d::PhysicsCommon* m_pCommon = nullptr;
#endif

		std::mutex m_mutex; // mutex for objects

	protected:
		std::vector<GameObject*> m_objects;
		std::vector<ParticleGroupT*> m_particleGroups;

		Application* m_app=nullptr;
	};
}

