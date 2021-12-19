#pragma once

//#include "Animation.h"
//#include "Model.h"

#include "../Handlers/AssetHandler.h"

namespace engone {

	class AnimationProperty {
	public:
		AnimationProperty() = default;
		AnimationProperty(bool loop, float blend, float speed);
		AnimationProperty(float frame, bool loop, float blend, float speed);

		float frame;
		float speed;// multiplier to the default speed
		bool loop;
		float blend;
		std::string instanceName;
		std::string animationName;
	};

	class Animator {
	public:
		Animator() = default;

		ModelAsset* model;
		/*
		Temporary active animations
		*/
		std::vector<AnimationProperty> enabledAnimations;
		
		void Update(float delta);

		void Blend(const std::string& name, float blend);
		void Speed(const std::string& name, float speed);
		/*
		Second argument can be done like this: {frame, loop, blend, speed}
		*/
		void Enable(const std::string& instanceName,const std::string& animationName, AnimationProperty prop);
		void Disable(const std::string& name);
	};
}