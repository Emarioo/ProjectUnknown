#pragma once

#include "Engone/AssetModule.h"

enum ForceType : int {
	ForceTypeNone = 0,
	ForceTypeAttractive=1,
	ForceTypeFriction=2,
	ForceTypeField=3,
};
struct FocalPoint {
	FocalPoint() : forceType(ForceTypeNone) {}
	FocalPoint(ForceType type, glm::vec3 pos, glm::vec3 vel, float strength, float min, float max) : 
		forceType(type), position(pos),velocity(vel),strength(strength),rangeMin(min),rangeMax(max) {}

	ForceType forceType = ForceTypeNone;
	glm::vec3 position = {0,0,0};
	glm::vec3 velocity = { 0,0,0 };
	float strength = 1;
	float rangeMin=0;
	float rangeMax=999;
	float rangeBlend=0; // is instant effect, 1 is

	float lifeTime;
 
	void bind(engone::Shader* shader, int index) {
		//if (index > 9) {
		//	engone::log::out << engone::log::RED << "FocalPoint::bind - index higher than 9 hasn't been implemented\n";
		//}
		shader->bind();
		//char str[30];
		//memset(str, 0, sizeof(str));
		//const char* uName = "uFocalPoints[i].";
		//memcpy_s(str, sizeof(str), uName, 16);
		//sprintf_s(str + 13, 3, "%d", index);
		// optimize this later

		shader->setInt("uFocalPoints["+std::to_string(index)+"].forceType",forceType);
		shader->setVec3("uFocalPoints[" + std::to_string(index) + "].position",position);
		shader->setVec3("uFocalPoints[" + std::to_string(index) + "].velocity",velocity);
		shader->setFloat("uFocalPoints["+std::to_string(index)+"].strength", strength);
		shader->setFloat("uFocalPoints["+std::to_string(index)+"].rangeMin",rangeMin);
		shader->setFloat("uFocalPoints["+std::to_string(index)+"].rangeMax",rangeMax);
		shader->setFloat("uFocalPoints["+std::to_string(index)+"].rangeBlend",rangeBlend);
	}
};
struct FocalPlane {
	FocalPlane() : forceType(ForceTypeNone) {}
	// max needs to be positive
	FocalPlane(ForceType type, glm::vec3 pos, glm::vec3 dir,glm::vec3 vel, float strength, float max) :
		forceType(type), position(pos), direction(glm::normalize(dir)),velocity(vel), strength(strength), rangeMax(max) {
		if (rangeMax < 0) {
			rangeMax = -rangeMax;
			engone::log::out << engone::log::RED << "ForcePlane - rangeMax cannot be negative!\n";
		}
	}

	ForceType forceType = ForceTypeNone;
	glm::vec3 position = { 0,0,0 };
	glm::vec3 direction = { 0,0,0 }; // MUST BE NORMALIZED!
	glm::vec3 velocity = { 0,0,0 };
	float strength = 1;
	float rangeMax = 999; // not negative
	float rangeBlend = 0; // is instant effect, 1 is

	float lifeTime=0.f;

	void bind(engone::Shader* shader, int index) {
		//if (index > 9) {
		//	engone::log::out << engone::log::RED << "FocalPoint::bind - index higher than 9 hasn't been implemented\n";
		//}
		shader->bind();
		//char str[30];
		//memset(str, 0, sizeof(str));
		//const char* uName = "uFocalPoints[i].";
		//memcpy_s(str, sizeof(str), uName, 16);
		//sprintf_s(str + 13, 3, "%d", index);
		// optimize this later

		// ForceField types needs to be filtered last since they take in forces
		// This can be done in the shader without affect on performance. Unless you uss 100 focalPoint but
		// i don't think the filter will be the issue at that point.

		// recalculate min,max and position so that max is a positive number, min is 0. and position
		// is right position so keep the same effect.

		shader->setInt("uFocalPlanes[" + std::to_string(index) + "].forceType", forceType);
		shader->setVec3("uFocalPlanes[" + std::to_string(index) + "].position", position);
		shader->setVec3("uFocalPlanes[" + std::to_string(index) + "].direction", direction);
		shader->setVec3("uFocalPlanes[" + std::to_string(index) + "].velocity", velocity);
		shader->setFloat("uFocalPlanes[" + std::to_string(index) + "].strength", strength);
		shader->setFloat("uFocalPlanes[" + std::to_string(index) + "].rangeMax", rangeMax);
		shader->setFloat("uFocalPlanes[" + std::to_string(index) + "].rangeBlend", rangeBlend);
	}
};