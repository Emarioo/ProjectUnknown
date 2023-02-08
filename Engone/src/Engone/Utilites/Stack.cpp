
#include "Engone/Utilities/Stack.h"

namespace engone {
#define POP printf("Pop - num:%d yes:%d\n",num,yes);
#define PUSH printf("Push - yes:%d\n",yes);
#define SIZE printf("Size - num:%d\n",num);
	void EngoneStackTest() {
		int num=0;
		bool yes=0;

		Stack<int> stack;
		
		num = stack.pop(yes); POP;
		yes = stack.push(52); PUSH;
		num = stack.size(); SIZE;
		yes = stack.push(923); PUSH;
		num = stack.size();	SIZE;
		num = stack.pop(yes); POP;
		num = stack.size(); SIZE;
		num = stack.pop(yes); POP;
		num = stack.pop(yes); POP;
		num = stack.size(); SIZE;

		// improve testing by randomly generating stuff?
		// analyze with time?

		std::cin.get();
	}
}