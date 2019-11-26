#include "identifier.h"
#include <cassert>

// Identifier
identifier::identifier()
{
	this->name = "";
	this->kind = ILLEGAL_KIND;
	this->type = ILLEGAL_TYPE;
	this->value = -1;
	this->location = ERROR_LOCATION;
}

identifier::identifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type, int _value, IDENTIFIER_LOCATION _loc)
{
	this->name = _name;
	this->kind = _kind;
	this->type = _type;
	this->value = _value;
	this->location = _loc;
}

void identifier::setOffset(int _offset) {
	this->offset = _offset;
}

// Identifier Table

identifierTable::identifierTable() {}

identifier* identifierTable::addIdentifier(identifier _identifier)
{
	this->identifierList.push_back(&_identifier);
	// this->identifierMap.insert(pair<string, identifier*>(_identifier.name, &_identifier));
	return this->findIdentifier(_identifier.name);
}

identifier* identifierTable::addIdentifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type, 
	int _value, IDENTIFIER_LOCATION _loc)
{
	assert(_loc != ERROR_LOCATION);
	this->identifierList.push_back(new identifier(_name, _kind, _type, _value, _loc));
	// this->identifierMap.insert(pair<string, identifier*>(_name, new identifier(_name, _kind, _type, _value, _loc)));
	return this->findIdentifier(_name);
}

identifier* identifierTable::findIdentifier(string _name) {
	auto iter = identifierList.begin();
	for (; iter != identifierList.end(); iter++) {
		if ((*iter)->name == _name) return (*iter);
	}
	return new identifier();

	/*
	auto iter = identifierMap.find(_name);
	if (iter == identifierMap.end()) {
		return new identifier();
	} else {
		return iter->second;
	}
	*/
}