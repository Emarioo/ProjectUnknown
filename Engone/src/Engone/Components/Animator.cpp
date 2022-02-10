#include "gonpch.h"

#include "Animator.h"


#include "../DebugTool/DebugHandler.h"

namespace engone {
	
	AnimationProperty::AnimationProperty(bool loop, float blend, float speed)
		: frame(0), loop(loop), blend(blend), speed(speed) {
	}
	AnimationProperty::AnimationProperty(float frame, bool loop, float blend, float speed) :
		frame(frame), loop(loop), blend(blend), speed(speed) {

	}
	void Animator::Update(float delta, ModelAsset* model) {

		if (model != nullptr) {
			std::vector<std::string> disable;
			for (int i = 0; i < enabledAnimations.size();i++){
				AnimationProperty& prop = enabledAnimations[i];
				//std::cout << " enab" << std::endl;
				for (AnimationAsset* anim : model->animations) {// This is kind of bad. A lot of animations will cause performance issues
					if (prop.animationName == anim->baseName) {
						//std::cout << "prop" << std::endl;
						prop.frame += anim->defaultSpeed * prop.speed * delta;
						if (anim->frameEnd <= prop.frame) {
							prop.frame = 0;
							if (!prop.loop) {
								disable.push_back(prop.animationName);
								// Disable prop
							}
							//bug::out < model->animations[j]->frameEnd < " <= " < p.second < " V"< bug::end;
						}
						else {
							//bug::out < model->animations[j]->frameEnd < " <= " < p.second < bug::end;
						}
					}
				}
			}
			for (std::string& str : disable) {
				for (int i = 0; i < enabledAnimations.size(); i++) {
					if(enabledAnimations[i].animationName==str)
						enabledAnimations.erase(enabledAnimations.begin()+i);
				}
			}
			/*
			if (animation!=nullptr) {
				if (running) {
					frame += speed * delta;
					if (animation->frameEnd <= GetFrame()) {
						frame = 0;
						if (!loop)
							running = false;
					}
				}
			}*/
		}
	}
	void Animator::Blend(const std::string& name, float blend) {
		for (int i = 0; i < enabledAnimations.size(); i++) {
			if (enabledAnimations[i].animationName == name)
				enabledAnimations[i].blend = blend;
		}
	}
	void Animator::Speed(const std::string& name, float speed) {
		for (int i = 0; i < enabledAnimations.size(); i++) {
			if (enabledAnimations[i].animationName == name)
				enabledAnimations[i].speed = speed;
		}
	}
	/*
	If animation doesn't exist in the model by default. It will try to find the asset and if it does.
	The animation will be added to the model. This is mainly used for debug purposes.
	*/
	void Animator::Enable(ModelAsset* model, const std::string& instanceName, const std::string& animationName, AnimationProperty prop) {
		if (model != nullptr) {
			bool found = false;
			for (AnimationAsset* a : model->animations) {
				if (a->baseName == animationName) {
					found = true;
					break;
				}
			}
			if (found) {
				prop.instanceName = instanceName;
				prop.animationName = animationName;
				enabledAnimations.push_back(prop);
			}
			else {
				log::out <<log::RED<< "Animator: could not find " << animationName << "\n"<<log::SILVER;
			}
		}

	}
	void Animator::Disable(const std::string& name) {
		for (int i = 0; i < enabledAnimations.size();i++) {
			if(enabledAnimations[i].animationName==name)
				enabledAnimations.erase(enabledAnimations.begin() + i);
		}
	}
}