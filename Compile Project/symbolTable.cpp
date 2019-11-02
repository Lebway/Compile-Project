#include "symbolTable.h"

// Identifier
identifier::identifier()
{
	this->name = "";
	this->kind = ILLEGAL_KIND;
	this->type = ILLEGAL_TYPE;
}

identifier::identifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type, int _level)
{
	this->name = _name;
	this->kind = _kind;
	this->type = _type;
	this->level = _level;
}

// Identifier Table

identifierTable::identifierTable()
{

}

void identifierTable::addIdentifier(identifier _identifier)
{
	// TODO: 重名元素会忽略!!!
	this->identifierMap.insert(pair<string, identifier>(_identifier.name, _identifier));
}

void identifierTable::addIdentifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type, int _level)
{
	// TODO: 重名元素会忽略!!!
	this->identifierMap.insert(pair<string, identifier>(_name, identifier(_name,_kind,_type,_level)));
}

identifier identifierTable::findIdentifier(string _name) {
	map<string, identifier>::iterator iter;
	iter = identifierMap.find(_name);
	if (iter == identifierMap.end()) {
		return identifier();
	} else {
		return iter->second;
	}
}

void identifierTable::cleanLevel(int _level) {
	map<string, identifier>::iterator iter;
	for (iter = identifierMap.begin(); iter != identifierMap.end();) {
		if (((iter)->second).level >= _level) {
			identifierMap.erase(iter++);
		} else {
			iter++;
		}
	}
}

// Function

func::func() {
	this->name = "";
	this->status = NON_RETURN;
	this->type = ILLEGAL_FUNC;
	this->paramNum = 0;
}

func::func(string _name, FUNC_TYPE _type) {
	this->name = _name;
	this->type = _type;
	this->status = NON_RETURN;
	this->paramNum = 0;
}

void func::setStatus(FUNC_STATUS _status) {
	if (this->status == NON_RETURN && _status != NON_RETURN) {
		this->status = _status;
	} else {
		//	TODO: raise error: cannot return twice 
	}
}

void func::addParam(Symbol _symbol) {
	this->paramNum++;
	this->paramList.push_back(_symbol);
}

int func::getParamNum() {
	return this->paramNum;
}

list<Symbol> func::getParamList() {
	return this->paramList;
}

// Function Table

funcTable::funcTable() {

}

void funcTable::addFunc(func _func) {
	// TODO: 同名函数会被忽略!!!
	funcMap.insert(pair<string, func>(_func.name, _func));
}

void funcTable::addFunc(string _name, FUNC_TYPE _type) {
	funcMap.insert(pair<string, func>(_name, func(_name, _type)));
}

void funcTable::setFuncStatus(string _name, FUNC_STATUS _status) {
	map<string, func>::iterator iter;
	iter = funcMap.find(_name);
	if (iter != funcMap.end()) {
		iter->second.setStatus(_status);
	}
}

func* funcTable::findFunc(string _name) {
	map<string, func>::iterator iter;
	iter = funcMap.find(_name);
	if (iter == funcMap.end()) {
		return new func();
	} else {
		return &(iter->second);
	}
}
