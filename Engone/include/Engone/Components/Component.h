#pragma once

#include "Engone/Logger.h"
#include "Engone/AssetModule.h"
#include "reactphysics3d/reactphysics3d.h"

namespace engone {

	// Enums are order in alphabetic order except for None.
	enum ComponentMask : uint8_t {
		ComponentNone = 0,
		ComponentAnimator=1,
		ComponentCollisionBody=2, // reactphysics
		ComponentMesh=4,
		ComponentModel=8,
		ComponentRigidBody=16, // reactphysics
	};
	//class ComponentMask {
	//protected:
	//public:
	//	uint32_t componentMask=0;
	//	ComponentMask() = default;
	//	ComponentMask(uint32_t mask);
	//	ComponentMask(ComponentEnum mask);
	//	ComponentMask(const ComponentMask& mask);
	//	ComponentMask(ComponentMask* mask);

	//	ComponentMask operator|(ComponentEnum filter);

	//	bool same(ComponentMask& filter) {
	//		return componentMask == filter.componentMask;
	//	}
	//	bool same(ComponentMask* filter) {
	//		return componentMask == filter->componentMask;
	//	}
	//	bool has(ComponentMask& filter) {
	//		uint32_t a = componentMask ^ filter.componentMask;
	//		a = a | (~filter.componentMask);
	//		a = ~a;
	//		return a==filter.componentMask;
	//	}
	//	bool has(ComponentMask* filter) {
	//		//log::out << componentMask << " " << filter->componentMask;
	//		uint32_t a = componentMask ^ filter->componentMask;
	//		uint32_t b = a | (~filter->componentMask);
	//		//log::out << a << " > " << b;
	//		uint32_t c = ~b;
	//		return c == filter->componentMask;
	//	}
	//	bool has(ComponentEnum filter) {
	//		//log::out << componentMask << " " << ((int)filter)<<" "<< (componentMask & (1 << ((int)filter - 1)))<< " \n";
	//		return componentMask & (1<<((uint32_t)filter-1));
	//	}
	//};
	//ComponentMask operator|(ComponentEnum a, ComponentEnum b);

	class Component {
	public:
		static const ComponentMask ID = ComponentNone;
		template<class T>
		T* cast() {
			return reinterpret_cast<T>(this);
		}
	};

	class CollisionBody : public Component {
	public:
		static const ComponentMask ID = ComponentCollisionBody;
		CollisionBody() = default;

		rp3d::CollisionBody* collisionBody;
	};
	class RigidBody : public Component {
	public:
		static const ComponentMask ID = ComponentRigidBody;
		RigidBody() = default;

		rp3d::RigidBody* rigidBody;
	};
	class ModelRenderer : public Component {
	public:
		static const ComponentMask ID = ComponentModel;
		ModelRenderer() = default;

		ModelAsset* asset;
		bool invisible;
	};
	struct AnimatorProperty {
		bool loop;
		float blend;
		float speed;// multiplier to the default speed
		float frame;
		char instanceName[20];
		AnimationAsset* asset;
	};
	class Animator : public Component {
	public:
		static const ComponentMask ID = ComponentAnimator;
		Animator() = default;

		ModelAsset* asset; // animations are stored in here
		// may not need to store this here, because if animator exists,
		// there should be a ModelRenderer

		static const uint32_t maxAnimations = 4;
		AnimatorProperty enabledAnimations[maxAnimations];

		void Update(float delta);

		void Blend(const std::string& name, float blend);
		void Speed(const std::string& name, float speed);
		/*
		Second argument can be done like this: {frame, loop, blend, speed}
		*/
		void Enable(const std::string& instanceName, const std::string& animationName, AnimatorProperty prop);
		void Disable(const std::string& name);
	};
	class MeshRenderer : public Component {
	public:
		static const ComponentMask ID = ComponentMesh;
		MeshRenderer() = default;

		MeshAsset* asset;
		bool invisible;
	};
}