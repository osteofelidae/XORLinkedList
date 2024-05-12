// === DEPENDENCIES ===========================================================
#include "xorLinkedList.hpp"
#include <iostream>


// === MAIN CLASS =============================================================
int main(
	int argc,  // Argument count
	char *argv[]  // Arguments
) {
	
	XorLinkedList<int> *xll = new XorLinkedList<int>();

	for (int i = 0; i < 20; i += 2) {
		xll->add(i);
		//xll->print();
	}
	
	xll->print();
	xll->contains(4);
	xll->insert(3, 2);
	xll->insert(5, 4);
	xll->print();

	delete xll;

}