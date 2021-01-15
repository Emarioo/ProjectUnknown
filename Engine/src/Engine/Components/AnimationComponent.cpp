#include "AnimationComponent.h"

#include "Utility/Debugger.h"

namespace engine {
	void AnimationComponent::SetAnim(AnimData* data) {
		if (data != nullptr) {
			anim = data;
			speed = data->defaultSpeed;
			enabled = true;
		}
	}
	int AnimationComponent::GetFrame() {
		return frame;
	}
	/*
	Local Matrix of bone
	*/
	glm::mat4 AnimationComponent::GetTransform(int index) {
		if (enabled) {
			if (anim->objects.count(index) > 0)
				return anim->objects[index].GetMatrix(GetFrame());
		}
		return glm::mat4(1);
	}
	void AnimationComponent::Update(float delta) {
		if (enabled) {
			if (running) {
				frame += speed * delta;
				if (anim->frameEnd <= GetFrame()) {
					frame = 0;
					if (!loop)
						running = false;
				}
			}
		}
	}
}