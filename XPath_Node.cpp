#include <vector>
#include <regex>
#include <sstream>

#include "XPath_Node.h"


void XPathNode::parseNode(std::string& node) // Separates element from predicate
{
	std::regex elementReg("[^\n\\[\\]]+[\\[]?"); // Regular expression for element
	std::regex predicateReg("\\[[^&<>\\[\\]]+\\]"); // Regular expression for predicate

	std::sregex_iterator lastMatch;

	std::sregex_iterator elementMatch(node.begin(), node.end(), elementReg);

	std::smatch elementM = *elementMatch;
	std::string _element(elementM.str());
	std::stringstream ss(_element);
	ss >> _element; // Removes whitespace characters from element string

	if (_element.back() == '[') _element.pop_back(); // Removes '[' from element string

	element = _element;

	std::sregex_iterator predicateMatch(node.begin(), node.end(), predicateReg);

	while (predicateMatch != lastMatch)
	{
		std::smatch predicateM = *predicateMatch;
		std::string _predicate(predicateM.str());
		if (!_predicate.empty())
		{
			_predicate.erase(0, 1);
			_predicate.pop_back();
		}
		predicate = _predicate;
		predicateMatch++;
		break;
	}
}

const std::string& XPathNode::getElement() const
{
	return element;
}

const std::string& XPathNode::getPredicate() const
{
	return predicate;
}

void XPathNode::setElement(std::string& _element)
{
	element = _element;
}

void XPathNode::setCondition(std::string& _predicate)
{
	predicate = _predicate;
}