#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <functional>

void open();
void save();
void saveas();
void help();

void print();
void select();
void set();
void settext();
void children();
void child();
void text();
void deleteattribute();
void newattribute();
void newchild();
void xpath();

bool mainCommandList(std::string);
bool openCommandList(std::string);
void executeCommand(std::string);
bool isExit(std::string);
bool isCloseOrExit(std::string);

void enterCommand(std::function<bool(std::string)> terminationPredicate, std::function<bool(std::string)> commandList);

#endif COMMANDS_H

