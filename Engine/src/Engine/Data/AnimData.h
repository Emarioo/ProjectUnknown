#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
//#include <glm/gtx/quaternion.hpp>
//#include <glm/common.hpp>
#include <unordered_map>

/*
class Keyframe {
public:
	unsigned short frame;
	char polation;
	//std::vector<DataPath>
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	Keyframe(unsigned short frame, char polation, glm::vec3 pos, glm::quat rot)
	: frame(frame),polation(polation),position(pos),rotation(rot) {}
	glm::mat4 getLocalTransform();
	glm::mat4 getLocalTransform(Keyframe* b, float progress);
};
*/
namespace engine {
	class Keyframe {
	public:
		char polation;
		unsigned short frame;
		float value;
		Keyframe(char polation, unsigned short frame, float value)
			: polation(polation), frame(frame), value(value) {}
	};
	class FCurve {
	public:
		FCurve() {}
		Keyframe* Get(int index) {
			return &frames.at(index);
		}
		void Add(Keyframe keyframe) {
			frames.push_back(keyframe);
		}
		void Add(char polation, unsigned short frame, float value) {
			frames.push_back(Keyframe(polation, frame, value));
		}

		std::vector<Keyframe> frames;
	};
	class FCurves {
	public:
		FCurves() {}
		glm::mat4 GetMatrix(int frame);
		FCurve* Get(char channel) {
			return &fcurves[channel];
		}
		void Add(char channel, FCurve fcurve) {
			fcurves[channel] = fcurve;
		}
		std::unordered_map<char, FCurve> fcurves;
	};
	class AnimData {
	public:
		AnimData() {}

		FCurves* Get(int i) {
			return &objects[i];
		}
		void Add(int index, FCurves fcurves) {
			objects[index] = fcurves;
		}
		std::unordered_map<int, FCurves> objects;

		unsigned short frameStart = 0;
		unsigned short frameEnd = 0;
		float defaultSpeed = 24;// frame per second

		bool hasError = false;

	};
}