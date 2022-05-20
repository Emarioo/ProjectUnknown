
#include "Component.h"

namespace engone {

	ComponentMask::ComponentMask(size_t mask) : componentMask(mask) {}
	ComponentMask::ComponentMask(ComponentEnum mask) : componentMask(1 << ((size_t)mask - 1)) {}
	ComponentMask::ComponentMask(const ComponentMask& mask) : componentMask(mask.componentMask) {}
	ComponentMask::ComponentMask(ComponentMask* mask) : componentMask(mask->componentMask) {}
	ComponentMask ComponentMask::operator|(ComponentEnum filter) {
		componentMask = componentMask | (1 << ((size_t)filter - 1));
		return *this;
	}

	ComponentMask operator|(ComponentEnum a, ComponentEnum b) {
		return (1 << ((size_t)a - 1)) | (1 << ((size_t)b - 1));
	}
	AnimationProperty::AnimationProperty(bool loop, float blend, float speed)
		: frame(0), loop(loop), blend(blend), speed(speed) {}
	AnimationProperty::AnimationProperty(float frame, bool loop, float blend, float speed) :
		frame(frame), loop(loop), blend(blend), speed(speed) {

	}
	void Animator::Update(float delta) {
		if (asset != nullptr) {
			for (size_t i = 0; i < maxAnimations; ++i) {
				if (enabledAnimations[i].asset) {
					AnimationProperty& prop = enabledAnimations[i];
					
					for (AnimationAsset* anim : asset->animations) {// This is kind of bad. A lot of animations will cause performance issues
						if (prop.asset == anim && anim) {
							
							prop.frame += anim->defaultSpeed * prop.speed * delta;
							if (anim->frameEnd <= prop.frame) {
								prop.frame = 0;
								if (!prop.loop) {
									enabledAnimations[i].asset = nullptr;
								}
								//bug::out < model->animations[j]->frameEnd < " <= " < p.second < " V"< bug::end;
							} else {
								//bug::out < model->animations[j]->frameEnd < " <= " < p.second < bug::end;
							}
						}
					}
				}
			}
		}
	}
	void Animator::Blend(const std::string& name, float blend) {
		for (size_t i = 0; i < maxAnimations; ++i) {
			if (std::strcmp(enabledAnimations[i].instanceName, name.c_str()) == 0)
				enabledAnimations[i].blend = blend;
		}
	}
	void Animator::Speed(const std::string& name, float speed) {
		for (size_t i = 0; i < maxAnimations; ++i) {
			if (std::strcmp(enabledAnimations[i].instanceName, name.c_str())==0)
				enabledAnimations[i].speed = speed;
		}
	}
	/*
	If animation doesn't exist in the model by default. It will try to find the asset and if it does.
	The animation will be added to the model. This is mainly used for debug purposes.
	*/
	void Animator::Enable(const std::string& instanceName, const std::string& animationName, AnimationProperty prop) {
		if (asset != nullptr) {
			AnimationAsset* anim = nullptr;
			for (AnimationAsset* a : asset->animations) {
				if (a->baseName == animationName) {
					anim = a;
					break;
				}
			}
			if (anim&&animationName.length()<20) {
				for (size_t i = 0; i < maxAnimations; ++i) {
					if (enabledAnimations[i].asset == anim) {
						if (std::strcmp(enabledAnimations[i].instanceName, instanceName.c_str()) == 0) {
							return;
						}
					}
				}
				std::strcpy(prop.instanceName, instanceName.c_str());

				prop.asset = anim;
				for (size_t i = 0; i < maxAnimations;++i) {
					if (!enabledAnimations[i].asset) {
						enabledAnimations[i] = prop;
						break;
					}
				}
			} else {
				log::out << log::RED << "Animator: could not find " << animationName << "\n" << log::SILVER;
			}
		}

	}
	void Animator::Disable(const std::string& name) {
		for (int i = 0; i < maxAnimations; i++) { 
			if (!enabledAnimations[i].asset) {
				if (enabledAnimations[i].instanceName == name)
					enabledAnimations[i].asset = nullptr;
			}
		}
	}
}