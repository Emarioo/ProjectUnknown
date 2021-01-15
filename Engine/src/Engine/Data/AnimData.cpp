#include "AnimData.h"

#include "Utility/Debugger.h"
#include "Utility/Utilities.h"
/*
glm::mat4 Keyframe::getLocalTransform(){
	glm::mat4 m(1);
	m *= glm::translate(position);
	//m *= glm::toMat4(rotation);
	return m;
}
glm::mat4 Keyframe::getLocalTransform(Keyframe* b, float progress) {
	glm::vec3 newPosition = position * (1 - progress) + b->position * progress;
	glm::mat4 newRotation = glm::mat4_cast(glm::mix(rotation, b->rotation, progress));
	return glm::mat4(1) * glm::translate(newPosition) * newRotation;
}
*/
/*
float bezier(float x, float xStart, float xEnd, float y0, float y1) {
	float t = (x - xStart) / (xEnd - xStart);
	float va = (pow(1 - t, 3) + 3 * pow(1 - t, 2) * t) * y0 + (3 * (1 - t) * pow(t, 2) + pow(t, 3)) * y1;
	return va;
}
*/
namespace engine {
	glm::mat4 FCurves::GetMatrix(int frame) {
		float build[6]{ 0,0,0,0,0,0 };
		glm::quat q0 = glm::quat(0, 0, 0, 0);
		glm::quat q1 = glm::quat(0, 0, 0, 0);
		float slerpT = 0;
		bool noRotation = false;
		for (std::pair<char, FCurve> curve : fcurves) {
			char type = curve.first;
			Keyframe* a = nullptr;
			Keyframe* b = nullptr;
			for (Keyframe& k : curve.second.frames) {
				if (k.frame <= frame) {
					a = &k;
				} else if (k.frame >= frame) {
					if (a == nullptr)
						a = &k;
					b = &k;
					break;
				}
			}
			if (a == nullptr && b == nullptr)
				continue;
			if (b == nullptr)
				b = a;

			if (a->frame == b->frame) {
				build[curve.first] = a->value;
			} else {
				float lerp = 0;
				if (a != nullptr && b != nullptr) {
					if (a->polation == 'C') {
						lerp = 0;
					} else if (a->polation == 'L') {
						lerp = ((float)frame - a->frame) / (b->frame - a->frame);
					} else if (a->polation == 'B') {
						lerp = bezier((float)frame, a->frame, b->frame);
					}
				}
				if (type > 5) {
					slerpT = lerp;
					if (type == 6) {
						q0.w = a->value;
						q1.w = b->value;
					} else if (type == 7) {
						q0.x = a->value;
						q1.x = b->value;
					} else if (type == 8) {
						q0.y = a->value;
						q1.y = b->value;
					} else if (type == 9) {
						q0.z = a->value;
						q1.z = b->value;
					}
				} else
					build[curve.first] = a->value * (1 - lerp) + b->value * lerp;
			}
		}
		//glm::mat4 rotation=glm::mat4(1);

		//if(!noRotation)
		//	rotation *= glm::mat4_cast(glm::slerp(q0,q1,slerpT));
		//bug::outs < w < x < y < z < bug::end;
		//build[0] = build[3];
		//bug::outs < build[0] < build[1] < build[2] <build[3] < build[4] < build[5] < bug::end;
		return
			glm::translate(glm::vec3(build[0], build[1], build[2]))
			* glm::rotate(build[3], glm::vec3(1, 0, 0))
			* glm::rotate(build[4], glm::vec3(0, 1, 0))
			* glm::rotate(build[5], glm::vec3(0, 0, 1))
			//* glm::scale(glm::vec3(build[6], build[7], build[8]))
			;
	}
}