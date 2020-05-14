#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "Main_Functionalities.h"

int main()
{
	/*
	std::string test;

	std::ifstream file("C:/Users/Pavel/Desktop/SAMPLE_XML.xml");

	while (file)
	{
		getline(file, test);
		std::cout << test << '\n';
	}
	*/

	std::string filename;
	std::cin >> filename;

	open(filename);


	return 0;
}