#pragma once

#include "Animation.h"
#include "Model.h"

namespace engine {

	class AnimProp {
	public:
		AnimProp() = default;
		AnimProp(bool loop, float blend, float speed);
		AnimProp(float frame, bool loop, float blend, float speed);
		float frame;
		float speed;// multiplier to the default speed
		bool loop;
		float blend;
	};

	class Animator {
	public:
		Animator() {}

		Model* model=nullptr;

		/*
		Temporary active animations
		*/
		std::unordered_map<std::string, AnimProp> enabledAnimations;
		
		void Update(float delta);

		void Blend(const std::string& name, float blend);
		void Speed(const std::string& name, float speed);
		/*
		Second argument can be done like this: {frame, loop, blend, speed}
		*/
		void Enable(const std::string& name,AnimProp prop);
		void Disable(const std::string& name);
	};
}