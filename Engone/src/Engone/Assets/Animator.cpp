#include "Engone/Assets/AnimationAsset.h"
#include "Engone/Assets/ModelAsset.h"
#include "Engone/Assets/Animator.h"
#include "Engone/Logger.h"

namespace engone {

	float AnimatorProperty::getRemainingSeconds() const {
		if (!asset)
			DebugBreak();
		float out = asset->frameEnd - frame;
		out *= speed / asset->defaultSpeed;
		return out;
	}
	void Animator::cleanup() {
		asset = nullptr;
		for (uint32_t i = 0; i < maxAnimations; ++i) {
			enabledAnimations[i].asset = nullptr;
		}
	}
	void Animator::update(float delta) {
		if (asset != nullptr) {
			for (uint32_t i = 0; i < maxAnimations; ++i) {
				if (enabledAnimations[i].asset) {
					AnimatorProperty& prop = enabledAnimations[i];

					for (AnimationAsset* anim : asset->animations) {// This is kind of bad. A lot of animations will cause performance issues
						if (prop.asset == anim && anim) {

							prop.frame += anim->defaultSpeed * prop.speed * delta;
							if (anim->frameEnd <= prop.frame) {
								prop.frame = 0;
								if (!prop.loop) {
									enabledAnimations[i].asset = nullptr;

								}
								//bug::out < model->animations[j]->frameEnd < " <= " < p.second < " V"< bug::end;
							}
							else {
								//bug::out < model->animations[j]->frameEnd < " <= " < p.second < bug::end;
							}
						}
					}
				}
			}

		}
	}
	//void Animator::setBlend(const std::string& name, float blend) {
	//	for (uint32_t i = 0; i < maxAnimations; ++i) {
	//		if (enabledAnimations[i].instanceName== name)
	//			enabledAnimations[i].blend = blend;
	//	}
	//}
	//void Animator::setSpeed(const std::string& name, float speed) {
	//	for (uint32_t i = 0; i < maxAnimations; ++i) {
	//		if (enabledAnimations[i].instanceName== name)
	//			enabledAnimations[i].speed = speed;
	//	}
	//}
	/*
	If animation doesn't exist in the model by default. It will try to find the asset and if it does.
	The animation will be added to the model. This is mainly used for debug purposes.
	*/
	void Animator::enable(const std::string& instanceName, const std::string& animationName, AnimatorProperty prop) {
		if (asset != nullptr) {
			AnimationAsset* anim = nullptr;
			for (AnimationAsset* a : asset->animations) {
				// ISSUE: FIX HERE
				if (a->getName() == animationName) {
					anim = a;
					break;
				}
			}
			if (anim) {
				for (uint32_t i = 0; i < maxAnimations; ++i) {
					if (enabledAnimations[i].asset == anim) {
						if (enabledAnimations[i].instanceName == instanceName) {
							// return if animation already is enabled
							return;
						}
					}
				}
				prop.instanceName = instanceName;

				prop.asset = anim;
				for (uint32_t i = 0; i < maxAnimations; ++i) {
					if (!enabledAnimations[i].asset) {
						enabledAnimations[i] = prop;
						break;
					}
				}
			}
			else {
				log::out << log::RED << "Animator: could not find " << animationName << "\n" << log::SILVER;
			}
		}

	}
	void Animator::disable(const std::string& instName) {
		for (int i = 0; i < maxAnimations; i++) {
			if (enabledAnimations[i].asset) {
				if (enabledAnimations[i].instanceName == instName)
					enabledAnimations[i].asset = nullptr;
			}
		}
	}
	AnimatorProperty* Animator::getProp(const std::string& instName) {
		for (uint32_t i = 0; i < maxAnimations; ++i) {
			if (enabledAnimations[i].instanceName == instName)
				return &enabledAnimations[i];
		}
		return nullptr;
	}
	bool Animator::isEnabled(const std::string& instName) const {
		for (uint32_t i = 0; i < maxAnimations; ++i) {
			if (enabledAnimations[i].asset) {
				if (enabledAnimations[i].instanceName == instName)
					return true;
			}
		}
		return false;
	}
}