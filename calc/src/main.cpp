
#include <iostream>

#include "add.h"
#include "operators/adder/adder.h"
#include "operators/multiplier.h"

int main()
{
	int a = 2;
	int b = 3;

	std::cout << a << "+" << b << "=" << add(a, b) << std::endl;

	Adder ad;
	Multiplier ml;

	std::cout << a << " plus " << b << " is " << ad.calc(a, b) << std::endl;
	std::cout << a << " times " << b << " is " << ml.calc(a, b) << std::endl;

	return 0;
}

