#pragma once

#include "Engone/EngineObject.h"

#include "ProUnk/World.h"

#include "ProUnk/Combat/Tags.h"

namespace prounk {

	// Since the game is based on a registry, entity component and more data oriented there is not a Player EngineObject class.
	// This will be the replacement for that.
	class PlayerController : public rp3d::RaycastCallback {
	public:
		PlayerController();

		// attach/set player object
		void setPlayerObject(engone::EngineObject* player);
		engone::EngineObject* getPlayerObject() { return m_player; }
		engone::EngineObject* m_player=nullptr;

		void update(engone::LoopInfo& info);
		void Movement(engone::LoopInfo& info);
		void Input(engone::LoopInfo& info);

		// This updates the position of the weapon when held
		void WeaponUpdate(engone::LoopInfo& info);

		rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo& raycastInfo) override;

		// Movement and camera
		float zoom = 3;
		float zoomSpeed = 0;

		inline void setWorld(World* world) { m_world = world; }
		World* m_world = nullptr;

		// temporary
		engone::EngineObject* inventorySword = nullptr;
		
		// object held in hand
		engone::EngineObject* heldObject = nullptr;

		// info exists as an argument because NetGameGround is required.
		void setWeapon(engone::LoopInfo& info, engone::EngineObject* weapon);

		bool noclip = false;
		bool flight = false;
		void setFlight(bool yes);
		void setNoClip(bool yes);

		float deathTime = 0;
		float deathShockStrength = 1.f; // how much the player "jumps" when it dies.
		//bool isDead = false;
		void setDead(bool yes) {
			if (yes)
				m_player->flags |= OBJECT_IS_DEAD;
			else
				m_player->flags &= ~OBJECT_IS_DEAD;
		}
		bool isDead() {
			return m_player->flags & OBJECT_IS_DEAD;
		}

		float flySpeed = 8.f;
		float flyFastSpeed = 90.f;
		float walkSpeed = 8.f;
		float sprintSpeed = 15.f;
		float jumpForce = 10.f;

		//engone::Camera testCam;

		bool onGround = false;

	private:
		float animBlending = 0;
		float animSpeed = 1.7f;

	};
}