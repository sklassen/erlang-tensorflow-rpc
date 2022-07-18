#include <iostream>
#include "cppflow/cppflow.h"

int main() {
	auto a = cppflow::tensor({1.0,2.0,3.0});
	auto b = cppflow::fill({3},1.0);

	std::cout << a+b << std::endl;

	return 0;
}
