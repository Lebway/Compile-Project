#ifndef IDENTIFIER__H
#define IDENTIFIER__H
#include <string>
#include <map>
using namespace std;

enum IDENTIFIER_KIND
{
	CONST_IDENTIFIER,
	VAR_IDENTIFIER,
	ARRAY_IDENTIFIER,
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
	identifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type);
	identifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type, int _value);
	string name;
	IDENTIFIER_KIND kind;
	IDENTIFIER_TYPE type;
	int value;
	int array_lenth;
};

class identifierTable
{
public:
	identifierTable();
	void addIdentifier(identifier _identifier);
	void addIdentifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type);
	identifier* findIdentifier(string _name);
	map<string, identifier> identifierMap;
};

#endif // !IDENTIFIER__H

