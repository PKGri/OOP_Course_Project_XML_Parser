#include <iostream>

#include "Commands.h"


int main()
{
	enterCommand(&isExit, &mainCommandList);

	std::cout << "Exiting";

	return 0;
}