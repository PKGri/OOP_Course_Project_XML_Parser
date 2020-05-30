#ifndef XPATH_H
#define XPATH_H

#include <string>

class XPathNode
{
private:
	std::string element;
	std::string condition;
public:
	XPathNode(): element(), condition() {}

	const std::string& getElement() const;
	const std::string& getCondition() const;

	void setElement(std::string&);
	void setCondition(std::string&);

	void parseNode(std::string&);

};

inline std::ostream& operator << (std::ostream& os, XPathNode node)
{
	os << node.getElement() << node.getCondition();
	
	return os;
}

#endif XPATH_H