#include "Attribute.h"


Attribute::Attribute() : key(), value() {}

Attribute::Attribute(const size_t id) : key("id"), value(std::to_string(id)) {}


void Attribute::setKey(std::string _key)
{
	key = _key;
}

void Attribute::setValue(std::string _value)
{
	value = _value;
}

std::string& Attribute::useValue()
{
	return value;
}

const std::string& Attribute::getKey() const
{
	return key;
}

const std::string& Attribute::getValue() const
{
	return value;
}


bool Attribute::operator == (const std::string& _key) // Checks if attribute key matches given key
{
	return key == _key;
}

