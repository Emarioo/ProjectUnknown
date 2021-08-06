#include "Animator.h"

#include "../DebugTool/DebugHandler.h"

namespace engine {
	
	AnimProp::AnimProp() {

	}
	AnimProp::AnimProp(bool loop, float blend, float speed)
		: frame(0), loop(loop), blend(blend), speed(speed) {
	}
	AnimProp::AnimProp(float frame, bool loop, float blend, float speed) :
		frame(frame), loop(loop), blend(blend), speed(speed) {

	}
	void Animator::Update(float delta) {
		if (model != nullptr) {
			std::vector<std::string> disable;
			for (auto& p : enabledAnimations) {
				for (int j = 0; j < model->animations.size(); j++) {// This is kind of bad. A lot of animations will cause performance issues
					if (p.first == model->animations[j]->name) {
						AnimProp& prop = p.second;
						prop.frame += model->animations[j]->defaultSpeed * prop.speed * delta;
						if (model->animations[j]->frameEnd <= prop.frame) {
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
	void Animator::Enable(const std::string& name, AnimProp prop) {
		enabledAnimations[name] = prop;
	}
	void Animator::Disable(const std::string& name) {
		enabledAnimations.erase(name);
	}
}