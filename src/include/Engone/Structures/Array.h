#pragma once

#include "Engone/Utilities/Alloc.h"

namespace engone {

	class Array {
	public:
		Array(u32 typeSize, u32 allocType) : m_values(typeSize,allocType) {}
		// calls cleanup
		~Array() { cleanup(); }
		// frees allocations
		void cleanup();

		u32 size();
		void* data();

		// Item is added to the back.
		// Returns false if allocation failed
		bool add(const void* value, void** outPtr=nullptr);

		bool insert(u32 index, const void* value);

		// nullptr if unused/invalid index
		void* get(u32 index);
		void remove(u32 index);

		bool copy(Array* out);
		
		void clear();

		// Allocate memory in advance.
		// Method can trim values if less than size
		// count is not in bytes!
		bool reserve(u32 count) {
			return m_values.resize(count);
		}
		bool resize(u32 count) {
			if (!m_values.resize(count))
				return false;
			m_values.used = count;
			return true;
		}
	private:
		Memory m_values;
	};

	void TestEngoneArray();
}