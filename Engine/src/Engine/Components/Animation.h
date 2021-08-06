#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <unordered_map>

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
		/*
		Add values to the referances given in the argument
		*/
		void GetValues(int frame, float blend, glm::vec3& pos, glm::vec3& euler, glm::vec3& scale, glm::mat4& quater);
		FCurve* Get(char channel) {
			return &fcurves[channel];
		}
		void Add(char channel, FCurve fcurve) {
			fcurves[channel] = fcurve;
		}
		std::unordered_map<char, FCurve> fcurves;
	};
	class Animation {
	public:
		Animation() {}

		std::string name;

		FCurves* Get(int i) {
			return &objects[i];
		}
		void Add(int index, FCurves fcurves) {
			objects[index] = fcurves;
		}
		std::unordered_map<int, FCurves> objects;

		unsigned short frameStart = 0;
		unsigned short frameEnd = 0;
		float defaultSpeed = 24;// frames per second. 24 is default from blender.

		bool hasError = false;

	};
}