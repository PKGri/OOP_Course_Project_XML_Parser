#include "XPath_Node.h"

#include <vector>
#include <regex>
#include <sstream>

/*
std::vector<XPath> parseXPath(std::string& xpath)
{
	std::regex element("/[^ \t\n&<>/\\[\\]]+[ \t]*");

	std::sregex_iterator currentNodeMatch(xpath.begin(), xpath.end(), element);

	std::sregex_iterator lastMatch;

	std::vector<XPath> XPath;

	while (currentNodeMatch != lastMatch)
	{
		std::smatch match = *currentNodeMatch;

		std::string node(match.str().substr(1));

		std::stringstream ss(node);
		ss >> node;

		if (node != "")
			//XPath.push_back(node);

		currentNodeMatch++;
	}

	return XPath;
}
*/

void XPathNode::parseNode(std::string& node)
{
	std::regex elementReg("[^\n\\[\\]]+[\\[]?");
	std::regex conditionReg("\\[[^&<>\\[\\]]+\\]");

	std::sregex_iterator lastMatch;

	std::sregex_iterator elementMatch(node.begin(), node.end(), elementReg);

	std::smatch elementM = *elementMatch;
	std::string _element(elementM.str());
	std::stringstream s(_element);
	s >> _element;

	if (_element.back() == '[') _element.pop_back();

	element = _element;

	std::sregex_iterator conditionMatch(node.begin(), node.end(), conditionReg);

	while (conditionMatch != lastMatch)
	{
		std::smatch conditionM = *conditionMatch;
		std::string _condition(conditionM.str());
		if (!_condition.empty())
		{
			_condition.erase(0, 1);
			_condition.pop_back();
		}
		condition = _condition;
		conditionMatch++;
	}
}

const std::string& XPathNode::getElement() const
{
	return element;
}

const std::string& XPathNode::getCondition() const
{
	return condition;
}

void XPathNode::setElement(std::string& _element)
{
	element = _element;
}

void XPathNode::setCondition(std::string& _condition)
{
	condition = _condition;
}