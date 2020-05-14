#ifndef MAINFUNCS_H
#define MAINFUNCS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "XML_Elements.h"

bool is_xml_file(const std::string& filename)
{
	if (filename.size() < 5) return false;
	if (filename.substr(filename.size() - 4, 4) == ".xml") return true;
	else return false;
}

void read_tag(std::ifstream& xml_file)
{
	char c;
	xml_file.get(c);
	if (c != '<') return;

	std::string tag;
	while (xml_file.get(c) && c != '>')
	{
		tag += c;
	}

	std::stringstream ss(tag);
	std::string key;
	ss >> key;

	std::cout << key;
}

void read_xml(std::ifstream& xml_file)
{
	std::vector<std::string> opened_tags;

	read_tag(xml_file);

	/*
	std::string test;
	getline(xml_file, test);
	std::cout << test;
	*/
}

void open(std::string filename)
{
	if (is_xml_file(filename))
	{
		std::ifstream xml_file(filename.data());
		if (!xml_file)
		{
			std::ofstream new_file(filename.data());
			new_file.close();
			return;
		}

		std::cout << "Successfully opened " << filename << '\n';

		/*CODE HERE TO LOAD INFO*/

		std::string xml_version;
		getline(xml_file, xml_version);
		if (xml_version.substr(0, 5) != "<?xml")
		{
			xml_version.clear();
			xml_file.seekg(0);
		}
		else std::cout << xml_version << '\n';

		read_xml(xml_file);

		xml_file.close();
	}
	else std::cout << "Invalid file\n";

	std::cout << "\nBack to main...\n";
}

void close()
{

}

void save()
{

}

void saveas()
{

}

void help()
{
	std::cout << "\nThe following commands are supported:\n\n"
		"open <file>\n"
		" -reads information from <file>\n"
		"close\n"
		" -frees memory\n"
		"save\n"
		" -saves information to origin file\n"
		"saveas <file>\n"
		" -saves information to <file>\n"
		"print\n"
		" -prints out loaded XML file content\n"
		"select <id> <key>\n"
		" -prints attribute value by given id and key\n"
		"set <id> <key> <value>\n"
		" -sets attribute value by given id and key\n"
		"children <id>\n"
		" -prints a list of id's children's attributes\n"
		"child <id> <n>\n"
		" -prints nth child of id\n"
		"text <id>\n"
		" -prints id text content\n"
		"delete <id> <key>\n"
		" -deletes id's attribute by key\n"
		"newchild <id>\n"
		" -creates a new child to id, with it's own id and nothing else\n"
		"xpath <id> <XPath>\n"
		" -simple XPath 2.0 expressions\n"
		"help\n"
		" -prints this information\n"
		"exit\n"
		" -exits the program\n";
}

void exit()
{

}

/*

--print
Извежда на екрана прочетената информация от XML файла (в рамките на посочените по-горе ограничения за поддържаната информация).
Печатането да е XML коректно и да е “красиво”, т.е. да е форматирано визуално по подходящ начин (например, подчинените елементи да са по-навътре)
--select <id> <key>
Извежда стойност на атрибут по даден идентификатор на елемента и ключ на атрибута
--set <id> <key> <value>
Присвояване на стойност на атрибут
--children <id>
Списък с атрибути на вложените елементи
--child <id> <n>
Достъп до n-тия наследник на елемент
--text <id>
Достъп до текста на елемент
--delete <id> <key>
Изтриване на атрибут на елемент по ключ
--newchild <id>
Добавяне на НОВ наследник на елемент. Новият елемент няма никакви атрибути, освен идентификатор
--xpath <id> <XPath>
операции за изпълнение на прости XPath 2.0 заявки към даден елемент, която връща списък от XML елементи

*/


void print()
{

}

#endif MAINFUNCS_H