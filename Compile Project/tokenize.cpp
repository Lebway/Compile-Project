#include"tokenize.h"
#include<fstream>
#include<string>
#include<map>
#include<iostream>
#include<ctype.h>

using namespace std;
static char ch;
static map<string, int> reservedWords;
static string str;

void reservedWords_init() {
	reservedWords.insert(pair<string, int>("const", CONSTTK));
	reservedWords.insert(pair<string, int>("int", INTTK));
	reservedWords.insert(pair<string, int>("char", CHARTK));
	reservedWords.insert(pair<string, int>("void", VOIDTK));
	reservedWords.insert(pair<string, int>("main", MAINTK));
	reservedWords.insert(pair<string, int>("if", IFTK));
	reservedWords.insert(pair<string, int>("else", ELSETK));
	reservedWords.insert(pair<string, int>("do", DOTK));
	reservedWords.insert(pair<string, int>("while", WHILETK));
	reservedWords.insert(pair<string, int>("for", FORTK));
	reservedWords.insert(pair<string, int>("scanf", SCANFTK));
	reservedWords.insert(pair<string, int>("printf", PRINTFTK));
	reservedWords.insert(pair<string, int>("return", RETURNTK));
}

struct token next_token(ifstream& inFile) {
	struct token newtoken = { 0,0,0,&str };
	str.clear();
	while (isspace(ch)) read_char(inFile);
	if (isLetter_Special(ch)) {							// 标识符或者保留字
		while (isLetter_General(ch)) {
			str.append(1, ch);
			read_char(inFile);
		}
		newtoken.symbol = reservedWords_lookup(str);
		newtoken.string = &str;
	}
	else if (isdigit(ch)) {
		newtoken.number = read_number(inFile);
		newtoken.symbol = INTCON;
	}
	else {
		switch (ch)
		{
		case '+': newtoken.symbol = PLUS; break;
		case '-': newtoken.symbol = MINU; break;
		case '*': newtoken.symbol = MULT; break;
		case '/': newtoken.symbol = DIV; break;
		case ';': newtoken.symbol = SEMICN; break;
		case ',': newtoken.symbol = COMMA; break;
		case '(': newtoken.symbol = LPARENT; break;
		case ')': newtoken.symbol = RPARENT; break;
		case '[': newtoken.symbol = LBRACK; break;
		case ']': newtoken.symbol = RBRACK; break;
		case '{': newtoken.symbol = LBRACE; break;
		case '}': newtoken.symbol = RBRACE; break;
		case '=':
			ch = inFile.peek();
			if (ch == '=') {
				newtoken.symbol = EQL;
				read_char(inFile);
			}
			else {
				newtoken.symbol = ASSIGN;
			}
			break;
		case '<':
			ch = inFile.peek();
			if (ch == '=') {
				read_char(inFile);
				newtoken.symbol = LEQ;
			}
			else {
				newtoken.symbol = LSS;
			}
			break;
		case '>':
			ch = inFile.peek();
			if (ch == '=') {
				read_char(inFile);
				newtoken.symbol = GEQ;
			}
			else {
				newtoken.symbol = GRE;
			}
			break;
		case '!':
			read_char(inFile);
			if (ch == '=') newtoken.symbol = NEQ;
			else {
				// TODO: raise error
			}
			break;
		case '\'':				// TODO: 这里是否考虑转义字符?
			read_char(inFile);
			newtoken.c = ch;
			read_char(inFile);
			if (ch == '\'') newtoken.symbol = CHARCON;
			else {
				// TODO: raise error
			}
			break;
		case '"':
			while (true) {
				read_char(inFile);
				if (ch == '\"') break;
				str.append(1, ch);
			}
			newtoken.string = &str;
			newtoken.symbol = STRCON;	// TODO: raise error
			break;
		case EOF:
			newtoken.symbol = EOF;
			break;
		default:						// TODO: raise error
			// *a = 1;
			break;
		}
		read_char(inFile);
	}
	return newtoken;
}

// Utils
int isLetter_Special(char c) {
	if (isalpha(c))  return 1;
	else if (c == '_') return 1;
	else return 0;
}

int isLetter_General(char c) {
	if (isalpha(c))  return 1;
	else if (c == '_') return 1;
	else if (isdigit(c)) return 1;
	else return 0;
}

void read_char(ifstream& inFile) {
	ch = inFile.get();
}

void un_read_char(char c, ifstream& inFile) {
	inFile.putback(c);
}

int read_number(ifstream& inFile) {
	int num = 0;
	while (isdigit(ch)) {
		num = num * 10 + ch - '0';
		read_char(inFile);
	}
	return num;
}

int reservedWords_lookup(string strIn) {
	/*
		标识符 返回0
		否则返回对应值
	*/
	map<string, int>::iterator iter;
	iter = reservedWords.find(strIn);
	if (iter == reservedWords.end()) {	// cannt find 说明是标识符
		return IDENFR;
	}
	else {
		return iter->second;
	}
}