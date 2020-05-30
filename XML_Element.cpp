#include <sstream>
#include <regex>

#include "XML_Element.h"

//**********************************************************************************************************\\
|-----------------------------------------> ATTRIBUTE CLASS <------------------------------------------------|

size_t xmlElement::autoID = 0;

void xmlElement::resetAutoID()
{
	autoID = 0;
}

Attribute::Attribute() : key(), value() {}

Attribute::Attribute(const size_t id) : key("id"), value(std::to_string(id)) {}


bool Attribute::operator == (const Attribute& other)
{
	return key == other.key;
}

bool Attribute::operator >= (const Attribute& other)
{
	return key == other.key && value == other.value;
}

bool Attribute::operator == (const std::string& _key)
{
	return key == _key;
}




//**********************************************************************************************************\\
|-----------------------------------------> XMLELEMENT CLASS <------------------------------------------------|

xmlElement::xmlElement() : ancestors(0), key(), id(++autoID), attributes(), parent(nullptr), content(), children() {}

xmlElement::xmlElement(const size_t id) : ancestors(), key(), id(id), attributes(), parent(nullptr), content(), children() {}


const bool xmlElement::contains(const Attribute& attribute) const
{
	for (size_t i = 0; i < attributes.size(); i++)
	{
		if (attributes[i].getKey() == attribute.getKey()) return true;
	}
	return false;
}

const bool xmlElement::contains(const std::string& attributeKey) const
{
	for (size_t i = 0; i < attributes.size(); i++)
	{
		if (attributes[i].getKey() == attributeKey) return true;
	}
	return false;
}


Attribute* xmlElement::findByKey(const std::string& key)
{
	for (size_t i = 0; i < attributes.size(); i++)
	{
		if (attributes[i].getKey() == key) return &attributes[i];
	}
	return nullptr;
}


void xmlElement::createAttributes(const std::string tag) // Parses tag attributes
{
	std::regex keyRegex("[^ \n\t&<>]+[ \n\t]*=");
	std::regex valueRegex("\"[^&<>\"]+\"");

	std::sregex_iterator currentKeyMatch(tag.begin(), tag.end(), keyRegex);
	std::sregex_iterator currentValueMatch(tag.begin(), tag.end(), valueRegex);

	std::sregex_iterator lastMatch;


	while (currentKeyMatch != lastMatch && currentValueMatch != lastMatch)
	{
		Attribute toAdd;

		std::smatch kmatch = *currentKeyMatch;
		std::smatch vmatch = *currentValueMatch;

		std::string key(kmatch.str());
		std::string value(vmatch.str());

		key.pop_back();
		std::stringstream ss(key);
		ss >> key;

		value.erase(value.begin());
		value.pop_back();

		toAdd.setKey(key);
		toAdd.setValue(value);

		if (key == "id") setID(value);
		else addAttribute(toAdd);

		currentKeyMatch++;
		currentValueMatch++;
	}
}

void xmlElement::setAncestors(std::istream& is) 
{ 
	is >> ancestors; 
}

void xmlElement::setKey(std::istream& is)
{
	is >> key;
	if (key.back() == '/') key.pop_back();
}

void xmlElement::setContent(std::istream& is)
{
	is >> content;
}

void xmlElement::setID(std::string s) 
{ 
	id.setValue(s);
}

void xmlElement::setAncestors(size_t n) 
{ 
	ancestors = n; 
}

void xmlElement::setParent(xmlElement* _parent)
{
	parent = _parent;
}

void xmlElement::setKey(std::string s) 
{ 
	key = s; 
	if (key.back() == '/') key.pop_back();
}

void xmlElement::setContent(std::string s) 
{
	content = s;
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


const char* xmlElement::getType() const
{
	if (!children.empty()) return "parent";
	if (!content.empty()) return "text";
	return "empty";
}


void xmlElement::addAttribute(Attribute attribute)
{
	if (!contains(attribute))
		attributes.push_back(attribute);
}

void xmlElement::addChild(xmlElement& child)
{
	child.setParent(this);
	child.setAncestors(getAncestors() + 1);
	children.push_back(&child);
}

bool isDigits(const std::string& str) // Checks if string is only digits
{
	return str.find_first_not_of("0123456789") == std::string::npos;
}

bool xmlElement::meetsCondition(std::string condition)
{
	if (condition == "")
		return true;

	else if (std::regex_match(condition, std::regex("@id[ \t]*")))
		return true;

	else if (condition[0] == '@') 
		return contains(condition.substr(1));

	else if (isDigits(condition))
	{
		if (parent == nullptr)
			return false;

		size_t number = std::stoul(condition, nullptr, 0);
		if (number > 0 && number <= parent->children.size())
		{
			size_t matches = 0;
			for (size_t currChild = 0; currChild < parent->children.size(); currChild++)
			{
				if (parent->children[currChild]->getKey() == key) matches++;
				if (matches == number && parent->children[currChild] == this) return true;
			}
		}
	}

	else if (condition.find('=') != std::string::npos)
	{
		std::stringstream ss(condition);

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

		for (size_t currChild = 0; currChild < children.size(); currChild++)
		{
			if (children[currChild]->getKey() == childKey && children[currChild]->getContent() == textContent)
				return true;
		}
	}

	else if (condition.find(' ') == std::string::npos && condition.find('\t') == std::string::npos)
	{
		for (size_t currChild = 0; currChild < children.size(); currChild++)
		{
			if (children[currChild]->getKey() == condition)
				return true;
		}
	}

	return false;
}