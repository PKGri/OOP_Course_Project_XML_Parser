#ifndef XML_ELEMENTS_H
#define XML_ELEMENTS_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class Attribute
{
private:
	std::string key;
	std::string value;
public:
	Attribute();
};

class XML_Element
{
private:
	std::string key;
	std::vector<Attribute> attributes;
	std::vector<XML_Element> children;
	std::string content;
public:
	XML_Element();

	static void create_XML_Element(std::ifstream& xml_file)
	{
		std::string test;
		getline(xml_file, test);
		std::cout << test;
	}
};

#endif XML_ELEMENTS_H