#pragma once

#include "Engone/EngineObject.h"

#include "ProUnk/Combat/Tags.h"

#include "ProUnk/Registries/InventoryRegistry.h"

namespace prounk {

	class GameApp;
	// Since the game is based on a registry, entity component and more data oriented there is not a Player EngineObject class.
	// This will be the replacement for that.
	class PlayerController {
	public:
		PlayerController();

		// attach/set player object
		void setPlayerObject(engone::UUID player);
		//void setPlayerObject(engone::EngineObject* player);
		//engone::EngineObject* getPlayerObject() { return m_player; }
		//engone::EngineObject* m_player=nullptr;
		engone::UUID m_playerId=0;

		engone::EngineObject* requestPlayer();
		void releasePlayer(engone::EngineObject* plr);
		engone::EngineObject* requestHeldObject();
		void releaseHeldObject(engone::EngineObject* held);

		void update(engone::LoopInfo& info);
		void render(engone::LoopInfo& info);
		void Movement(engone::LoopInfo& info);
		void Input(engone::LoopInfo& info);

		void performSkills(engone::LoopInfo& info);

		// This updates the position of the weapon when held
		void WeaponUpdate(engone::LoopInfo& info);

		std::string hoveredItem;

		// object the player is looking at
		engone::EngineObject* getSeenObject();

		Inventory* getInventory();

		// turns the object into a chicken. no haha.
		// it turns the object into an item and destroys the object.
		// things may fail.
		bool pickupItem(engone::EngineObject* object);

		// drops the item in the first slot
		void dropItem(int slotIndex, int count);

		void dropAllItems();
		bool keepInventory=false;
		
		// Movement and camera
		float zoom = 3;
		float zoomSpeed = 0;

		glm::vec3 camOffset = {0,2.4f,0}; // offset from player position

		// object held in hand
		//engone::EngineObject* heldObject = nullptr;
		engone::UUID heldObjectId = 0;

		// info exists as an argument because NetGameGround is required.
		//void setWeapon(engone::EngineObject* weapon);

		bool noclip = false;
		bool flight = false;
		void setFlight(bool yes);
		void setNoClip(bool yes);

		float deathTime = 0;
		float deathShockStrength = 1.f; // how much the player "jumps" when they die.
		float deathItemShockStrength = 1.f; // how much items fly on death.
		void setDead(bool yes);
		bool isDead();

		float flySpeed = 8.f;
		float flyFastSpeed = 90.f;
		float walkSpeed = 8.f;
		float sprintSpeed = 15.f;
		float jumpForce = 10.f;

		ItemType lastItemType=0;

		//engone::Camera testCam;

		bool onGround = false;

		GameApp* app=nullptr;

	private:
		float animBlending = 0;
		float animSpeed = 1.7f;

	};
}