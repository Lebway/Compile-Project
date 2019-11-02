#include <string>
#include <map>
#include <list>
#include "token.h"
#ifndef SYMBOL_TABLE__H
#define SYMBOL_TABLE__H

using namespace std;

enum IDENTIFIER_KIND
{
	CONST_IDENTIFIER,
	VAR_IDENTIFIER,
	ILLEGAL_KIND
};

enum IDENTIFIER_TYPE
{
	INT_IDENTIFIER,
	CHAR_IDENTIFIER,
	ILLEGAL_TYPE
};

class identifier
{
public:
	identifier();
	identifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type, int level);
	string name;
	IDENTIFIER_KIND kind;
	IDENTIFIER_TYPE type;
	int level;
private:
};

class identifierTable
{
public:
	identifierTable();
	void addIdentifier(identifier _identifier);
	void addIdentifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type, int level);
	identifier findIdentifier(string _name);
	void cleanLevel(int _leverl);
private:
	map<string,identifier> identifierMap;
};

enum FUNC_TYPE
{
	ILLEGAL_FUNC,
	VOID_FUNC,
	INT_FUNC,
	CHAR_FUNC
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
	string name;
	FUNC_TYPE type;
	FUNC_STATUS status;
private:
	int paramNum;
	list<Symbol> paramList;
};

class funcTable
{
public:
	funcTable();
	void addFunc(func _func);
	void addFunc(string _name, FUNC_TYPE _type);
	func* findFunc(string _name);
	void setFuncStatus(string _name, FUNC_STATUS _status);
private:
	map<string, func> funcMap;
};

#endif // !SYMBOL_TABLE__H

