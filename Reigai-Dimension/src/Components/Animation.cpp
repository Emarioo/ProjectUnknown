#include "Animation.h"

Animation::Animation(std::string name) {
	data=GetAnim(name);
	if(data!=nullptr)
		fps = data->defaultSpeed;
}
int Animation::GetFrame() {
	return (int)frame;
}
glm::vec3 Animation::GetPos(std::string o) {
	if (data != nullptr) {
		float v[]{
			0,0,0
		};
		FCurves* curv = &data->curves[o];
		for (int i = 0; i < 3;i++) {
			if (curv->CurveExist(i)) {
				v[i] = curv->keyframes[i].GetValue(frame);
			}
		}
		return glm::vec3(v[0], v[1], v[2]);
	}
	return glm::vec3(0, 0, 0);
}
glm::vec3 Animation::GetRot(std::string o){
	if (data != nullptr) {
		float v[]{
			0,0,0
		};
		FCurves* curv = &data->curves[o];
		for (int i = 3; i < 6; i++) {
			if (curv->CurveExist(i)) {
				v[i-3] = curv->keyframes[i].GetValue(frame);
			}
		}
		return glm::vec3(v[0], v[1], v[2]);
	}
	return glm::vec3(0, 0, 0);
}
glm::vec3 Animation::GetScale(std::string o) {
	if (data != nullptr) {
		float v[]{
			0,0,0
		};
		FCurves* curv = &data->curves[o];
		for (int i = 6; i < 9; i++) {
			if (curv->CurveExist(i)) {
				v[i-6] = curv->keyframes[i].GetValue(frame);
			}
		}
		return glm::vec3(v[0], v[1], v[2]);
	}
	return glm::vec3(0, 0, 0);
}
void Animation::Update(float delta) {
	if (data != nullptr) {
		if (running) {
			frame+=fps*delta;
			if (data->frameEnd < GetFrame()) {
				frame = 0;
				if (!data->loop)
					running = false;
			}
		}
	}
}
