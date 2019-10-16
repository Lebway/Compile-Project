#include "token.h"
#include<string>
#include<fstream>
#include<iostream>
#include<map>

using namespace std;

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

string Token::toStr() {
	string return_str;
	switch (this->getType())
	{
	case IDENFR:
		return_str += "IDENFR";
		return_str += " ";
		return_str += this->str;
		return return_str;
		break;
	case INTCON:
		return_str += "INTCON";
		return_str += " ";
		return_str += to_string(this->getNum());
		return return_str;
		break;
	case CHARCON:
		return_str += "CHARCON";
		return_str += " ";
		return_str += char(this->getNum());
		return return_str;
		break;
	case STRCON:
		return_str += "STRCON";
		return_str += " ";
		return_str += (this->getStr());
		return return_str;
		break;
	case CONSTTK:
		return "CONSTTK const";
		break;
	case INTTK:
		return "INTTK int";
		break;
	case CHARTK:
		return "CHARTK char";
		break;
	case VOIDTK:
		return "VOIDTK void";
		break;
	case MAINTK:
		return "MAINTK main" ;
		break;
	case IFTK:
		return "IFTK if";
		break;
	case ELSETK:
		return "ELSETK else";
		break;
	case DOTK:
		return "DOTK do";
		break;
	case WHILETK:
		return "WHILETK while";
		break;
	case FORTK:
		return "FORTK for";
		break;
	case SCANFTK:
		return "SCANFTK scanf";
		break;
	case PRINTFTK:
		return "PRINTFTK printf";
		break;
	case RETURNTK:
		return "RETURNTK return";
		break;
	case PLUS:
		return "PLUS +";
		break;
	case MINU:
		return "MINU -";
		break;
	case MULT:
		return "MULT *";
		break;
	case DIV:
		return "DIV /";
		break;
	case LSS:
		return "LSS <";
		break;
	case LEQ:
		return "LEQ <=";
		break;
	case GRE:
		return "GRE >";
		break;
	case GEQ:
		return "GEQ >=";
		break;
	case EQL:
		return "EQL ==";
		break;
	case NEQ:
		return "NEQ !=";
		break;
	case ASSIGN:
		return "ASSIGN =";
		break;
	case SEMICN:
		return "SEMICN ;";
		break;
	case COMMA:
		return "COMMA ,";
		break;
	case LPARENT:
		return "LPARENT (";
		break;
	case RPARENT:
		return "RPARENT )";
		break;
	case LBRACK:
		return "LBRACK [";
		break;
	case RBRACK:
		return "RBRACK ]";
		break;
	case LBRACE:
		return "LBRACE {";
		break;
	case RBRACE:
		return "RBRACE }";
		break;
	default:
		break;
	}
	return "";
}
