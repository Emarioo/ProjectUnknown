#include "Gamecore.h"
/*
class Oliver {// Size
public:
	int taste=0;
	Oliver() {}
	void Data(int t){
		taste = t;
	}
};
class Bean {// Text
public:
	Bean() {}
	Oliver* ol;
};
class Can {// Elem
public:
	Can() {}
	Bean bean;
	Oliver ol;
};
*/

int main(void) {
	/*
	std::vector<Can> cans;

	Can c1;
	c1.ol.Data(25);
	c1.bean.ol = &c1.ol;
	cans.push_back(c1);

	Can c2;
	c2.ol.Data(10);
	c2.bean.ol = &c2.ol;
	cans.push_back(c2);
	
	cans.push_back(Can());
	Can* c = &cans.at(0);
	c->ol.Data(25);
	c->bean.ol = &c->ol;

	cans.push_back(Can());
	c = &cans.at(1);
	c->ol.Data(10);
	c->bean.ol = &c->ol;
	
	std::cout << cans.at(0).bean.ol->taste  << std::endl;
	std::cout << cans.at(1).bean.ol->taste << std::endl;

	std::cin.get();
	*/
	Gamecore game= Gamecore();
}