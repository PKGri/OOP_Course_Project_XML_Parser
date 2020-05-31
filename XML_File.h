#ifndef XML_FILE_H
#define XML_FILE_H

#include <list>

#include "XPath_Node.h"
#include "XML_Element.h"

bool isXmlFile(const std::string&);
std::string parseTag(std::ifstream&);
bool isEmptyTag(std::string&);
bool isClosingTag(std::string&);
void parseClosingTag(std::string&);
std::string parseTextTagContent(std::ifstream&);
std::vector<XPathNode> parseXPath(const std::string&);

class xmlFile
{
private:
	std::string path;
	std::string xmlVersion;
	xmlElement* root;
	std::list<xmlElement> Elements;

	xmlElement& listNth(const size_t n);

	void uniquifyID();
	xmlElement* findByID(const std::string&);
	std::list<xmlElement>::iterator findPositionByID(const std::string&);

	void createElement(const std::string);
	void insertElement(const std::list<xmlElement>::iterator, const std::string);

public:
	xmlFile();

	const std::string& getPath();
	//const std::string getxmlVersion() { return xmlVersion; }
	//const xmlElement* getRoot() { return root; }
	//const std::list<xmlElement>& getElements() { return Elements; }
	//const std::vector<size_t>& getOpenedTags() { return openedTags; }
	//
	//std::string& usePath() { return path; }
	//std::string& usexmlVersion() { return xmlVersion; }
	//xmlElement* useRoot() { return root; }
	//
	//std::list<xmlElement>& useElements() { return Elements; }
	//std::vector<size_t>& useOpenedTags() { return openedTags; }

	void open();
	void save();
	void saveas();
	void clear();

	void print();
	void select(std::string&, std::string&);
	void set(std::string&, std::string&, std::string&);
	void settext(std::string&, std::string&);
	void children(std::string&);
	void child(std::string&, size_t);
	void text(std::string&);
	void deleteattribute(std::string&, std::string&);
	void newattribute(std::string&, std::string&, std::string&);
	void newchild(std::string&);
	void xpath(std::string&);
};

#endif XML_FILE_H