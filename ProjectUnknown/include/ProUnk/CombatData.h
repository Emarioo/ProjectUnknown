#pragma once

#include "Engone/LoopInfo.h"

enum UserType : uint8_t {
	AttackDataType = 1,
	DefenseDataType = 2,
};
class UserData {
public:
	UserData(int type) : type(type) {}
	int type = 0;
};

class engone::GameObject;
class AttackData : public UserData {
public:
	static const int TYPE = 1;
	AttackData() : UserData(TYPE) {}

	engone::GameObject* owner=nullptr;
	float animationTime = 0; // endFrame-currentFrame in seconds
	bool attacking = false;
	void update(engone::UpdateInfo& info) {
		using namespace engone;
		//log::out << "animtime " << animationTime << "\n";
		animationTime -= info.timeStep;
		if (animationTime < 0)
			animationTime = 0;
	}
};
class DefenseData : public UserData {
public:
	static const int TYPE = 2;
	DefenseData() : UserData(TYPE) {}
	float hitCooldown = 0;

	void update(engone::UpdateInfo& info) {
		using namespace engone;
		//log::out << "cooldown " << hitCooldown << "\n";
		hitCooldown -= info.timeStep;
		if (hitCooldown < 0)
			hitCooldown = 0;
	}
};
