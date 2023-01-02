#include "Engone/Utilities/StableArray.h"

namespace engone {

#define ADD(X) printf("Add: %u\n",arr.add(X));
#define REMOVE(X) printf("Remove %u\n",arr.remove(X));
#define SIZE() printf("Size %u\n",arr.size());
	void EngoneStableArrayTest() {
		StableArray<int> arr;
		
		ADD(6);
		ADD(26);
		REMOVE(0);
		SIZE();
		ADD(263);
		REMOVE(1);
		SIZE();

		std::cin.get();
	}
}