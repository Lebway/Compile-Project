#ifndef TOKENIZE_H
#define TOKENIZE_H

#include<string>
#include<fstream>
#include<iostream>
#include<map>
#include"token.h"
#include<list>
#include"error_handler.h"
#include<string>

using namespace std;

class Tokenizer {
public:
	Tokenizer();
	Tokenizer(string filename, Error_handler* _error_handler);

	Token next_token();
	list<Token> getTokenlist();

private:
	char ch;
	int lineNum;
	ifstream inFile;
	map<string, Symbol> reservedWords;
	Error_handler* error_handler;

	void read_char();
	int read_number();
	void make_dict();
	void un_read_char(char c);
	bool isLetter_General(char c);
	bool isLetter_Special(char c);
	bool isWrongLetter(char c);
	bool isWrongStrLetter(char c);
	Symbol reservedWords_lookup(string token);
};

#endif