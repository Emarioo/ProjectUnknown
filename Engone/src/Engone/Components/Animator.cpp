#include "gonpch.h"

#include "Animator.h"


#include "../DebugTool/DebugHandler.h"

namespace engone {
	
	AnimProp::AnimProp(bool loop, float blend, float speed)
		: frame(0), loop(loop), blend(blend), speed(speed) {
	}
	AnimProp::AnimProp(float frame, bool loop, float blend, float speed) :
		frame(frame), loop(loop), blend(blend), speed(speed) {

	}
	void Animator::Update(float delta) {
#if gone
		if (model != nullptr) {
			std::vector<std::string> disable;
			for (auto& p : enabledAnimations) {
				//std::cout << " enab" << std::endl;
				for (Animation* anim : model->animations) {// This is kind of bad. A lot of animations will cause performance issues
					if (p.first == anim->name) {
						AnimProp& prop = p.second;
						//std::cout << "prop" << std::endl;
						prop.frame += anim->defaultSpeed * prop.speed * delta;
						if (anim->frameEnd <= prop.frame) {
							prop.frame = 0;
							if (!prop.loop) {
								disable.push_back(p.first);
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
				enabledAnimations.erase(str);
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
#endif
	}
	void Animator::Blend(const std::string& name, float blend) {
		if (enabledAnimations.count(name) > 0) {
			enabledAnimations[name].blend = blend;
		}
	}
	void Animator::Speed(const std::string& name, float speed) {
		if (enabledAnimations.count(name) > 0) {
			enabledAnimations[name].speed = speed;
		}
	}
	/*
	If animation doesn't exist in the model by default. It will try to find the asset and if it does.
	The animation will be added to the model. This is mainly used for debug purposes.
	*/
	void Animator::Enable(const std::string& name, AnimProp prop) {
#if gone
		if (model != nullptr) {
			bool found = false;
			for (auto a : model->animations) {
				if (a->name == name) {
					found = true;
					break;
				}
			}
			if (found) {
				enabledAnimations[name] = prop;
			}
			else {
				if(model->AddAnimation(name))
					enabledAnimations[name] = prop;
			}
		}
#endif
	}
	void Animator::Disable(const std::string& name) {
		enabledAnimations.erase(name);
	}
}