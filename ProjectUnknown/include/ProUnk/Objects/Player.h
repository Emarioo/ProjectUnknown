#pragma once

#include "Engone/Engone.h"
#include "Engone/GameObject.h"

#include "ProUnk/Combat/CombatData.h"

namespace prounk {
	class NetGameGround;
	class Player : public engone::GameObject {
	public:
		Player(engone::GameGround* ground);

		void update(engone::UpdateInfo& info) override;
		void WeaponUpdate(engone::UpdateInfo& info);
		void Movement(engone::UpdateInfo& info);
		void Input(engone::UpdateInfo& info);

		// Movement and camera
		float zoom = 3;
		float zoomSpeed = 0;

		NetGameGround* ground = nullptr;

		engone::GameObject* inventorySword = nullptr;
		engone::GameObject* heldWeapon = nullptr;
		
		// info exists as an argument because NetGameGround is required.
		void setWeapon(engone::UpdateInfo& info, engone::GameObject* weapon);

		bool noclip = false;
		bool flight = false;
		void setFlight(bool yes);
		void setNoClip(bool yes);

		float flySpeed = 8.f;
		float flyFastSpeed = 90.f;
		float walkSpeed = 8.f;
		float sprintSpeed = 20.f;
		float jumpForce = 5.f;

		engone::Camera testCam;

		bool onGround = false;

	private:
		float animBlending = 0;
		float animSpeed = 1.7f;

		//-- This code provides a smoother attachment for weapons
		//struct WeaponState {
		//	// before weapon was locked
		//	bool gravity = false;
		//	// now
		//	static const int sampleCount = 3;
		//	int sampleIndex = 0;
		//	glm::vec3 lastVelocities[sampleCount];
		//	glm::vec3 lastPosition;
		//	double lastTime = 0;
		//	void reset() {
		//		for (int i = 0; i < sampleCount; i++) {
		//			lastVelocities[i] = {};
		//		}
		//		lastPosition = {};
		//		lastTime = 0;
		//		sampleIndex = 0;
		//	}
		//	void sample(engone::GameObject* weapon) {
		//		using namespace engone;
		//		if (!weapon)return;
		//		double now = GetSystemTime();
		//		double time = now - lastTime;
		//		if (time == 0) return; // nothing should have happend in 0 seconds.
		//		glm::vec3 pos = ToGlmVec3(weapon->rigidBody->getTransform().getPosition());
		//		if (lastTime != 0) { // skip first time
		//			glm::vec3 diff = pos - lastPosition;
		//			//log::out << (diff / (float)time) << " " << (1 / (float)time) << "\n";
		//			lastVelocities[sampleIndex] = (pos - lastPosition) / (float)time; // casting time to float is fine because it will be a small number
		//			sampleIndex++;
		//			if (sampleIndex > sampleCount)
		//				sampleIndex = 0;
		//		}
		//		lastPosition = pos;
		//		lastTime = now;
		//	}
		//	glm::vec3 sampledVelocity() {
		//		glm::vec3 out{};
		//		for (int i = 0; i < sampleCount; i++) {
		//			out += lastVelocities[i];
		//		}
		//		out /= (float)sampleCount;
		//		return out;
		//	}
		//};
		//WeaponState weaponState;
	};
}