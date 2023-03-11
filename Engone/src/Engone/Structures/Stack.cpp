
#include "Engone/Structures/Stack.h"

namespace engone {
	Stack::Stack(uint32 typeSize, uint32 allocType) :
		m_values(typeSize,allocType) {}
	bool Stack::push(void* value) {
		if (m_values.used == m_values.max) {
			bool yes = m_values.resize(m_values.max * 2 + 4);
			if (!yes) return false;
		}
		uint32_t index = m_values.used;
		m_values.used++;

		void* ptr = (char*)m_values.data + index * m_values.getTypeSize();

		if(value)
			memcpy(ptr, value, m_values.getTypeSize());
		else
			memset(ptr, 0, m_values.getTypeSize());
		//new(ptr)Value(value);

		return true;
	}
	// memcpy into outPtr is done
	bool Stack::pop(void* outPtr) {
		if (m_values.used == 0) {
			if (outPtr)
				memset(outPtr, 0, m_values.getTypeSize());
			return false;
		}
		m_values.used--;

		void* ptr = (char*)m_values.data + m_values.used * m_values.getTypeSize();
		if(outPtr)
			memcpy(outPtr, ptr,m_values.getTypeSize());
		memset(ptr, 0, m_values.getTypeSize());
		return true;
	}
	void* Stack::peek(uint32_t index) {
		if (m_values.used <= index) {
			return 0;
		}
		return (char*)m_values.data + m_values.used * m_values.getTypeSize();
	}

	bool Stack::copy(Stack& outStack) {
		bool yes = outStack.m_values.resize(m_values.max);
		if (!yes) {
			//cleanup();
			return false;
		}
		outStack.m_values.used = m_values.used;

		memcpy(outStack.m_values.data, m_values.data, m_values.max * m_values.getTypeSize());
		return true;
	}

#define TEST_STACK_POP() yes = stack.pop(&num); printf("Pop %d - yes:%d\n",num,yes);
#define TEST_STACK_PUSH(V) num=V; yes = stack.push(&num); printf("Push %d - yes:%d\n",num,yes);
#define TEST_STACK_SIZE printf("Size - num:%d\n",stack.size());
	void EngoneStackTest() {
		Stack stack(sizeof(int),ALLOC_TYPE_HEAP);
		int num = 0;
		bool yes = false;

		TEST_STACK_POP();
		TEST_STACK_PUSH(52);
		TEST_STACK_SIZE;
		TEST_STACK_PUSH(923);
		TEST_STACK_SIZE;
		TEST_STACK_POP();
		TEST_STACK_SIZE;
		TEST_STACK_POP();
		TEST_STACK_POP();
		TEST_STACK_SIZE;

		// improve testing by randomly generating stuff?
		// analyze with time?

		std::cin.get();
	}
}