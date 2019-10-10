#include "token.h"
#include<string>
#include<fstream>
#include<iostream>
#include<map>

Token::Token(Symbol _type, int _number, string _str) {
	this->type = _type;
	this->number = _number;
	this->str = _str;
}

Token::Token() {
	this->type = ILLEGAL;
	this->number = 0;
	this->str = "";
}

void Token::setType(Symbol _type) {
	this->type = _type;
}

void Token::setNumber(int _number) {
	this->number = _number;
}

void Token::setStr(string _str) {
	this->str = _str;
}

Symbol Token::getType() {
	return type;
}

int Token::getNum() {
	return number;
}

string Token::getStr() {
	return str;
}
