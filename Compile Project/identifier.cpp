#include "identifier.h"
#include <cassert>
#include <iostream>

// Identifier
identifier::identifier()
{
	this->name = "";
	this->kind = ILLEGAL_KIND;
	this->type = ILLEGAL_TYPE;
	this->value = -1;
	this->location = ERROR_LOCATION;
	this->have_set_value = false;
}

identifier::identifier(const identifier* _id) {
	this->name = string(_id->name);
	this->kind = _id->kind;
	this->type = _id->type;
	this->location = _id->location;
	this->array_lenth = _id->array_lenth;
	this->value = _id->value;
	this->have_set_value = _id->have_set_value;
	this->offset = _id->offset;
}

identifier::identifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type, int _value, IDENTIFIER_LOCATION _loc)
{
	this->name = _name;
	this->kind = _kind;
	this->type = _type;
	this->value = _value;
	this->location = _loc;
	this->have_set_value = false;
}

void identifier::setOffset(int _offset) {
	this->offset = _offset;
}

void identifier::setValue(int _value) {
	if (!this->have_set_value) {
		this->have_set_value = true;
		this->value = _value;
	} else {
		cout << "[DEBUG] try to assign to const (which have already set value)" << endl;
	}
}

int identifier::getValue() {
	return this->value;
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