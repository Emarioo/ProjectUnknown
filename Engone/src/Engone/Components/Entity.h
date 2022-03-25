#pragma once

#include "Component.h"

namespace engone {

	class EntityStack;
	class Entity : public ComponentMask {
	private:
		ComponentMask operator|(ComponentEnum b) {};

		friend class EntityStack;

		int* componentSizes = nullptr;// pointing to an array in entityStack;
		char* stackPtr = nullptr;// pointing to component memory in EntityStack
	public:

		uint64_t entityId=0;

		Entity(ComponentMask mask);
		Entity(ComponentMask mask, int* sizes, char* ptr);

		template <class T>
		T* getComponent() {
			if (has(T::ID)) {
				if (!stackPtr)
					return nullptr;
				int index = ((int)T::ID) - 1;
				return (T*)(stackPtr + componentSizes[index]);
			}
			return nullptr;
		}
		
		virtual void Init() {}
	};
}