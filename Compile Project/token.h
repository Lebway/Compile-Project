#ifndef TOKEN_H
#define TOKEN_H

#include<string>
#include<fstream>
#include<iostream>
#include<map>

using namespace std;

enum Symbol
{
	ILLEGAL,		// Illegal token
	ENDFILE,		// EOF
	IDENFR,
	INTCON,
	CHARCON,
	STRCON,
	CONSTTK,
	INTTK,
	VOIDTK,
	MAINTK,
	IFTK,
	ELSETK,
	DOTK,
	WHILETK,
	FORTK,
	SCANFTK,
	PRINTFTK,
	RETURNTK,
	PLUS,
	MINU,
	MULT,
	DIV,
	LSS,
	LEQ,
	GRE,
	GEQ,
	EQL,
	NEQ,
	ASSIGN,
	SEMICN,
	COMMA,
	LPARENT,
	RPARENT,
	LBRACK,
	RBRACK,
	LBRACE,
	RBRACE,
	CHARTK
};
class Token {
private:
	Symbol type;
	int number;
	string str;
	int lineNum;

public:
	Token(Symbol _type, int _number, string _str, int _lineNum);
	Token();
	void setType(Symbol _type);
	void setNumber(int _number);
	void setStr(string _str);
	void setLineNum(int _line_num);

	Symbol getType();
	int getNum();
	int getLineNum();
	string getStr();
	string toStr();
};


#endif // !TOKEN_H