#pragma once

#include "Engone/Components/Component.h"

namespace engone {

	class EntityStack;
	class Entity {
	public:
		uint64_t entityId=0;

		Entity(ComponentMask mask);
		Entity(int mask);
		Entity(ComponentMask mask, size_t* sizes, char* ptr);

		template <class T>
		T* getComponent() {
			if (has(T::ID)) {
				if (!stackPtr) {
					log::out << log::RED << "Missing component. Did you forget AddEntity(...)\n";
					return nullptr;
				}
				size_t index = ((size_t)T::ID) - 1;
				return (T*)(stackPtr + componentSizes[index]);
			}
			log::out << log::RED << "Missing component. Did you forget AddEntity(...)\n";
			return nullptr;
		}
		
		virtual void Init() {}

		inline ComponentMask getMask() const { return m_mask; }

	private:
		ComponentMask m_mask;

		size_t* componentSizes = nullptr;// pointing to an array in entityStack;
		char* stackPtr = nullptr;// pointing to component memory in EntityStack
		
		friend class EntityStack;
	};
}