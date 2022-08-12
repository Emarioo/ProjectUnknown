#pragma once

#include "Engone/AssetModule.h"

enum ForceType : int {
	ForceTypeNone = 0,
	ForceTypeAttractive=1,
	ForceTypeRepllent=2,
};
struct FocalPoint {
	FocalPoint(ForceType type, glm::vec3 pos, float strength, float min, float max) : 
		forceType(), position(pos),strength(strength),rangeMin(min),rangeMax(max) {}

	ForceType forceType = ForceTypeNone;
	glm::vec3 position = {0,0,0};
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
		shader->setFloat("uFocalPoints["+std::to_string(index)+"].strength", strength);
		shader->setFloat("uFocalPoints["+std::to_string(index)+"].rangeMin",rangeMin);
		shader->setFloat("uFocalPoints["+std::to_string(index)+"].rangeMax",rangeMax);
		shader->setFloat("uFocalPoints["+std::to_string(index)+"].rangeBlend",rangeBlend);
	}
};