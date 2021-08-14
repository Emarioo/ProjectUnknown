#include "gonpch.h"

#include "Animation.h"

#include "../DebugTool/DebugHandler.h"
#include "../Utility/Utilities.h"


namespace engine {
	Keyframe::Keyframe(PolationType polation, unsigned short frame, float value)
		: polation(polation), frame(frame), value(value) {}
	FCurve::FCurve() {}
	Keyframe& FCurve::Get(int index) {
		return frames.at(index);
	}
	void FCurve::Add(Keyframe keyframe) {
		frames.push_back(keyframe);
	}
	FCurve& Channels::Get(ChannelType channel) {
		return fcurves[channel];
	}
	/*
	"channel" is what type of value the keyframes will be changing. Example PosX.
	Create a "FCurve" object and see the "Add" function for more details.
	*/
	void Channels::Add(ChannelType channel, FCurve fcurve) {
		fcurves[channel] = fcurve;
	}
	void Channels::GetValues(int frame,float blend,glm::vec3& pos, glm::vec3& euler, glm::vec3& scale, glm::mat4& quater) {
		float build[9]{ 0,0,0,0,0,0,1,1,1 };
		glm::quat q0 = glm::quat(1, 0, 0, 0);
		glm::quat q1 = glm::quat(1, 0, 0, 0);
		//bug::out < "curve "< fcurves.size() < bug::end;
		float slerpT = 0;
		for (std::pair<ChannelType, FCurve> curve : fcurves) {
			ChannelType type = curve.first;
			Keyframe* a = nullptr;
			Keyframe* b = nullptr;
			for (Keyframe& k : curve.second.frames) {
				if (k.frame == frame) { // the keyframe is at the current time frame.
					a = &k;
					break;
				}
				if (k.frame<frame) { // set a to the keyframe below the time frame at this moment in time.
					a = &k;
					continue;
				}
				if (k.frame>frame) { // set b to the keyframe directly after the time frame.
					if (a == nullptr)
						a = &k;
					else
						b = &k;
					break;
				}
			}
			if (a == nullptr && b == nullptr) { // no keyframes
				//std::cout << "no frames "<< type <<" " << curve.second.frames.size() << std::endl;
				continue;
			}
			if (b == nullptr) { // one keyframe ( "a" )
				//std::cout << "one frame "<<a->frame << std::endl;
				if (type > ScaZ) {
					slerpT = 0;
					q0[curve.first-QuaX] = a->value;
					continue;
				}
				else {
					build[curve.first] = a->value;
					continue;
				}
			}
			//std::cout << a->frame << " " << b->frame << std::endl;
			// lerping between to keyframes
			float lerp = 0;
			if (a->polation == Constant) {
				lerp = 0;
			} else if (a->polation == Linear) {
				lerp = ((float)frame - a->frame) / (b->frame - a->frame);
			} else if (a->polation == Bezier) {
				lerp = bezier((float)frame, a->frame, b->frame);
			}

			if (type > ScaZ) {
				slerpT = lerp;
				q0[curve.first-QuaX] = a->value;
				q1[curve.first-QuaX] = b->value;

			} else {
				build[curve.first] = a->value * (1 - lerp) + b->value * lerp;
			}
		}

		pos.x += build[0] * blend;
		pos.y += build[1] * blend;
		pos.z += build[2] * blend;

		euler.x += build[3] * blend;
		euler.y += build[4] * blend;
		euler.z += build[5] * blend;

		scale.x *= build[6] * blend;
		scale.y *= build[7] * blend;
		scale.z *= build[8] * blend;
		
		//glm::mat4 matis = glm::mat4_cast(glm::slerp(glm::quat(0, 0, 0, 1),glm::slerp(q0, q1, slerpT), 1.f));
		//glm::mat4 matis = glm::mat4_cast(glm::slerp(q0, q1, slerpT));
		//bug::out < matis<"\n";
		//quater *= glm::slerp(q0, q1, slerpT);
		
		
		//std::cout << slerpT << std::endl;

		//bug::outs < "b " <build[1] < "\n";

		if (q0 == q1||slerpT==0) {
			quater *= glm::mat4_cast(glm::slerp(glm::quat(1, 0, 0, 0), q0, blend));
		} else if (slerpT==1) {
			quater *= glm::mat4_cast(glm::slerp(glm::quat(1, 0, 0, 0), q1, blend));
		} else // Expensive. Is it possible to optimize?
			quater *= glm::mat4_cast(glm::slerp(glm::quat(1, 0, 0, 0), glm::slerp(q0, q1, slerpT), blend));
	}
	Channels& Animation::Get(unsigned short i) {
		return objects[i];
	}
	/*
	objectIndex is the index of the bone. Also known as the vertex group in blender.
	Create a "Channels" object and see the "Add" function for more details.
	*/
	void Animation::AddObjectChannels(int objectIndex, Channels channels) {
		objects[objectIndex] = channels;
	}
	/*
	Use this to create an animation by code.
	See the "AddObjectChannels" function for more details.
	*/
	void Animation::Modify(unsigned short startFrame, unsigned short endFrame) {
		hasError = false;
		frameStart = startFrame;
		frameEnd = endFrame;
	}
	/*
	Use this to create an animation by code.
	"speed" is 24 by default. Speed can also be changed in the animator.
	See the "AddObjectChannels" function for more details.
	*/
	void Animation::Modify(unsigned short startFrame, unsigned short endFrame, float speed) {
		hasError = false;
		frameStart = startFrame;
		frameEnd = endFrame;
		defaultSpeed = speed;
	}
}