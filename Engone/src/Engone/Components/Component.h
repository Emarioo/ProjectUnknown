#pragma once

#include "../Logger.h"
#include "../Handlers/AssetHandler.h"

namespace engone {

	enum class ComponentEnum : int {
		None = 0,
		Transform,
		Physics,
		Scriptable,
		Model,
		Animator,
	};
	class  ComponentMask {
	protected:
	public:
		int componentMask=0;
		ComponentMask() = default;
		ComponentMask(int mask);
		ComponentMask(ComponentEnum mask);
		ComponentMask(const ComponentMask& mask);
		ComponentMask(ComponentMask* mask);

		ComponentMask operator|(ComponentEnum filter);

		bool same(ComponentMask& filter) {
			return componentMask == filter.componentMask;
		}
		bool same(ComponentMask* filter) {
			return componentMask == filter->componentMask;
		}

		// 01011
		// xor
		// 00110

		// 01101
		
		// 01101
		// or
		// 11001
		// 
		// 11101
		// not
		// 00010
		bool has(ComponentMask& filter) {
			int a = componentMask ^ filter.componentMask;
			a = a | (~filter.componentMask);
			a = ~a;
			return a==filter.componentMask;
		}
		bool has(ComponentMask* filter) {
			//log::out << componentMask << " " << filter->componentMask;
			int a = componentMask ^ filter->componentMask;
			int b = a | (~filter->componentMask);
			//log::out << a << " > " << b;
			int c = ~b;
			return c == filter->componentMask;
		}
		bool has(ComponentEnum filter) {
			//log::out << componentMask << " " << ((int)filter)<<" "<< (componentMask & (1 << ((int)filter - 1)))<< " \n";
			return componentMask & (1<<((int)filter-1));
		}
	};
	ComponentMask operator|(ComponentEnum a, ComponentEnum b);

	class Component {
	public:
		static const ComponentEnum MASK = ComponentEnum::None;
		template<class T>
		T* cast() {
			return reinterpret_cast<T>(this);
		}
	};

	class Transform : public Component {
	public:
		static const ComponentEnum MASK = ComponentEnum::Transform;
		Transform() = default;

		glm::vec3 position = { 0,0,0 };
		glm::quat rotation = { 1,0,0,0 };
		glm::vec3 scale = { 1,1,1 };
	};
	class Physics : public Component {
	public:
		static const ComponentEnum MASK = ComponentEnum::Physics;
		Physics() = default;

		glm::vec3 lastPosition = {0,0,0};
		glm::vec3 velocity = { 0,0,0 };
		float gravity = -9.81;
		bool renderCollision = true;
		bool movable = false;
	};
	class Entity;
	class Scriptable : public Component {
	public:
		static const ComponentEnum MASK = ComponentEnum::Scriptable;
		Scriptable() = default;

		Entity* entity=nullptr;
	};
	class Model : public Component {
	public:
		static const ComponentEnum MASK = ComponentEnum::Model;
		Model() = default;

		ModelAsset* modelAsset = nullptr;
	};
	class AnimationProperty {
	public:
		AnimationProperty() = default;
		AnimationProperty(bool loop, float blend, float speed);
		AnimationProperty(float frame, bool loop, float blend, float speed);

		float frame;
		float speed;// multiplier to the default speed
		bool loop;
		float blend;
		std::string instanceName;
		std::string animationName;
	};
	class Animator : public Component {
	public:
		static const ComponentEnum MASK = ComponentEnum::Animator;
		Animator() = default;

		/*
		Temporary active animations
		*/
		std::vector<AnimationProperty> enabledAnimations;

		void Update(float delta, ModelAsset* model);

		void Blend(const std::string& name, float blend);
		void Speed(const std::string& name, float speed);
		/*
		Second argument can be done like this: {frame, loop, blend, speed}
		*/
		void Enable(ModelAsset* model, const std::string& instanceName, const std::string& animationName, AnimationProperty prop);
		void Disable(const std::string& name);
	};
}