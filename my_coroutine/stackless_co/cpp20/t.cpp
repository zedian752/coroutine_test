
#include "serial_number_generator.h"
#include <iostream>

using namespace std;


int main(int arg, char ** args) {
	auto gen = sequence();
	for (int i = 0; i < 5; ++i) {
		std::cout << gen.next() << std::endl;
	}

}