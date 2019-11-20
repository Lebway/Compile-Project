#include "symbolTable.h"
#include "identifier.h"

// Identifier
identifier::identifier()
{
	this->name = "";
	this->kind = ILLEGAL_KIND;
	this->type = ILLEGAL_TYPE;
}

identifier::identifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type)
{
	this->name = _name;
	this->kind = _kind;
	this->type = _type;
}

identifier::identifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type, int _value)
{
	this->name = _name;
	this->kind = _kind;
	this->type = _type;
	this->value = _value;
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

void identifierTable::addIdentifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type)
{
	// TODO: 重名元素会忽略!!!
	this->identifierMap.insert(pair<string, identifier>(_name, identifier(_name,_kind,_type)));
}

identifier* identifierTable::findIdentifier(string _name) {
	map<string, identifier>::iterator iter;
	iter = identifierMap.find(_name);
	if (iter == identifierMap.end()) {
		return new identifier();
	} else {
		return & iter->second;
	}
}

// Function

func::func() {
	this->name = "";
	this->status = NON_RETURN;
	this->type = ILLEGAL_FUNC;
	this->paramNum = 0;
	this->tempIdentifier = 0;
}

func::func(string _name, FUNC_TYPE _type) {
	this->name = _name;
	this->type = _type;
	this->status = NON_RETURN;
	this->paramNum = 0;
	this->tempIdentifier = 0;
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

void func::addIdentifier(identifier _identifier) {
	this->identifierTable.addIdentifier(_identifier);
}

void func::addIdentifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type) {
	this->identifierTable.addIdentifier(_name, _kind, _type);
}

identifier* func::findIdentifier(string _name) {
	return this->identifierTable.findIdentifier(_name);
}

identifier* func::genTempConst(IDENTIFIER_TYPE _type, const int _value) {
	string name = to_string(_value);
	if (this->identifierTable.findIdentifier(name)->kind == ILLEGAL_KIND) {
		this->identifierTable.addIdentifier(
			identifier(name, CONST_IDENTIFIER, _type, _value));
	}
	return this->identifierTable.findIdentifier(name);
}

identifier* func::genTempVar(IDENTIFIER_TYPE _type) {
	static int counter = 1;
	this->tempIdentifier = counter;
	return new identifier("#" + to_string(counter++), VAR_IDENTIFIER, _type);
}

// Function Table

SymbolTable::SymbolTable() {

}

void SymbolTable::addFunc(func _func) {
	// TODO: 同名函数会被忽略!!!
	funcTable.push_back( _func);
}

void SymbolTable::addFunc(string _name, FUNC_TYPE _type) {
	funcTable.push_back(func(_name, _type));
}

void SymbolTable::setFuncStatus(string _name, FUNC_STATUS _status) {
	list<func>::iterator iter;
	for (iter = this->funcTable.begin(); iter != this->funcTable.end(); iter++) {
		if ((*iter).name == _name) (*iter).setStatus(_status);
	}
}

func* SymbolTable::findFunc(string _name) {
	list<func>::iterator iter;
	for (iter = this->funcTable.begin(); iter != this->funcTable.end(); iter++) {
		if ((*iter).name == _name) return &(*iter);
	}
	return new func();
}

void SymbolTable::midCode_output(ofstream& output_handler)
{
	list<func>::iterator func_iter;
	list<midCode>::iterator midCode_iter;
	for (func_iter = this->funcTable.begin(); func_iter != this->funcTable.end(); func_iter++) {
		for (midCode_iter = (*func_iter).midCodeList.begin(); midCode_iter != (*func_iter).midCodeList.end(); midCode_iter++) {
			(*midCode_iter).output(output_handler);
			output_handler << endl;
		}
		output_handler << endl;
	}
}
