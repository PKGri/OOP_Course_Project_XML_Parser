#ifndef XML_ELEMENTS_H
#define XML_ELEMENTS_H

#include <iostream>
#include <string>
#include <vector>

class Attribute
{
private:
	std::string key;
	std::string value;
public:
	Attribute();
	Attribute(const size_t);

	void setKey(std::istream& is) { is >> key; }
	void setValue(std::istream& is) { is >> value; }

	void setKey(std::string s) { key = s; }
	void setValue(std::string s) { value = s; }

	std::string& useValue() { return value; }

	const std::string getKey() const { return key; }
	const std::string getValue() const { return value; }

	bool operator == (const Attribute&);
	bool operator >= (const Attribute&);

	bool operator == (const std::string&);
};


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
	xmlElement(const size_t);

	static void resetAutoID();

	Attribute* findByKey(const std::string&);

	void createAttributes(const std::string);

	void setAncestors(std::istream& is);
	void setKey(std::istream& is);
	void setContent(std::istream& is);

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

	bool meetsCondition(std::string);
};


inline std::ostream& operator << (std::ostream& os, Attribute attribute)
{
	os << ' ' << attribute.getKey() << "=\"" << attribute.getValue() << '"';
	return os;
}

inline std::ostream& operator << (std::ostream& os, std::vector<Attribute> attributes)
{
	for (size_t i = 0; i < attributes.size(); i++)
	{
		os << attributes[i];
	}

	return os;
}

inline std::ostream& operator << (std::ostream& os, std::vector<xmlElement*> children);

inline void nesting(std::ostream& os, xmlElement element)
{
	for (size_t i = 0; i < element.getAncestors(); i++)
	{
		os << "    ";
	}
}

inline std::ostream& operator << (std::ostream& os, xmlElement element)
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

inline std::ostream& operator << (std::ostream& os, std::vector<xmlElement*> children)
{
	for (size_t i = 0; i < children.size(); i++)
	{
		os << *children[i];
	}
	
	return os;
}

#endif XML_ELEMENTS_H
