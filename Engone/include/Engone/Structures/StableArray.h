#pragma once

#include "Engone/Structures/Stack.h"

namespace engone {
	
	// An added value will have the same index until it is removed.
	// remove will cause a position in the array to be empty.
	// If a value is added then that position will probably be filled by the new value.
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
		uint32_t size() {
			return valueCount;
		}
		// The furthest index used + 1
		uint32_t range() {
			return m_values.used+1;
		}
		// Returns index of where value was put
		// Returns -1 when function fails
		uint32_t add(const Value& value) {
			uint32_t index = -1;
			if (m_emptySpots.size() != 0) {
				index = m_emptySpots.pop();
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
		Value* get(uint32_t index) {
			if (m_values.used <= index) {
				return nullptr;
			}
			for (int i = 0; i < m_emptySpots.size(); i++) {
				uint32_t index2 = m_emptySpots.peek(i);
				if (index == index2) {
					return nullptr;
				}
			}
			return ((Value*)m_values.data + index);
		}
		Value remove(uint32_t index) {
			bool temp;
			return remove(index,temp);
		}
		Value remove(uint32_t index, bool& success) {
			if (m_values.used <= index) {
				success = false;
				return {}; // fail
			}

			valueCount--;
			m_emptySpots.push(index);
			success = true;
			return *((Value*)m_values.data + index);
		}

		// this function does not allow emptySpots to be a Stack.
		// emptySpots needs to be an Array where you can pop middle elements.
		//Value& operator[](uint32_t index) {
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
		//	uint32_t indexInEmpty = -1;
		//	for (int i = 0; i < m_emptySpots.size(); i++) {
		//		uint32_t index2 = m_emptySpots.peek(i);
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
		bool reserve(uint32_t count) {
			return m_values.resize(count);
		}

	private:
		Memory m_values{sizeof(Value)};
		Stack<uint32_t> m_emptySpots; // Empty spots in the middle. Free spots from the right is stored in m_values.used.
		uint32_t valueCount=0;
	};
	
	void EngoneStableArrayTest();
}