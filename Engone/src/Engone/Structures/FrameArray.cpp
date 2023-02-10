#include "Engone/Utilities/FrameArray.h"

namespace engone {
	void FrameArrayTest() {
		struct Apple {
			float x, y, size;
			void print() {
				printf("{%f, %f, %f}\n", x,y,size);
			}
		};
		FrameArray<Apple> arr(80);

		int id_a = arr.add({25,15,92});
		int id_b = arr.add({29,617,92});
		
		arr.remove(id_a);

		Apple* first = arr.get(0);

		printf("%p\n", first);

		int id_c = arr.add({ 92,61,22 });

		//printf("%d\n", id_c);
		//a->print();
		//b->print();
		
		FrameArray<Apple>::Iterator iterator;
		while (arr.iterate(iterator)) {
			iterator.ptr->print();
		}


		std::cin.get();
	}
}