#pragma once

#include "Engone/EngineObject.h"

// #include "ProUnk/Combat/Tags.h"

#include "ProUnk/Registries/InventoryRegistry.h"

namespace prounk {

	// Since the game is based on a registry, entity component and more data oriented there is not a Player EngineObject class.
	// This will be the replacement for that.
	class PlayerController {
	public:
		PlayerController();

		// attach/set player object
		void setPlayerObject(engone::UUID uuid);
		engone::UUID playerId=0;

		void update(engone::LoopInfo* info, engone::EngineObject* player);
		void render(engone::LoopInfo* info, engone::EngineObject* player);
		void Movement(engone::LoopInfo* info, engone::EngineObject* player);
		void Input(engone::LoopInfo* info, engone::EngineObject* player);

		void performSkills(engone::LoopInfo* info, engone::EngineObject* player);
		void simpleTest(engone::LoopInfo* info);
		// This updates the position of the weapon when held

		void DetectHeldWeapon(engone::LoopInfo* info, engone::EngineObject* player);
		void UpdateWeaponTransform(engone::LoopInfo* info, engone::EngineObject* player);

		std::string hoveredItem;

		// object the player is looking at
		engone::EngineObject* getSeenObject(engone::LoopInfo* info,engone::EngineObject* player);

		// turns the object into a chicken. no haha.
		// it turns the object into an item and destroys the object.
		// things may fail.
		bool pickupItem(engone::EngineObject* player, engone::EngineObject* object);

		// drops the item in the first slot
		void dropItem(int slotIndex, int count, engone::EngineObject* player);

		void dropAllItems(engone::EngineObject* player);
		bool keepInventory=false;
		
		// Movement and camera
		float zoom = 3;
		float zoomSpeed = 0;

		glm::vec3 camOffset = {0,2.4f,0}; // offset from player position
		float offsetTransitionTime = 0;
		float offsetTransitionTimeMax = 0.1;

		// object held in hand
		//engone::EngineObject* heldObject = nullptr;
		// engone::UUID heldObjectId = 0;
		// engone::EngineObject* getHeldObject();

		// info exists as an argument because NetGameGround is required.
		//void setWeapon(engone::EngineObject* weapon);

		bool noclip = false;
		bool flight = false;
		void setFlight(bool yes, engone::EngineObject* player);
		void setNoClip(bool yes, engone::EngineObject* player);

		float deathTime = 0;
		float jumpTime = 0;
		
		static constexpr float deathShockStrength = 1.f; // how much the player "jumps" when they die.
		static constexpr float deathItemShockStrength = 1.f; // how much items fly on death.

		static constexpr float flySpeed = 8.f;
		static constexpr float flyFastSpeed = 90.f;
		static constexpr float walkSpeed = 8.f;
		static constexpr float sprintSpeed = 15.f;
		// static constexpr float jumpForce = 17.f;
		static constexpr float jumpForce = 50.f;
		static constexpr float jumpDelay = 0.4;

		ItemType lastItemType=0;

		bool followTargetRotation=false;
		float targetRotation=0;

		//engone::Camera testCam;

		// GameApp* app=nullptr;

	private:
		float animBlending = 0;
		float animSpeed = 1.7f;

	};
}