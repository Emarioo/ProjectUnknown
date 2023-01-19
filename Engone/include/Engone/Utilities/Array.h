#pragma once

#include "Engone/Utilities/Alloc.h"

namespace engone {

	// An added value will have the same index until it is removed.
	// remove will cause a position in the array to be empty.
	// If a value is added then that position will probably be filled by the new value.
	template<typename Value>
	class Array {
	public:
		Array() = default;
		// calls cleanup
		~Array() {
			cleanup();
		}
		// frees allocations
		void cleanup() {
			m_values.resize(0);
		}
		uint32_t size() {
			return m_values.used;
		}
		// Item is added to the back.
		// Returns false if allocation failed
		bool add(const Value& value) {

			if (m_values.used == m_values.max) {
				bool yes = m_values.resize(m_values.max * 2 + 4);
				if (!yes) return false;
			}

			Value* ptr = m_values.data + m_value.used;
			new(ptr)Value(value);
			return true;
		}
		bool insert(const Value& value) {

		}
		// nullptr if unused/invalid index
		Value* get(uint32_t index) {
			if (index >= m_values.used) {
				return nullptr;
			}
			if (!m_values.data)
				return nullptr;
			return (m_values.data + m_values.used);
		}
		void remove(uint32_t index) {
			if (!m_values.data)
				return;

			if (index >= m_values.used){
				return;
			}
			
			//m_values;
			//m_values.

			m_values.used--;
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
		Memory<Value> m_values;
	};

	void EngoneStableArrayTest();
}