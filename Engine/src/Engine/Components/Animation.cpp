#include "Animation.h"

#include "../DebugTool/DebugHandler.h"
#include "../Utility/Utilities.h"

namespace engine {
	void FCurves::GetValues(int frame,float blend,glm::vec3& pos, glm::vec3& euler, glm::vec3& scale, glm::mat4& quater) {
		float build[9]{ 0,0,0,0,0,0,1,1,1 };
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
				}
				if (k.frame >= frame) {
					if (a == nullptr)
						a = &k;
					b = &k;
					break;
				}
			}
			if (a == nullptr && b == nullptr) {
				continue;
			}
			if (b == nullptr) {
				b = a;
			}
			if (a->frame == b->frame) {
				if (type > 8) {
					slerpT = 0;
					if (type == 9) {
						q0.w = a->value;
						q1.w = b->value;
					} else if (type == 10) {
						q0.x = a->value;
						q1.x = b->value;
					} else if (type == 11) {
						q0.y = a->value;
						q1.y = b->value;
					} else if (type == 12) {
						q0.z = a->value;
						q1.z = b->value;
					}
					continue;
				} else {
					build[curve.first] = a->value;
					continue;
				}
			}
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
			if (type > 8) {
				slerpT = lerp;
				if (type == 9) {
					q0.w = a->value;
					q1.w = b->value;
				} else if (type == 10) {
					q0.x = a->value;
					q1.x = b->value;
				} else if (type == 11) {
					q0.y = a->value;
					q1.y = b->value;
				} else if (type == 12) {
					q0.z = a->value;
					q1.z = b->value;
				}
			} else {
				build[curve.first] = a->value * (1 - lerp) + b->value * lerp;
			}

		}

		//glm::mat4 quatRot = glm::mat4_cast(glm::slerp(glm::slerp(q0, q1, slerpT), glm::quat(),blend));
		pos.x += build[0] * blend;
		pos.y += build[1] * blend;
		pos.z += build[2] * blend;

		euler.x += build[3] * blend;
		euler.y += build[4] * blend;
		euler.z += build[5] * blend;

		scale.x *= build[6] * blend;
		scale.y *= build[7] * blend;
		scale.z *= build[8] * blend;
		quater *= glm::mat4_cast(glm::slerp(glm::slerp(q0, q1, slerpT), glm::quat(), blend));
		//quater *= glm::slerp(q0, q1, slerpT);
		/*return
			glm::translate(glm::vec3(build[0], build[1], build[2]))
			* quatRot
			* glm::rotate(build[3], glm::vec3(1, 0, 0))
			* glm::rotate(build[4], glm::vec3(0, 1, 0))
			* glm::rotate(build[5], glm::vec3(0, 0, 1))
			* glm::scale(glm::vec3(build[6], build[7], build[8]))
			;
		*/
	}
}