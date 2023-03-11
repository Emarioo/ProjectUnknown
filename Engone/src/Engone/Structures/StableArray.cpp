#include "Engone/Structures/StableArray.h"

namespace engone {

#define TEST_STABLE_ADD(X) printf("Add: %u\n",arr.add(X));
#define TEST_STABLE_REMOVE(X) printf("Remove %u\n",arr.remove(X));
#define TEST_STABLE_SIZE() printf("Size %u\n",arr.size());
	void EngoneStableArrayTest() {
		StableArray<int> arr;
		
		TEST_STABLE_ADD(6);
		TEST_STABLE_ADD(26); // 
		TEST_STABLE_REMOVE(0);
		TEST_STABLE_SIZE();
		TEST_STABLE_ADD(263);
		TEST_STABLE_REMOVE(1);
		TEST_STABLE_SIZE();

		std::cin.get();
	}
}