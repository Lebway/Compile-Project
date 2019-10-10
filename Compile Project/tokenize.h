#ifndef TOKENIZE_H
#define TOKENIZE_H

#include<string>
#include<fstream>
#include<iostream>
#include<map>
#include"token.h"

using namespace std;

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

#endif