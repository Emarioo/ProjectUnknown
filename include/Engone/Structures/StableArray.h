#pragma once

#include "Engone/Structures/Stack.h"

namespace engone {
	
	// An added value will have the same index until it is removed.
	// remove will cause a position in the array to be empty.
	// If a value is added then that position will probably be filled by the new value.
    // Addresses are not stable. Resizes occur. It is only the indexes that are stable.
	// REMOVE TEMPLATE!
	template<typename Value>
	class StableArray {
	public:
		StableArray() = default;
		// calls cleanup
		~StableArray() {
			cleanup();
		}
		// frees allocations
		void cleanup() {
			m_values.resize(0);
			m_emptySpots.cleanup();
		}
		u32 size() {
			return valueCount;
		}
		// The furthest index used + 1
		u32 range() {
			return m_values.used+1;
		}
		// Returns index of where value was put
		// Returns -1 when function fails
		u32 add(const Value& value) {
			u32 index = -1;
			if (m_emptySpots.size() != 0) {
				m_emptySpots.pop(&index);
			} else {
				if (m_values.used == m_values.max) {
					bool yes = m_values.resize(m_values.max * 2 + 4);
					if (!yes) return -1;
				}
				index = m_values.used;
				m_values.used++;
			}
			valueCount++;
			Value* ptr = (Value*)m_values.data + index;
			new(ptr)Value(value);
			//*ptr = value;
			return index;
		}
		// nullptr if unused/invalid index
		Value* get(u32 index) {
			if (m_values.used <= index) {
				return nullptr;
			}
			for (int i = 0; i < m_emptySpots.size(); i++) {
				u32 index2 = *(u32*)m_emptySpots.peek(i);
				if (index == index2) {
					return nullptr;
				}
			}
			return ((Value*)m_values.data + index);
		}
		Value remove(u32 index) {
			bool temp;
			return remove(index,temp);
		}
		Value remove(u32 index, bool& success) {
			if (m_values.used <= index) {
				success = false;
				return {}; // fail
			}

			valueCount--;
			m_emptySpots.push(&index);
			success = true;
			return *((Value*)m_values.data + index);
		}

		// this function does not allow emptySpots to be a Stack.
		// emptySpots needs to be an Array where you can pop middle elements.
		//Value& operator[](u32 index) {
		//	if (m_values.used < index)
		//		assert((false, "out of bounds"));
		//	if (m_values.used == index) {
		//		if (m_values.used == m_values.max) {
		//			bool yes = m_values.resize(m_values.max * 2 + 4);
		//			if (!yes) assert((false, "failed allocation"));
		//		}
		//		m_values.used++;
		//		*(m_values.data + index) = {};
		//		return *(m_values.data + index);
		//	}
		//	u32 indexInEmpty = -1;
		//	for (int i = 0; i < m_emptySpots.size(); i++) {
		//		u32 index2 = m_emptySpots.peek(i);
		//		if (index == index2) {
		//			indexInEmpty = i;
		//			break;
		//		}
		//	}
		//	if(index)
		//}

		// Allocate memory in advance.
		// Method can trim values if less than size
		// count is not in bytes!
		bool reserve(u32 count) {
			return m_values.resize(count);
		}

	private:
		Memory m_values{sizeof(Value),ALLOC_TYPE_HEAP};
		Stack m_emptySpots{sizeof(u32),ALLOC_TYPE_HEAP}; // Empty spots in the middle. Free spots from the right is stored in m_values.used.
		u32 valueCount=0;
	};
	
	void EngoneStableArrayTest();
}