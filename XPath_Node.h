#ifndef XPATH_NODE_H
#define XPATH_NODE_H

#include <string>

class XPathNode
{
private:
	std::string element;
	std::string predicate;
public:
	XPathNode(): element(), predicate() {}

	const std::string& getElement() const;
	const std::string& getPredicate() const;

	void setElement(std::string&);
	void setPredicate(std::string&);

	void parseNode(std::string&);
};

inline std::ostream& operator << (std::ostream& os, XPathNode node)
{
	os << node.getElement() << node.getPredicate();
	
	return os;
}

#endif XPATH_NODE_H