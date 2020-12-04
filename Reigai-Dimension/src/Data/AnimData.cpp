#include "AnimData.h"

glm::mat4 Keyframe::getLocalTransform(){
	glm::mat4 m(1);
	m *= glm::translate(position);
	//m *= glm::toMat4(rotation);
	return m;
}
glm::mat4 Keyframe::getLocalTransform(Keyframe* b, float progress) {
	return glm::mat4(1) * glm::translate(position * (1 - progress) + b->position * progress);// *glm::toMat4(glm::mix(rotation, b->rotation, progress));
}
/*
float bezier(float x, float xStart, float xEnd, float y0, float y1) {
	float t = (x - xStart) / (xEnd - xStart);
	float va = (pow(1 - t, 3) + 3 * pow(1 - t, 2) * t) * y0 + (3 * (1 - t) * pow(t, 2) + pow(t, 3)) * y1;
	return va;
}
*/
float bezier(float x, float xStart, float xEnd) {
	float t = (x - xStart) / (xEnd - xStart);
	float va = (pow(1 - t, 3) + 3 * pow(1 - t, 2) * t) + (3 * (1 - t) * pow(t, 2) + pow(t, 3));
	return va;
}
glm::mat4 Transforms::GetValue(int frame) {
	Keyframe* a=nullptr;
	Keyframe* b=nullptr;
	for (Keyframe k : frames) {
		if (k.frame <= frame) {
			a = &k;
		}
		if (k.frame >= frame) {
			b = &k;
			break;
		}
	}
	if (a == b) {
		return a->getLocalTransform();
	}
	if (a != nullptr&&b!=nullptr) {
		if (a->polation == 'C') {
			return a->getLocalTransform();
		} else if (a->polation == 'L') {
			float p = ((float)frame - a->frame) / (b->frame - a->frame);
			return a->getLocalTransform(b, p);
		} else if (a->polation == 'B') {
			float p = bezier((float)frame, a->frame, b->frame);
			return a->getLocalTransform(b, p);
		}
		return a->getLocalTransform();
	}
	return glm::mat4(1);
}