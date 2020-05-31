#include <sstream>
#include <regex>

#include "XML_Element.h"

size_t xmlElement::autoID = 0; // Static variable, generates unique IDs for elements

void xmlElement::resetAutoID() // Static function, resets ID generator
{
	autoID = 0;
}


xmlElement::xmlElement() : ancestors(0), key(), id(++autoID), attributes(), parent(nullptr), content(), children() {}


const bool xmlElement::contains(const Attribute& attribute) const // Checks if element contains attribute
{
	for (size_t i = 0; i < attributes.size(); i++)
	{
		if (attributes[i].getKey() == attribute.getKey()) return true;
	}
	return false;
}

const bool xmlElement::contains(const std::string& attributeKey) const // Checks if element contains attribute by given key
{
	for (size_t i = 0; i < attributes.size(); i++)
	{
		if (attributes[i].getKey() == attributeKey) return true;
	}
	return false;
}


Attribute* xmlElement::findByKey(const std::string& key) // Finds attribute by key
{
	for (size_t i = 0; i < attributes.size(); i++)
	{
		if (attributes[i].getKey() == key) return &attributes[i];
	}
	return nullptr;
}


void xmlElement::createAttributes(const std::string& tag) // Parses tag attributes
{
	std::regex keyRegex("[^ \n\t&<>]+[ \n\t]*="); // Regular expression for attribute key
	std::regex valueRegex("\"[^&<>\"]+\""); // Regular expression for attribute value

	std::sregex_iterator currentKeyMatch(tag.begin(), tag.end(), keyRegex);
	std::sregex_iterator currentValueMatch(tag.begin(), tag.end(), valueRegex);

	std::sregex_iterator lastMatch;

	while (currentKeyMatch != lastMatch && currentValueMatch != lastMatch) // Separates all attributes and adds them to element attribute vector
	{
		Attribute toAdd;

		std::smatch kmatch = *currentKeyMatch;
		std::smatch vmatch = *currentValueMatch;

		std::string key(kmatch.str());
		std::string value(vmatch.str());

		key.pop_back(); // Removes '=' from key end
		std::stringstream ss(key);
		ss >> key; // Removes whitespace characters from key string

		value.erase(value.begin()); // Removes \" from value beginning
		value.pop_back(); // Removes \" from value end

		toAdd.setKey(key);
		toAdd.setValue(value);

		if (key == "id") setID(value);
		else addAttribute(toAdd);

		currentKeyMatch++;
		currentValueMatch++;
	}
}

void xmlElement::setKey(std::istream& is)
{
	is >> key;
	if (key.back() == '/') key.pop_back();
}

void xmlElement::setID(std::string _id) 
{ 
	id.setValue(_id);
}

void xmlElement::setAncestors(size_t n) 
{ 
	ancestors = n; 
}

void xmlElement::setParent(xmlElement* _parent)
{
	parent = _parent;
}

void xmlElement::setKey(std::string _key) 
{ 
	key = _key;
	if (key.back() == '/') key.pop_back();
}

void xmlElement::setContent(std::string _content) 
{
	content = _content;
}

std::vector<Attribute>& xmlElement::useAttributes()
{
	return attributes;
}

std::vector<xmlElement*>& xmlElement::useChildren() 
{
	return children;
}

Attribute& xmlElement::useID()
{
	return id;
}

const size_t& xmlElement::getAncestors() const
{ 
	return ancestors;
}

const xmlElement* xmlElement::getParent() const
{
	return parent;
}

const std::string& xmlElement::getKey() const
{ 
	return key; 
}

const Attribute& xmlElement::getID() const
{
	return id;
}

const std::vector<Attribute>& xmlElement::getAttributes() const
{ 
	return attributes;
}

const std::string& xmlElement::getContent() const
{
	return content;
}

const std::vector<xmlElement*>& xmlElement::getChildren() const
{ 
	return children;
}

const char* xmlElement::getType() const // Says what type the element is
{
	if (!children.empty()) return "parent";
	if (!content.empty()) return "text";
	return "empty";
}


void xmlElement::addAttribute(Attribute attribute) // Adds attribute
{
	if (!contains(attribute))
		attributes.push_back(attribute);
}

void xmlElement::addChild(xmlElement& child) // Adds child
{
	child.setParent(this);
	child.setAncestors(getAncestors() + 1);
	children.push_back(&child);
}

bool isDigits(const std::string& str) // Checks if string is only digits | Function is taken from here: https://stackoverflow.com/questions/8888748/how-to-check-if-given-c-string-or-char-contains-only-digits
{
	return str.find_first_not_of("0123456789") == std::string::npos;
}

bool xmlElement::fulfilsPredicate(const std::string& predicate) // Checks if element fulfils given predicate
{
	if (predicate == "") // If no predicate
		return true;

	else if (std::regex_match(predicate, std::regex("@id[ \t]*"))) // If predicate is [@id]
		return true;

	else if (predicate[0] == '@') // [@attributeKey] predicate
		return contains(predicate.substr(1));

	else if (isDigits(predicate)) // [(positive integer)] predicate
	{
		if (parent == nullptr) // If root element
			return false;

		size_t number = std::stoul(predicate, nullptr, 0); // Converts predicate string into number
		if (number > 0 && number <= parent->children.size()) // If element has at least this many children
		{
			size_t matches = 0; // Number of children with the same name
			for (size_t currentChild = 0; currentChild < parent->children.size(); currentChild++) // Cycles through all children
			{
				if (parent->children[currentChild]->getKey() == key) matches++;
				if (matches == number && parent->children[currentChild] == this) return true; // Returns on nth child with given name
			}
		}
	}

	else if (predicate.find('=') != std::string::npos) // [textChild=textContent] predicate
	{
		std::stringstream ss(predicate);

		std::string childKey;
		std::string textContent;
		char c;

		while (ss.get(c) && c != '=')
		{
			childKey += c;
		}

		while (ss.get(c))
		{
			textContent += c;
		}

		for (size_t currentChild = 0; currentChild < children.size(); currentChild++) // Cycles through all children
		{
			if (children[currentChild]->getKey() == childKey && children[currentChild]->getContent() == textContent) // If child has text content as searched for
				return true;
		}
	}

	else if (predicate.find(' ') == std::string::npos && predicate.find('\t') == std::string::npos) // [childKey] predicate
	{
		for (size_t currChild = 0; currChild < children.size(); currChild++) // Cycles through all children
		{
			if (children[currChild]->getKey() == predicate) // If child key matches searched child key
				return true;
		}
	}

	return false;
}