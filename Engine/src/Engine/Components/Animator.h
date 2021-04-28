#pragma once

#include "Animation.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <unordered_map>

namespace engine {

	class AnimProp {
	public:
		AnimProp()
		: frame(0),loop(false), blend(1),speed(1) {}
		AnimProp(float frame, bool loop, float blend,float speed) :
			frame(frame), loop(loop), blend(blend),speed(speed) {}
		float frame;
		float speed;// multiplier to the default speed
		bool loop;
		float blend;
	};

	class Animator {
	public:
		Animator() {}

		Model* model=nullptr;

		std::unordered_map<std::string, AnimProp> enabledAnimations;
		
		void Update(float delta);
		/*
		What if name doesn't exist?
		*/

		void Enable(const std::string& name,AnimProp prop);
		void Blend(const std::string& name,float blend);
		void Disable(const std::string& name);
	};
}