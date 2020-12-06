#pragma once

#include <vector>
#include <string>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
//#include "glm/gtx/component_wise.hpp"
//#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/quaternion.hpp"
#include <unordered_map>

class Keyframe {
public:
	unsigned short frame;
	char polation;
	//std::vector<DataPath>
	glm::vec3 position;
	glm::quat rotation;

	Keyframe(unsigned short frame, char polation, glm::vec3 pos, glm::quat rot)
	: frame(frame),polation(polation),position(pos),rotation(rot) {}
	glm::mat4 getLocalTransform();
	glm::mat4 getLocalTransform(Keyframe* b, float progress);
};
class Transforms {
public:
	Transforms(){}
	glm::mat4 GetValue(int frame);
	std::vector<Keyframe> frames;
};
class AnimData {
public:
	AnimData(){}
	// unordered_map?
	std::vector<Transforms> bones;

	unsigned short frameStart = 0;
	unsigned short frameEnd = 0;
	float defaultSpeed=24;// frame per second
	bool loop=0;

	bool hasError = false;
};
