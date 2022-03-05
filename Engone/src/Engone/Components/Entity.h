#pragma once

#include "Component.h"

namespace engone {

	struct Collider;
	class EntityStack;
	class Entity : public ComponentMask {
	private:
		ComponentMask operator|(ComponentEnum b) {};

		friend class EntityStack;

	public:
		int* componentSizes = nullptr;// pointing to an array in entityStack;
		char* stackPtr = nullptr;// pointing to component memory in EntityStack

		uint64_t entityId=0;
		//uint64_t entityClass=0u-1;

		Entity(ComponentMask mask);
		Entity(ComponentMask mask, int* sizes, char* ptr);

		template <class T>
		T* getComponent() {
			//log::out << componentMask << " - " << (int)T::MASK << "\n";
			if (has(T::MASK)) {
				if (!stackPtr)
					return nullptr;
				int index = ((int)T::MASK) - 1;
				return (T*)(stackPtr + componentSizes[index]);
			}
			return nullptr;
		}
		
		virtual void Init() {}
		virtual void Update(float delta) {}
		virtual void OnCollision(Collider& c1, Collider& c2) {}
	};
	struct Collider {
		ColliderAsset* asset;
		Entity entity;

		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
	};
}