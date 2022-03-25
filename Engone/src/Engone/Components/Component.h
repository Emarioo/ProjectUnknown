#pragma once

#include "../Logger.h"
#include "../Handlers/AssetHandler.h"

namespace engone {

	enum class ComponentEnum : int {
		None = 0,
		Transform,
		Physics,
		ModelRenderer,
		Animator,
		MeshRenderer,
		Collision,
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
		static const ComponentEnum ID = ComponentEnum::None;
		template<class T>
		T* cast() {
			return reinterpret_cast<T>(this);
		}
	};

	class Transform : public Component {
	public:
		static const ComponentEnum ID = ComponentEnum::Transform;
		Transform() = default;

		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
	};
	class Physics : public Component {
	public:
		static const ComponentEnum ID = ComponentEnum::Physics;
		Physics() = default;

		glm::vec3 lastPosition;
		glm::vec3 velocity;
		float gravity;
		bool renderCollision;
		bool movable;
	};
	class ModelRenderer : public Component {
	public:
		static const ComponentEnum ID = ComponentEnum::ModelRenderer;
		ModelRenderer() = default;

		ModelAsset* asset;
		bool visible;
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
		char instanceName[20];
		AnimationAsset* asset;
	};
	class Animator : public Component {
	public:
		static const ComponentEnum ID = ComponentEnum::Animator;
		Animator() = default;

		ModelAsset* asset; // animations are stored in here

		static const int maxAnimations = 4;
		AnimationProperty enabledAnimations[maxAnimations];

		void Update(float delta);

		void Blend(const std::string& name, float blend);
		void Speed(const std::string& name, float speed);
		/*
		Second argument can be done like this: {frame, loop, blend, speed}
		*/
		void Enable(const std::string& instanceName, const std::string& animationName, AnimationProperty prop);
		void Disable(const std::string& name);
	};
	class MeshRenderer : public Component {
	public:
		static const ComponentEnum ID = ComponentEnum::MeshRenderer;
		MeshRenderer() = default;

		MeshAsset* asset;
		bool visible;
	};
	class Collision : public Component {
	public:
		static const ComponentEnum ID = ComponentEnum::Collision;
		Collision() = default;

		ColliderAsset* asset;
		bool visible;
	};
}