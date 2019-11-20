#include "midCode.h"
#include <string>
#include <map>
#include <list>
#include "token.h"
#include "identifier.h"
#include <vector>

#ifndef SYMBOL_TABLE__H
#define SYMBOL_TABLE__H

using namespace std;

enum FUNC_TYPE
{
	ILLEGAL_FUNC,
	VOID_FUNC,
	INT_FUNC,
	CHAR_FUNC,
	GLOBAL_FUNC,
};

enum FUNC_STATUS
{
	RETURN_INT,
	RETURN_CHAR,
	RETURN_VOID,
	NON_RETURN
};

class func 
{
public:
	func();
	func(string _name, FUNC_TYPE _type);
	void setStatus(FUNC_STATUS _status);
	void addParam(Symbol _symbol);
	int getParamNum();
	list<Symbol> getParamList();
	void addIdentifier(identifier);
	void addIdentifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type);
	identifier* genTempVar(IDENTIFIER_TYPE);
	identifier* genTempConst(IDENTIFIER_TYPE, const int value);

	identifier* findIdentifier(string);
	int tempIdentifier;
	string name;
	FUNC_TYPE type;
	FUNC_STATUS status;
	list<midCode> midCodeList;
	identifierTable identifierTable;
	map<string, string> strToPrint;

private:
	int paramNum;
	list<Symbol> paramList;
};

class SymbolTable
{
public:
	void addFunc(func _func);
	void addFunc(string _name, FUNC_TYPE _type);
	func* findFunc(string _name);
	void setFuncStatus(string _name, FUNC_STATUS _status);
	SymbolTable();
	void midCode_output(ofstream&);
	list<func> funcTable;
};

#endif // !SYMBOL_TABLE__H

