#include "Engone/Structures/Array.h"

namespace engone {
	void Array::cleanup() {
		m_values.resize(0);
	}
	uint32 Array::size() {
		return m_values.used;
	}
	void* Array::data() {
		return m_values.data;
	}
	// Item is added to the back.
	// Returns false if allocation failed
	bool Array::add(const void* value, void** outPtr) {
		if (m_values.used == m_values.max) {
			bool yes = m_values.resize(m_values.max * 2 + 4);
			if (!yes) return false;
		}
		void* ptr = (char*)m_values.data + m_values.used * m_values.getTypeSize();
		if (value)
			memcpy(ptr,value,m_values.getTypeSize());
		else
			memset(ptr, 0, m_values.getTypeSize());
		if (outPtr)
			*outPtr = ptr;
		//new(ptr)Value(value);
		m_values.used++;
		return true;
	}
	void Array::clear(){
		m_values.used=0;
	}
	bool Array::insert(uint32 index, const void* value) {
		if (index >= m_values.used) {
			return false;
		}
		if (m_values.used == m_values.max) {
			bool yes = m_values.resize(m_values.max * 2 + 4);
			if (!yes) return false;
		}
		uint32 ts = m_values.getTypeSize();
		char* ptr = (char*)m_values.data + index * ts;

		memmove(ptr+ ts,ptr, (m_values.used-index)* ts);

		if (value)
			memcpy(ptr, value, ts);
		else
			memset(ptr, 0, ts);

		m_values.used++;
		return true;
	}
	bool Array::copy(Array* out) {
		bool yes = out->reserve(m_values.max);
		if (!yes) {
			return false;
		}
		out->m_values.used = m_values.used;

		memcpy(out->m_values.data, m_values.data, m_values.max * m_values.getTypeSize());
		return true;
	}
	// nullptr if unused/invalid index
	void* Array::get(uint32 index) {
		if (index >= m_values.used) {
			return nullptr;
		}
		return ((char*)m_values.data + index *m_values.getTypeSize());
	}
	void Array::remove(uint32 index) {
		if (index >= m_values.used) {
			return;
		}
		uint32 ts = m_values.getTypeSize();
		char* ptr = (char*)m_values.data + index * ts;
		memmove(ptr, ptr + ts, (m_values.used - index) * ts);
		m_values.used--;
	}
	void TestEngoneArray() {


	}
}