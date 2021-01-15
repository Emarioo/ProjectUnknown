#pragma once

#include "../Data/AnimData.h"
#include <glm/glm.hpp>

namespace engine {

	class AnimationComponent {
	public:
		AnimationComponent() {}

		AnimData* anim = nullptr;
		void SetAnim(AnimData* data);

		float frame = 0;
		float speed = 24; // per second
		bool loop = false;
		bool running = false;
		int GetFrame();
		void Update(float delta);
		/*
		What if name doesn't exist?
		*/
		glm::mat4 GetTransform(int index);

		bool enabled = false;
		bool hasError = false;
	};
}