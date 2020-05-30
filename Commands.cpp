#include <iostream>
#include <sstream>

#include "Commands.h"
#include "XML_File.h"

std::string command;
xmlFile file;


//**********************************************************************************************************\\
|-----------------------------------------> COMMAND FUNCTIONS <----------------------------------------------|

void open() // Opens file
{
	file.open();

	enterCommand(&isCloseOrExit, &openCommandList);

	std::cout << "Closing...\n";
	file.clear();
	std::cout << "Closed\n";
}

void save() // Saves file to original path
{
	file.save();
	std::cout << "Succesfully saved " << file.getPath() << '\n';
}

void saveas() // Saves file to specified path
{
	file.saveas();
	std::cout << "Succesfully saved as " << file.getPath() << '\n';
}

void help() // Prints supported functionalities
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
				" -prints attribute value by given element id and key\n"
				"set <id> <key> <value>\n"
				" -sets attribute value by given element id and key\n"
				"settext <id> <text>"
				" -sets text content of element by given element id"
				"children <id>\n"
				" -prints a list of id's children's attributes\n"
				"child <id> <n>\n"
				" -prints nth child of element identified by id\n"
				"text <id>\n"
				" -prints id text content\n"
				"delete <id> <key>\n"
				" -deletes id's attribute by key\n"
				"newattribute <id>\n"
				" -creates a new attribute to element identified by id\n"
				"newchild <id>\n"
				" -creates a new child to element identified by id, with it's own id and nothing else\n"
				"xpath <id> <XPath>\n"
				" -simple XPath 2.0 expressions\n"
				"help\n"
				" -prints this information\n"
				"exit\n"
				" -exits the program\n";
}

void print() // Prints loaded file data 
{
	file.print();
}

void select() // Prints attribute value by given element id and attribute key
{
	std::string id, key;
	std::cin >> id >> key;

	file.select(id, key);
}

void set() // Sets attribute value by given element id and attribute key
{
	std::string id, key, value;
	std::cin >> id >> key >> value;

	file.set(id, key, value);
}

void settext() // Sets text content of element by given element id
{
	std::string id, text;
	std::cin >> id ;
	std::cin.get();
	getline(std::cin, text);

	file.settext(id, text);
}

void children() // Prints all attributes belonging to children of element identified by id
{
	std::string id;
	std::cin >> id;

	file.children(id);
}

void child() // Prints nth child of element identified by id
{
	std::string id;
	size_t n;

	std::cin >> id >> n;

	file.child(id, n);
}

void text() // Prints text content of element identified by id
{
	std::string id;
	std::cin >> id;

	file.text(id);
}

void deleteattribute() // Removes attribute by given element id and attribute key
{
	std::string id, key;
	std::cin >> id >> key;

	file.deleteattribute(id, key);
}

void newattribute() // Creates new attribute by given element id and attribute key
{
	std::string id, key, value;
	std::cin >> id >> key >> value;

	file.newattribute(id, key, value);
}

void newchild() // Creates new child of element identified by id
{
	std::string id;
	std::cin >> id;

	file.newchild(id);
}

void xpath() // Recognises few basic XPath2.0 expressions, expects mostly ideal input, can handle some falsely formatted input as correct
{
	std::string xpath;
	getline(std::cin, xpath);

	file.xpath(xpath);
}



//**********************************************************************************************************\\
|----------------------------------------> COMMANDLINE CODE <------------------------------------------------|

bool mainCommandList(std::string command)
{
	return command == "open" || command == "help" || command == "exit";
}

bool openCommandList(std::string command)
{
	return command == "close" || command == "save" || command == "saveas"
		|| command == "help" || command == "exit" || command == "print" 
		|| command == "select" || command == "set" || command == "settext" 
		|| command == "children" || command == "child" || command == "text"
		|| command == "delete"  || command == "newattribute" || command == "newchild" 
		|| command == "xpath";
}

void executeCommand(std::string command)
{
	if (command == "help") help();
	else if (command == "open") open();
	else if (command == "save") save();
	else if (command == "saveas") saveas();
	else if (command == "print") print();
	else if (command == "select") select();
	else if (command == "set") set();
	else if (command == "settext") settext();
	else if (command == "children") children();
	else if (command == "child") child();
	else if (command == "text") text();
	else if (command == "delete") deleteattribute();
	else if (command == "newattribute") newattribute();
	else if (command == "newchild") newchild();
	else if (command == "xpath") xpath();
}

bool isExit(std::string command)
{
	return command == "exit";
}

bool isCloseOrExit(std::string command)
{
	return command == "close" || command == "exit";
}

void enterCommand(std::function<bool(std::string)> terminationPredicate,
	std::function<bool(std::string)> commandList)
{
	do
	{
		do
		{
			std::cout << '>';
			std::cin >> command;

		} while (!commandList(command));

		executeCommand(command);

	} while (!terminationPredicate(command));
}
