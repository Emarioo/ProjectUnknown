#include "AnimationComponent.h"

AnimationComponent::AnimationComponent(AnimData* data) {
	this->data=data;
	
	if(data!=nullptr)
		fps = data->defaultSpeed;
}
void AnimationComponent::SetAnim(AnimData* data) {
	this->data = data;
	
	if (data != nullptr)
		fps = data->defaultSpeed; // <- in magicstaff.cpp
}
int AnimationComponent::GetFrame() {
	//return frame*(data->frameEnd-data->frameStart)+data->frameStart;
	return frame;
}
glm::vec3 AnimationComponent::GetPos(std::string o) {
	if (data != nullptr) {
		float v[]{
			0,0,0
		};
		FCurves* curv = &data->curves[o];
		for (int i = 1; i < 2;i++) {
			if (curv->CurveExist(i)) {
				v[i] = curv->keyframes[i].GetValue(GetFrame());
			}
		}
		return glm::vec3(v[0], v[1], v[2]);
	}
	return glm::vec3(0, 0, 0);
}
glm::vec3 AnimationComponent::GetRot(std::string o){
	if (data != nullptr) {
		float v[]{
			0,0,0
		};
		FCurves* curv = &data->curves[o];
		for (int i = 3; i < 6; i++) {
			if (curv->CurveExist(i)) {
				v[i-3] = curv->keyframes[i].GetValue(GetFrame());
			}
		}
		return glm::vec3(v[0], v[1], v[2]);
	}
	return glm::vec3(0, 0, 0);
}
glm::vec3 AnimationComponent::GetScale(std::string o) {
	if (data != nullptr) {
		float v[]{
			0,0,0
		};
		FCurves* curv = &data->curves[o];
		for (int i = 6; i < 9; i++) {
			if (curv->CurveExist(i)) {
				v[i-6] = curv->keyframes[i].GetValue(GetFrame());
			}
		}
		return glm::vec3(v[0], v[1], v[2]);
	}
	return glm::vec3(0, 0, 0);
}
void AnimationComponent::Update(float delta) {
	if (data != nullptr) {
		if (running) {
			frame+=24*delta;
			if (data->frameEnd < GetFrame()) {
				frame = 0;
				if (!data->loop)
					running = false;
			}
		}
	}
}
