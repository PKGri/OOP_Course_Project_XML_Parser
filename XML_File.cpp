#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

#include "XML_File.h"
#include "XPath_Node.h"

//**********************************************************************************************************\\
|-----------------------------------------> PARSING FUNCTIONS <----------------------------------------------|

bool isXmlFile(const std::string& filename) // Checks file extension
{
	if (filename.size() < 5) return false;
	if (filename.substr(filename.size() - 4, 4) == ".xml") return true;
	else return false;
}

std::string parseTag(std::ifstream& xml_file) 
{
	std::streampos begin(xml_file.tellg());

	char c;
	xml_file >> c;
	if (c != '<') // IF TEXT CONTENT
	{
		xml_file.seekg(begin);
		return "Not tag";
	}

	std::string tag;
	while (xml_file.get(c) && c != '>')
	{
		tag += c;
	}

	return tag;
}

bool isEmptyTag(std::string& tag) // Checks if tag format is <tag/>
{
	if (tag.back() == '/') return true;
	return false;
}

bool isClosingTag(std::string& tag) // Checks if tag format is </tag>
{
	std::regex closingTag("/[^ \n\t]+[ \n\t]*");

	return regex_match(tag, closingTag);
}

void parseClosingTag(std::string& tag) // Removes '\' from closing tag
{
	std::stringstream ss(tag);
	char c;
	ss.get(c);
	ss >> tag;
}

std::string parseTextTagContent(std::ifstream& xml_file) // Reads text content of opened text tag
{
	std::string text;
	
	char c;
	xml_file.get(c);

	while (c != '<')
	{
		text += c;
		xml_file.get(c);
	}
	
	xml_file.unget();

	return text;
}

std::vector<XPathNode> parseXPath(std::string xpath)
{
	std::regex node("/[^ \t&<>/\\[\\]]+[^&<>/]*");

	std::sregex_iterator currentNodeMatch(xpath.begin(), xpath.end(), node);
	std::sregex_iterator lastMatch;

	std::vector<XPathNode> nodes;

	while (currentNodeMatch != lastMatch)
	{
		std::smatch nodeMatch = *currentNodeMatch;

		std::string node(nodeMatch.str().substr(1));

		XPathNode toAdd;

		toAdd.parseNode(node);

		nodes.push_back(toAdd);

		currentNodeMatch++;
	}

	return nodes;
}


//**********************************************************************************************************\\
|-------------------------------------------> XMLFILE CLASS <------------------------------------------------|

xmlElement& xmlFile::listNth(const size_t n) // Returns Nth element in Elements
{
	std::list<xmlElement>::iterator element = Elements.begin();
	std::advance(element, n);
	return *element;
}

void xmlFile::uniquifyID() // Name says it all, makes all IDs in file unique
{
	for (std::list<xmlElement>::iterator standard = Elements.begin(); standard != Elements.end(); ++standard)
	{
		std::string* currentID = &standard->useID().useValue();

		std::vector<std::string*> matches;
		matches.push_back(currentID);

		for (std::list<xmlElement>::iterator sample = Elements.begin(); sample != Elements.end(); ++sample)
		{
			if (*currentID == sample->useID().useValue() && currentID != &sample->useID().useValue())
			{
				matches.push_back(&sample->useID().useValue());
			}
		}

		if (matches.size() > 1)
		{
			for (size_t i = 0; i < matches.size(); i++)
			{
				*matches[i] += ("_" + std::to_string(i + 1));
			}
		}
	}
}

xmlElement* xmlFile::findByID(const std::string& id)
{
	for (std::list<xmlElement>::iterator element = Elements.begin(); element != Elements.end(); ++element)
	{
		if (element->getID().getValue() == id) return &*element;
	}
	return nullptr;
}

std::list<xmlElement>::iterator xmlFile::findPositionByID(const std::string& id)
{
	for (std::list<xmlElement>::iterator element = Elements.begin(); element != Elements.end(); ++element)
	{
		if (element->getID().getValue() == id) return element;
	}
	return Elements.end();
}

void xmlFile::createElement(const std::string tag) // Creates new element at end of the list
{
	if (tag == "Not tag") return;

	xmlElement toAdd;
	std::stringstream ss(tag);
	toAdd.setKey(ss);
	toAdd.createAttributes(tag);

	Elements.push_back(toAdd);
}

void xmlFile::insertElement(const std::list<xmlElement>::iterator position, const std::string tag) // Inserts new element at list position
{
	xmlElement toAdd;
	std::stringstream ss(tag);
	toAdd.setKey(ss);
	toAdd.createAttributes(tag);

	Elements.insert(position, toAdd);
}

const std::string  xmlFile::getPath()
{
	return path;
}

void xmlFile::open() // Loads data from file
{
	std::cin >> path;

	if (isXmlFile(path))
	{
		std::ifstream xml_file(path.data());

		// CREATE NEW FILE IF NONEXISTENT
		if (!xml_file)
		{
			std::ofstream new_file(path.data());
			new_file.close();

			createElement("root");
			root = &Elements.back();
			openedTags.push_back(Elements.size() - 1);

			std::cout << "Successfully created " << path << '\n';
			return;
		}

		std::cout << "Opening...\n";

		// READS XML VERSION IF GIVEN
		getline(xml_file, xmlVersion);
		if (xmlVersion.substr(0, 5) != "<?xml")
		{
			xmlVersion = "<?xml version=\"1.0\"?>";
			xml_file.seekg(0);
		}

		// READS DATA \\

		std::string currentInput(parseTag(xml_file));
		if (currentInput == "Not tag")
		{
			std::cout << "Invalid content\n";
			return;
		}

		createElement(currentInput);
		root = &Elements.back();

		if (root == nullptr) return;

		// IF ROOT IS EMPTY TAG
		if (isEmptyTag(currentInput)) return;

		openedTags.push_back(Elements.size() - 1);

		while (!xml_file.eof() && !openedTags.empty())
		{
			currentInput = parseTag(xml_file);

			// READS TEXT CONTENT OF TEXT TAG
			if (currentInput == "Not tag")
			{
				char c;
				if (!xml_file.get(c)) return; // IF ROOT HAS NO CLOSING TAG

				xml_file.unget();
				listNth(openedTags.back()).setContent(parseTextTagContent(xml_file));
			}
			// READS CLOSING TAG
			else if (isClosingTag(currentInput))
			{
				parseClosingTag(currentInput);

				if (currentInput == listNth(openedTags.back()).getKey())
					openedTags.pop_back();
				else
				{
					std::cout << "INVALID CLOSING TAG";
					abort();
				}
			}
			// READS EMPTY TAG SYNTAX
			else if (isEmptyTag(currentInput))
			{
				createElement(currentInput);
				listNth(openedTags.back()).addChild(Elements.back());
			}
			// READS CHILD TAG
			else
			{
				createElement(currentInput);
				listNth(openedTags.back()).addChild(Elements.back());
				openedTags.push_back(Elements.size() - 1);
			}
		}

		std::cout << "Successfully opened " << path << '\n';

		xml_file.close();

		uniquifyID();
	}
	else std::cout << "Invalid file\n";
}

void xmlFile::save() // Writes data to original file
{
	std::ofstream outputFile(path.data());
	outputFile << xmlVersion << '\n' << *root;
	outputFile.close();
}

void xmlFile::saveas() // Writes data to file
{
	std::cin >> path;
	save();
}

void xmlFile::clear() // Resets loaded information
{
	path = "";
	xmlVersion = "<?xml version=\"1.0\"?>";
	xmlElement::resetAutoID();
	root = nullptr;
	Elements.clear();
	openedTags.clear();
}


void xmlFile::print() // Prints currently loaded XML data
{
	if (root != nullptr)
	{
		std::cout << xmlVersion << '\n';
		std::cout << *root << '\n';
	}
	else std::cout << "EMPTY FILE\n\n";
}

void xmlFile::select(std::string& id, std::string& key) // Prints attribute value by given element id and attribute key
{
	xmlElement* element = findByID(id);
	if (element == nullptr)
	{
		std::cout << "ID not found\n";
		return;
	}
	
	if (key == "id")
	{
		std::cout << id << '\n';
		return;
	}
	
	Attribute* attribute = element->findByKey(key);
	if (attribute == nullptr)
	{
		std::cout << "Attribute key not found\n";
		return;
	}

	std::cout << attribute->getValue() << '\n';
}

void xmlFile::set(std::string& id, std::string& key, std::string& value) // Sets attribute value by given element id and attribute key
{
	xmlElement* element = findByID(id);
	if (element == nullptr)
	{
		std::cout << "ID not found\n";
		return;
	}

	if (key == "id")
	{
		element->useID().setValue(value);
		uniquifyID();
		return;
	}

	Attribute* attribute = element->findByKey(key);
	if (attribute == nullptr)
	{
		std::cout << "Attribute key not found\n";
		return;
	}

	attribute->setValue(value);
	std::cout << "Successfully changed attribute value\n";
}

void xmlFile::settext(std::string& id, std::string& text)
{
	xmlElement* element = findByID(id);
	if (element == nullptr)
	{
		std::cout << "ID not found\n";
		return;
	}

	element->setContent(text);
	std::cout << "Successfully changed text content\n";
}

void xmlFile::children(std::string& id) // Prints all attributes belonging to children of element identified by id
{
	xmlElement* element = findByID(id);
	if (element == nullptr)
	{
		std::cout << "ID not found\n";
		return;
	}

	for (size_t i = 0; i < element->getChildren().size(); i++)
	{
		std::cout << element->getChildren()[i]->getKey() << ':' << element->getChildren()[i]->getID();
		for (size_t j = 0; j < element->getChildren()[i]->getAttributes().size(); j++)
		{
			std::cout << ", " << element->getChildren()[i]->getAttributes()[j].getKey();
		}
		std::cout << '\n';
	}
}

void xmlFile::child(std::string& id, size_t n)  // Prints nth child of element identified by id
{
	xmlElement* element = findByID(id);
	if (element == nullptr)
	{
		std::cout << "ID not found\n";
		return;
	}

	if (n < 1 || n > element->getChildren().size())
	{
		std::cout << "ID has no such child\n";
		return;
	}

	std::cout << *element->getChildren()[n-1] << '\n';
}

void xmlFile::text(std::string& id) // Prints text content of element identified by id
{
	xmlElement* element = findByID(id);
	if (element == nullptr)
	{
		std::cout << "ID not found\n";
		return;
	}

	if (element->getType() == "text")
	{
		std::cout << element->getContent() << '\n';
	}
	else
	{
		std::cout << '\n';
	}
}

void xmlFile::deleteattribute(std::string& id, std::string& key) // Removes attribute by given element id and attribute key
{
	xmlElement* element = findByID(id);
	if (element == nullptr)
	{
		std::cout << "ID not found\n";
		return;
	}

	if (key == "id")
	{
		std::cout << "Cannot remove ID\n";
		return;
	}

	Attribute* attribute = element->findByKey(key);
	if (attribute == nullptr)
	{
		std::cout << "Attribute key not found\n";
		return;
	}

	if (attribute != &element->useAttributes().back())
	{
		std::swap(*attribute, element->useAttributes().back());
	}
	
	element->useAttributes().pop_back();
	std::cout << "Successfully removed attribute\n";
}

void xmlFile::newattribute(std::string& id, std::string& key, std::string& value) // Creates new attribute by given element id and attribute key
{
	xmlElement* element = findByID(id);
	if (element == nullptr)
	{
		std::cout << "ID not found\n";
		return;
	}

	if (key == "id")
	{
		std::cout << "Attribute already exists, try:\nset <id> <key> <value>\n";
		return;
	}

	Attribute* attribute = element->findByKey(key);
	if (attribute != nullptr)
	{
		std::cout << "Attribute already exists, try:\nset <id> <key> <value>\n";
		return;
	}

	Attribute toAdd;
	toAdd.setKey(key);
	toAdd.setValue(value);

	element->addAttribute(toAdd);

	std::cout << "Successfully added attribute\n";
}

void xmlFile::newchild(std::string& id) // Creates new child of element identified by id
{
	std::list<xmlElement>::iterator parentPosition = findPositionByID(id);
	if (parentPosition == Elements.end())
	{
		std::cout << "ID not found\n";
		return;
	}

	std::list<xmlElement>::iterator childPosition = ++parentPosition;
	--parentPosition;

	for (size_t i = 0; i < parentPosition->getChildren().size(); i++) // SET LIST POSITION AFTER LAST CHILD
	{
		++childPosition;
	}

	insertElement(childPosition, "child");
	--childPosition;

	parentPosition->addChild(*(childPosition));

	std::cout << "Successfully added child\n";
}

void recursion(std::vector<XPathNode>& nodes, size_t currentNodeIndex, xmlElement* currentNode)
{
	if (!nodes.empty() && nodes[currentNodeIndex].getElement() == currentNode->getKey() 
		&& currentNode->meetsCondition(nodes[currentNodeIndex].getCondition()))
	{
		currentNodeIndex++;
		if (currentNodeIndex < nodes.size())
		{
			for (size_t currChild = 0; currChild < currentNode->getChildren().size(); currChild++)
			{
				if (currentNodeIndex == (nodes.size() - 1) && currentNode->getChildren()[currChild]->getKey() == nodes[nodes.size() - 1].getElement())
				{
					if (currentNode->getChildren()[currChild]->getType() == "text")
					{
						std::cout << currentNode->getChildren()[currChild]->getContent() << '\n';
					}
					else
					{
						std::cout << *currentNode->getChildren()[currChild];
					}
					return;
				}

				recursion(nodes, currentNodeIndex, currentNode->getChildren()[currChild]);
			}
		}
	}
}

void xmlFile::xpath(/*std::string& id, */std::string& xpath) // Recognises few basic XPath2.0 expressions, expects mostly ideal input
// Currently supports '/', creating an absolute path to an element, last node must be a text element
{
	// I have chosen to make my code work as "proper" XPath support, even though it is at odds with the given task and its examples
	// And the given examples for this task are incorrect
	// It says "person/address[0]" should give the first person's address
	// In reality that should be "/people/person[1]/address"
	// The given example would actually select nothing, because counting starts from 1
	// Even if it were correctly "/people/person/address[1]" it would actually give the first address of each person
	// Also the () brackets in XPath syntax have a meaning different from the way they are used in the examples, they should be []

	std::vector<XPathNode> nodes = parseXPath(xpath);

	recursion(nodes, 0, root);
}
