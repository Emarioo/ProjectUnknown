#include "AnimationComponent.h"

void AnimationComponent::SetData(AnimData* data) {
	this->data = data;
	
	if (data != nullptr)
		speed = data->defaultSpeed;
}
int AnimationComponent::GetFrame() {
	return frame;
}
glm::mat4 AnimationComponent::GetTransform(int index) {
	if (data != nullptr) {
		return data->bones[index].GetValue(GetFrame());
	}
	return glm::mat4(1);
}
void AnimationComponent::Update(float delta) {
	if (data != nullptr) {
		if (running) {
			frame+=speed*delta;
			if (data->frameEnd < GetFrame()) {
				frame = 0;
				if (!data->loop)
					running = false;
			}
		}
	}
}
