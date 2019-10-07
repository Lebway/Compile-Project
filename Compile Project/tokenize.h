/*
	Tokenizer
*/

#pragma once
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

public:
	Token(Symbol _type, int _number, string _str) {
		this->type = _type;
		this->number = _number;
		this->str = _str;
	}
	Token() {
		this->type = ILLEGAL;
		this->number = 0;
		this->str = "";
	}
	void setType(Symbol _type) {
		this->type = _type;
	}
	void setNumber(int _number) {
		this->number = _number;
	}
	void setStr(string _str) {
		this->str = _str;
	}
	Symbol getType() {
		return type;
	}
	int getNum() {
		return number;
	}
	string getStr() {
		return str;
	}
};

class Tokenizer {
public:
	Tokenizer(string filename);
	Token next_token();

private:
	char ch;
	ifstream inFile;
	map<string, Symbol> reservedWords;
	void read_char();
	int read_number();
	void un_read_char(char c);
	bool isLetter_General(char c);
	bool isLetter_Special(char c);
	Symbol reservedWords_lookup(string token);
};



/*

struct token {
	int symbol;
	int	number;
	char c;
	string* string;
};

int isLetter_General(char c);
int isLetter_Special(char c);
int read_number(ifstream& inFile);
void read_char(ifstream& inFile);
void un_read_char(char c, ifstream& inFile);
int reservedWords_lookup(string token);
void reservedWords_init();
struct token next_token(ifstream& inFile);

#define IDENFR 38
#define INTCON 1
#define CHARCON 2
#define STRCON 3
#define CONSTTK 4
#define INTTK 5
#define VOIDTK 6
#define MAINTK 7
#define IFTK 8
#define ELSETK 9
#define DOTK 10
#define WHILETK 11
#define FORTK 12
#define SCANFTK 13
#define PRINTFTK 14
#define RETURNTK 15
#define PLUS 16
#define MINU 17
#define MULT 18
#define DIV 19
#define LSS 20
#define LEQ 21
#define GRE 22
#define GEQ 23
#define EQL 24
#define NEQ 25
#define ASSIGN 26
#define SEMICN 27
#define COMMA 28
#define LPARENT 29
#define RPARENT 30
#define LBRACK 31
#define RBRACK 32
#define LBRACE 33
#define RBRACE 34
#define CHARTK 35

*/