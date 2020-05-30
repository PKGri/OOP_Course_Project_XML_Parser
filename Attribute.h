#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <string>

class Attribute
{
private:
	std::string key;
	std::string value;
public:
	Attribute();
	Attribute(const size_t);

	void setKey(std::string s);
	void setValue(std::string s);

	std::string& useValue();

	const std::string& getKey() const;
	const std::string& getValue() const;

	bool operator == (const std::string&);
};

#endif ATTRIBUTE_H
