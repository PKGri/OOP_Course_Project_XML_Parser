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

std::string parseTag(std::ifstream& xml_file) // Reads tag
{
	std::streampos begin(xml_file.tellg());

	char c;
	xml_file >> c;
	if (c != '<') // If text content
	{
		xml_file.seekg(begin);
		return "Not tag";
	}

	std::string tag;
	while (xml_file.get(c) && c != '>') // Gets all characters until tag is closed
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
	std::regex closingTag("/[^ \n\t]+[ \n\t]*"); // Regular expression for closing tag

	return regex_match(tag, closingTag);
}

void parseClosingTag(std::string& tag) // Removes '/' from closing tag and all whitespace characters after key
{
	std::stringstream ss(tag);
	char c;
	ss.get(c); // Removes '/'
	ss >> tag; //Removes whitespace characters after key
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
	
	xml_file.unget(); // Ungets '<' so it can be read again to open a tag

	return text;
}

std::vector<XPathNode> parseXPath(const std::string& xpath) // Creates vector of nodes for easier traversal one by one later
{
	std::regex node("/[^ \t&<>/\\[\\]]+[^&<>/]*"); // Regular expression for node and predicate

	std::sregex_iterator currentNodeMatch(xpath.begin(), xpath.end(), node);
	std::sregex_iterator lastMatch;

	std::vector<XPathNode> nodes;

	while (currentNodeMatch != lastMatch) // Finds all nodes and corresponding predicates in xpath expression
	{
		std::smatch nodeMatch = *currentNodeMatch;

		std::string node(nodeMatch.str().substr(1)); // Removes '/' from front

		XPathNode toAdd;

		toAdd.parseNode(node); // Separates element from predicate

		nodes.push_back(toAdd);

		currentNodeMatch++;
	}

	return nodes;
}


//**********************************************************************************************************\\
|-------------------------------------------> XMLFILE CLASS <------------------------------------------------|

xmlFile::xmlFile() : path(), xmlVersion("<?xml version=\"1.0\"?>"), root(nullptr), Elements(), openedTags() {}

xmlElement& xmlFile::listNth(const size_t n) // Returns Nth element in Elements
{
	std::list<xmlElement>::iterator element = Elements.begin();
	std::advance(element, n);
	return *element;
}

void xmlFile::uniquifyID() // Name says it all, makes all XML element IDs in file unique
{
	for (std::list<xmlElement>::iterator currentElement = Elements.begin(); currentElement != Elements.end(); ++currentElement) // Cycles through all elements
	{
		std::string* currentID = &currentElement->useID().useValue(); // Current element ID value

		std::vector<std::string*> matches; // Vector containing all IDs with the same value as current
		matches.push_back(currentID); // First ID is current

		for (std::list<xmlElement>::iterator comparison = Elements.begin(); comparison != Elements.end(); ++comparison) // Cycles through all elements to compare them with current
		{
			if (*currentID == comparison->useID().useValue() && currentID != &comparison->useID().useValue()) // Populates vector of matches with all matches as the cycle runs
			{
				matches.push_back(&comparison->useID().useValue());
			}
		}

		if (matches.size() > 1) // If there are any matches with current
		{
			for (size_t currentMatch = 0; currentMatch < matches.size(); currentMatch++) // Concatenates all with a different number
			{
				*matches[currentMatch] += ("_" + std::to_string(currentMatch + 1));
			}
		}
	}
}

xmlElement* xmlFile::findByID(const std::string& id) // Finds element by id
{
	for (std::list<xmlElement>::iterator element = Elements.begin(); element != Elements.end(); ++element)
	{
		if (element->getID().getValue() == id) return &*element;
	}
	return nullptr;
}

std::list<xmlElement>::iterator xmlFile::findPositionByID(const std::string& id) // Finds element position in Elements by id
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

const std::string& xmlFile::getPath() // Returns file path
{
	return path;
}

void xmlFile::open() // Loads data from file
{
	std::cin >> path; // File path

	if (isXmlFile(path))
	{
		std::ifstream xml_file(path.data());

		if (!xml_file) // Creates new file with given path if nonexistent
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

		// Reads XML version if given in file beginning
		getline(xml_file, xmlVersion);
		if (xmlVersion.substr(0, 5) != "<?xml")
		{
			xmlVersion = "<?xml version=\"1.0\"?>";
			xml_file.seekg(0);
		}

		// READS DATA \\

		std::string currentInput(parseTag(xml_file)); // Checks if file starts with opening tag
		if (currentInput == "Not tag")
		{
			std::cout << "Invalid content\n";
			return;
		}

		createElement(currentInput); // Creates root element
		root = &Elements.back();

		if (root == nullptr) return;

		if (isEmptyTag(currentInput)) return; // If root is empty tag

		openedTags.push_back(Elements.size() - 1); // Saves last opened tag at end of vector

		while (!xml_file.eof() && !openedTags.empty()) // Reads until end of file or until root tag is closed
		{
			currentInput = parseTag(xml_file); // Reads tag

			if (currentInput == "Not tag") // Reads text content if text tag is opened
			{
				char c;
				if (!xml_file.get(c)) return; // In case root has no closing tag
				xml_file.unget();

				listNth(openedTags.back()).setContent(parseTextTagContent(xml_file)); // Sets text content of last opened tag
			}
			else if (isClosingTag(currentInput)) // Reads closing tag
			{
				parseClosingTag(currentInput);

				if (currentInput == listNth(openedTags.back()).getKey()) // Closes last opened tag
					openedTags.pop_back();
				else
				{
					std::cout << "INVALID CLOSING TAG";
					abort();
				}
			}
			else if (isEmptyTag(currentInput)) // Reads empty tag formatted as <tag/>
			{
				createElement(currentInput);
				listNth(openedTags.back()).addChild(Elements.back());
			}
			else // Reads child tag of last opened tag
			{
				createElement(currentInput);
				listNth(openedTags.back()).addChild(Elements.back());
				openedTags.push_back(Elements.size() - 1);
			}
		}

		std::cout << "Successfully opened " << path << '\n';

		xml_file.close();

		uniquifyID(); // Makes all IDs unique
	}
	else std::cout << "Invalid file\n";
}

void xmlFile::save() // Writes data to original file path
{
	std::ofstream outputFile(path.data());
	outputFile << xmlVersion << '\n' << *root;
	outputFile.close();
}

void xmlFile::saveas() // Writes data to newly specified file path
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
	xmlElement* element = findByID(id); // Finds element by id

	if (element == nullptr) // If no such element exists
	{
		std::cout << "ID not found\n";
		return;
	}
	if (key == "id") // If chosen attribute is id
	{
		std::cout << id << '\n';
		return;
	}
	
	Attribute* attribute = element->findByKey(key); // Finds attribute by key
	if (attribute == nullptr)// If no such attribute exists
	{
		std::cout << "Attribute key not found\n";
		return;
	}

	std::cout << attribute->getValue() << '\n';
}

void xmlFile::set(std::string& id, std::string& key, std::string& value) // Sets attribute value by given element id and attribute key
{
	xmlElement* element = findByID(id); // Finds element by id

	if (element == nullptr) // If no such element exists
	{
		std::cout << "ID not found\n";
		return;
	}
	if (key == "id") // If chosen attribute is id
	{
		element->useID().setValue(value);
		uniquifyID();
		return;
	}

	Attribute* attribute = element->findByKey(key);
	if (attribute == nullptr) // If no such attribute exists
	{
		std::cout << "Attribute key not found\n";
		return;
	}

	attribute->setValue(value);
	std::cout << "Successfully changed attribute value\n";
}

void xmlFile::settext(std::string& id, std::string& text) // Sets element text content
{
	xmlElement* element = findByID(id); // Finds element by id

	if (element == nullptr) // If no such element exists
	{
		std::cout << "ID not found\n";
		return;
	}
	
	if (element->getType() == "text" || element->getType() == "empty") // If element is not parent tag
	{
		element->setContent(text);
		std::cout << "Successfully changed text content\n";
	}
	else
	{
		std::cout << "Element is parent tag and cannot have text content\n";
	}
}

void xmlFile::children(std::string& id) // Prints all attributes belonging to children of element identified by id
{
	xmlElement* element = findByID(id); // Finds element by id

	if (element == nullptr) // If no such element exists
	{
		std::cout << "ID not found\n";
		return;
	}

	for (size_t currentChild = 0; currentChild < element->getChildren().size(); currentChild++) // Cycles through all children
	{
		std::cout << element->getChildren()[currentChild]->getKey() << ':' << element->getChildren()[currentChild]->getID(); // Prints child key and id
		for (size_t currentAttribute = 0; currentAttribute < element->getChildren()[currentChild]->getAttributes().size(); currentAttribute++) // Prints all other attributes of child
		{
			std::cout << ", " << element->getChildren()[currentChild]->getAttributes()[currentAttribute].getKey();
		}
		std::cout << '\n';
	}
}

void xmlFile::child(std::string& id, size_t n)  // Prints nth child of element identified by id
{
	xmlElement* element = findByID(id); // Finds element by id

	if (element == nullptr) // If no such element exists
	{
		std::cout << "ID not found\n";
		return;
	}

	if (n < 1 || n > element->getChildren().size()) // If n is out of range
	{
		std::cout << "ID has no such child\n";
		return;
	}

	std::cout << *element->getChildren()[n-1] << '\n';
}

void xmlFile::text(std::string& id) // Prints text content of element identified by id
{
	xmlElement* element = findByID(id); // Finds element by id

	if (element == nullptr) // If no such element exists
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
	xmlElement* element = findByID(id); // Finds element by id

	if (element == nullptr) // If no such element exists
	{
		std::cout << "ID not found\n";
		return;
	}
	if (key == "id") // If chosen attribute is id
	{
		std::cout << "Cannot remove ID\n";
		return;
	}

	Attribute* attribute = element->findByKey(key); // Finds attribute by key

	if (attribute == nullptr) // If no such attribute exists
	{
		std::cout << "Attribute key not found\n";
		return;
	}

	if (attribute != &element->useAttributes().back()) // Swaps attribute with last in vector in order to remove using pop_back()
	{
		std::swap(*attribute, element->useAttributes().back());
	}
	
	element->useAttributes().pop_back();
	std::cout << "Successfully removed attribute\n";
}

void xmlFile::newattribute(std::string& id, std::string& key, std::string& value) // Creates new attribute by given element id and attribute key
{
	xmlElement* element = findByID(id); // Finds element by id

	if (element == nullptr) // If no such element exists
	{
		std::cout << "ID not found\n";
		return;
	}
	if (key == "id") // If chosen attribute is id
	{
		std::cout << "Attribute already exists, try:\nset <id> <key> <value>\n";
		return;
	}

	Attribute* attribute = element->findByKey(key); // Finds attribute by key

	if (attribute != nullptr) // If no such attribute exists
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
	std::list<xmlElement>::iterator parentPosition = findPositionByID(id); // Finds element position by id

	if (parentPosition == Elements.end()) // If no such element exists
	{
		std::cout << "ID not found\n";
		return;
	}

	std::list<xmlElement>::iterator childPosition = ++parentPosition;
	--parentPosition; // Returns iterator to parent position

	for (size_t i = 0; i < parentPosition->getChildren().size(); i++) // Sets position in list after last child
	{
		++childPosition;
	}

	insertElement(childPosition, "child");
	--childPosition; // Returns iterator to newly added child

	parentPosition->addChild(*(childPosition));

	std::cout << "Successfully added child\n";
}

void traverseXPath(std::vector<XPathNode>& nodes, size_t currentNodeIndex, xmlElement* currentElementNode) // Recursive function to execute XPath expression
{
	if (!nodes.empty() && nodes[currentNodeIndex].getElement() == currentElementNode->getKey() // If there are nodes and the current nodename matches the current element key
		&& currentElementNode->fulfilsPredicate(nodes[currentNodeIndex].getPredicate())) // and checks if element fulfils predicate, if any
	{
		currentNodeIndex++;
		if (currentNodeIndex < nodes.size())
		{
			for (size_t currentChild = 0; currentChild < currentElementNode->getChildren().size(); currentChild++) // Cycles through all children
			{
				if (currentNodeIndex == (nodes.size() - 1) && currentElementNode->getChildren()[currentChild]->getKey() == nodes[nodes.size() - 1].getElement()) // If last node and if nodename matches element key
				{
					if (currentElementNode->getChildren()[currentChild]->getType() == "text")
					{
						std::cout << currentElementNode->getChildren()[currentChild]->getContent() << '\n';
						return;
					}
					else
					{
						std::cout << *currentElementNode->getChildren()[currentChild];
					}
				}

				traverseXPath(nodes, currentNodeIndex, currentElementNode->getChildren()[currentChild]); // Recursive calling
			}
		}
	}
}

void xmlFile::xpath(std::string& xpath) // Recognises few basic XPath2.0 expressions, expects mostly ideal input, can handle some falsely formatted input as correct
/*
Currently supports: 
	/ - creating an absolute path to an element
	[] - various predicates:
	parent/nodename[(positive integer n)] - selects the nth child with name "nodename" of parent
	nodename[(textChild=textContent)] - selects the elements with name "nodename" that have a child named "textChild" with content "textContent"
	nodename[@attribute] - selects the elements with name "nodename" that have an attribute with key "attribute"
*/
{
	// I have chosen to make my code work as "proper" XPath support, even though it is at odds with the given task and its examples
	// The given examples for this task are incorrect XPath expressions
	// It says "person/address[0]" should give the first person's address
	// In reality that should be "/people/person[1]/address"
	// The given example would actually select nothing, because counting starts from 1
	// Even if it were correctly "/people/person/address[1]" it would actually give the first address of each person
	// Also the () brackets in XPath syntax have a meaning different from the way they are used in the examples, they should be []

	std::vector<XPathNode> nodes = parseXPath(xpath);

	traverseXPath(nodes, 0, root);
}
