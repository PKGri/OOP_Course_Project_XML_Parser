#ifndef XML_ELEMENTS_H
#define XML_ELEMENTS_H

#include <iostream>
#include <string>
#include <vector>

#include "Attribute.h"

class xmlElement
{
protected:
	static size_t autoID;

	size_t ancestors;
	std::string key;
	Attribute id;
	std::vector<Attribute> attributes;
	xmlElement* parent;

	std::string content;
	std::vector<xmlElement*> children;

	const bool contains(const Attribute&) const;
	const bool contains(const std::string&) const;

public:
	xmlElement();

	static void resetAutoID();

	Attribute* findByKey(const std::string&);

	void createAttributes(const std::string&);

	void setKey(std::istream& is);

	void setID(std::string s);
	void setAncestors(size_t n);
	void setParent(xmlElement*);
	void setKey(std::string s);
	void setContent(std::string s);

	std::vector<Attribute>& useAttributes();
	std::vector<xmlElement*>& useChildren();
	Attribute& useID();

	const size_t& getAncestors() const;
	const xmlElement* getParent() const;
	const std::string& getKey() const;
	const Attribute& getID() const;
	const std::vector<Attribute>& getAttributes() const;
	const std::string& getContent() const;
	const std::vector<xmlElement*>& getChildren() const;

	const char* getType() const;

	void addAttribute(Attribute);
	void addChild(xmlElement&);

	bool fulfilsPredicate(const std::string&);
};


inline std::ostream& operator << (std::ostream& os, const Attribute& attribute)
{
	os << ' ' << attribute.getKey() << "=\"" << attribute.getValue() << '"';
	return os;
}

inline std::ostream& operator << (std::ostream& os, const std::vector<Attribute>& attributes)
{
	for (size_t i = 0; i < attributes.size(); i++)
	{
		os << attributes[i];
	}

	return os;
}

inline std::ostream& operator << (std::ostream& os, const std::vector<xmlElement*>& children);

inline void nesting(std::ostream& os, const xmlElement& element)
{
	for (size_t i = 0; i < element.getAncestors(); i++)
	{
		os << "    ";
	}
}

inline std::ostream& operator << (std::ostream& os, const xmlElement& element)
{
	if (element.getType() == "empty")
	{
		nesting(os, element);
		os << '<' << element.getKey() << element.getID() << element.getAttributes() << "/>\n";
	}
	else if (element.getType() == "text")
	{
		nesting(os, element);
		os << '<' << element.getKey() << element.getID() << element.getAttributes() << '>'
			<< element.getContent() << "</" << element.getKey() << ">\n";
	}
	else if (element.getType() == "parent")
	{
		nesting(os, element);
		os << '<' << element.getKey() << element.getID() << element.getAttributes() << ">\n"
			<< element.getChildren();
		nesting(os, element);
		os << "</" << element.getKey() << ">\n";
	}

	return os;
}

inline std::ostream& operator << (std::ostream& os, const std::vector<xmlElement*>& children)
{
	for (size_t i = 0; i < children.size(); i++)
	{
		os << *children[i];
	}
	
	return os;
}

#endif XML_ELEMENTS_H
