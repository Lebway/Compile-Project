#pragma once
#include<string>

using namespace std;

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