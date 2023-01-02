//const int size = 5;
	//int buffer[size]{0};
	//int in = 0;
	//int out = 0;
	//bool empty = true;
	//auto queue = [&](int num) {
	//	if (in == out && !empty) {
	//		printf("Full! %d\n",num);
	//		return;
	//	}
	//	buffer[in] = num;
	//	in = (in + 1) % size;
	//	empty = false;
	//};
	//auto unqueue = [&]() {
	//	if (in == out)
	//		return;
	//	buffer[out];
	//	out = (out + 1) % size;
	//	if (in == out)
	//		empty = true;
	//};
	//auto print = [&]() {
	//	for (int i = 0; i < size; i++) {
	//		if (i == out&&!empty) {
	//			printf("[");
	//		} else {
	//			printf(" ");
	//		}
	//		printf("%d", buffer[i]);
	//		if (i == ((in+size-1)%size)&&!empty) {
	//			printf("]");
	//		} else {
	//			printf(" ");
	//		}
	//	}
	//	printf("\n");
	//};
	//print();
	//unqueue();
	//print();
	//queue(1);
	//queue(3);
	//queue(3);
	//print();
	//queue(3);
	//queue(5);
	//queue(9);
	//print();