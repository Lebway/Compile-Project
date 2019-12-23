#include "symbolTable.h"
#include "identifier.h"
#include <cassert>

// Function

func::func() {
	this->name = "";
	this->status = NON_RETURN;
	this->type = ILLEGAL_FUNC;
	this->paramNum = 0;
	this->pushNum = 0;
	this->canInline = true;
}

func::func(string _name, FUNC_TYPE _type) {
	this->name = _name;
	this->type = _type;
	this->status = NON_RETURN;
	this->paramNum = 0;
	this->pushNum = 0;
	this->canInline = true;
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

identifier* func::addIdentifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type, 
	int _value, IDENTIFIER_LOCATION _loc) {
	assert(_loc != ERROR_LOCATION);
	return this->identifierTable.addIdentifier(_name, _kind, _type, _value, _loc);
}

identifier* func::findIdentifier(string _name) {
	return this->identifierTable.findIdentifier(_name);
}

identifier* func::genTempConst(IDENTIFIER_TYPE _type, const int _value, IDENTIFIER_LOCATION _loc) {
	string name = to_string(_value);
	if (this->identifierTable.findIdentifier(name)->kind == ILLEGAL_KIND) {
		this->identifierTable.addIdentifier(name, CONST_IDENTIFIER, _type, _value, _loc);
	}
	return this->identifierTable.findIdentifier(name);
}

identifier* func::genTempVar(IDENTIFIER_TYPE _type, IDENTIFIER_LOCATION _loc) {
	static int counter = 1;
	return this->addIdentifier("#"+to_string(counter++), VAR_IDENTIFIER, _type, 0, _loc);
}

int func::setOffset() {
	int offset = 4;
	for (auto identifier_iter = identifierTable.identifierList.begin();
		identifier_iter != identifierTable.identifierList.end(); identifier_iter++) {
		(*identifier_iter)->setOffset(offset);
		if ((*identifier_iter)->kind == ARRAY_IDENTIFIER) {
			offset += (*identifier_iter)->array_lenth * 4;
		} else {
			offset += 4;
		}
	}
	this->offsetSum = offset;
	return offset;
}

int func::getOffsetSum() {
	setOffset();
	return this->offsetSum;
}

// Function Table

SymbolTable::SymbolTable() {

}

void SymbolTable::addFunc(func _func) {
	funcTable.push_back(& _func);
}

void SymbolTable::addFunc(string _name, FUNC_TYPE _type) {
	funcTable.push_back(new func(_name, _type));
}

void SymbolTable::setFuncStatus(string _name, FUNC_STATUS _status) {
	list<func*>::iterator iter;
	for (iter = this->funcTable.begin(); iter != this->funcTable.end(); iter++) {
		if ((*iter)->name == _name) (*iter)->setStatus(_status);
	}
}

func* SymbolTable::findFunc(string _name) {
	list<func*>::iterator iter;
	for (iter = this->funcTable.begin(); iter != this->funcTable.end(); iter++) {
		if ((*iter)->name == _name) return (*iter);
	}
	return new func();
}

void SymbolTable::midCode_output(ofstream& output_handler)
{
	list<func*>::iterator func_iter;
	list<midCode*>::iterator midCode_iter;
	for (func_iter = this->funcTable.begin(); func_iter != this->funcTable.end(); func_iter++) {
		for (midCode_iter = (*func_iter)->midCodeList.begin(); midCode_iter != (*func_iter)->midCodeList.end(); midCode_iter++) {
			(*midCode_iter)->output(output_handler);
			output_handler << endl;
		}
		output_handler << endl;
	}
}
