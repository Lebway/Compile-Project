#ifndef IDENTIFIER__H
#define IDENTIFIER__H
#include <string>
#include <map>
#include <list>
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

enum IDENTIFIER_LOCATION
{
	GLOBAL_LOCATION,
	LOCAL_LOCATION,
	ERROR_LOCATION,
};

class identifier
{
public:
	identifier();
	identifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type, int _value, IDENTIFIER_LOCATION _loc);
	identifier(const identifier*);
	string name;
	IDENTIFIER_KIND kind;
	IDENTIFIER_TYPE type;
	IDENTIFIER_LOCATION location;
	void setOffset(int);
	void setValue(int);
	int getValue();
	int offset;
	int array_lenth;
private:
	int value;
	bool have_set_value;
};

class identifierTable
{
public:
	identifierTable();
	identifier* addIdentifier(identifier _identifier);
	identifier* addIdentifier(string _name, IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type,
		int _value=0, IDENTIFIER_LOCATION _loc=ERROR_LOCATION);
	identifier* findIdentifier(string _name);
	// map<string, identifier*> identifierMap;
	list<identifier*> identifierList;
};

#endif // !IDENTIFIER__H

