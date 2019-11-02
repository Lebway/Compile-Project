#include"tokenize.h"
#include"error_handler.h"
#include<fstream>
#include<string>
#include<map>
#include<iostream>
#include<ctype.h>
#include<list>

using namespace std;

Tokenizer::Tokenizer() {
	reservedWords.insert(pair<string, Symbol>("const", CONSTTK));
	reservedWords.insert(pair<string, Symbol>("int", INTTK));
	reservedWords.insert(pair<string, Symbol>("char", CHARTK));
	reservedWords.insert(pair<string, Symbol>("void", VOIDTK));
	reservedWords.insert(pair<string, Symbol>("main", MAINTK));
	reservedWords.insert(pair<string, Symbol>("if", IFTK));
	reservedWords.insert(pair<string, Symbol>("else", ELSETK));
	reservedWords.insert(pair<string, Symbol>("do", DOTK));
	reservedWords.insert(pair<string, Symbol>("while", WHILETK));
	reservedWords.insert(pair<string, Symbol>("for", FORTK));
	reservedWords.insert(pair<string, Symbol>("scanf", SCANFTK));
	reservedWords.insert(pair<string, Symbol>("printf", PRINTFTK));
	reservedWords.insert(pair<string, Symbol>("return", RETURNTK));
	read_char();
	this->lineNum = 1;

}


Tokenizer::Tokenizer(string filename, Error_handler* _error_handler) {
	ch = 0; 
	inFile.open(filename);
	reservedWords.insert(pair<string, Symbol>("const", CONSTTK));
	reservedWords.insert(pair<string, Symbol>("int", INTTK));
	reservedWords.insert(pair<string, Symbol>("char", CHARTK));
	reservedWords.insert(pair<string, Symbol>("void", VOIDTK));
	reservedWords.insert(pair<string, Symbol>("main", MAINTK));
	reservedWords.insert(pair<string, Symbol>("if", IFTK));
	reservedWords.insert(pair<string, Symbol>("else", ELSETK));
	reservedWords.insert(pair<string, Symbol>("do", DOTK));
	reservedWords.insert(pair<string, Symbol>("while", WHILETK));
	reservedWords.insert(pair<string, Symbol>("for", FORTK));
	reservedWords.insert(pair<string, Symbol>("scanf", SCANFTK));
	reservedWords.insert(pair<string, Symbol>("printf", PRINTFTK));
	reservedWords.insert(pair<string, Symbol>("return", RETURNTK));
	read_char();

	this->lineNum = 1;
	error_handler = _error_handler;
}

Token Tokenizer::next_token() {
	string str;
	str.clear();
	while (isspace(ch)) {
		if (ch == '\n') {
			this->lineNum++;
		}
		read_char();
	}
	
	if (isLetter_Special(ch)) {							// 标识符或者保留字
		while (isLetter_General(ch)) {
			str.append(1, ch);
			read_char();
		}
		return Token(
			reservedWords_lookup(str),
			0, 
			str,
			this->lineNum
		);
	} else if (isdigit(ch)) {
		return Token(
			INTCON,
			read_number(),
			"",
			this->lineNum
		);
	} else {
		Token newtoken = Token();
		newtoken.setLineNum(this->lineNum);
		switch (ch)
		{
		case '+': newtoken.setType(PLUS); break;
		case '-': newtoken.setType(MINU); break;
		case '*': newtoken.setType(MULT); break;
		case '/': newtoken.setType(DIV); break;
		case ';': newtoken.setType(SEMICN); break;
		case ',': newtoken.setType(COMMA); break;
		case '(': newtoken.setType(LPARENT); break;
		case ')': newtoken.setType(RPARENT); break;
		case '[': newtoken.setType(LBRACK); break;
		case ']': newtoken.setType(RBRACK); break;
		case '{': newtoken.setType(LBRACE); break;
		case '}': newtoken.setType(RBRACE); break;
		case '=':
			ch = inFile.peek();
			if (ch == '=') {
				newtoken.setType(EQL);
				read_char();
			}
			else {
				newtoken.setType(ASSIGN);
			}
			break;
		case '<':
			ch = inFile.peek();
			if (ch == '=') {
				read_char();
				newtoken.setType(LEQ);
			}
			else {
				newtoken.setType(LSS);
			}
			break;
		case '>':
			ch = inFile.peek();
			if (ch == '=') {
				read_char();
				newtoken.setType(GEQ);
			}
			else {
				newtoken.setType(GRE);
			}
			break;
		case '!':
			read_char();
			if (ch == '=') newtoken.setType(NEQ);
			else {
				error_handler->raise_error(Error(lineNum, ILLEGAL_SYMBOL));
			}
			break;
		case '\'':
			read_char();
			if (isWrongLetter(ch)) {
				error_handler->raise_error(Error(lineNum, ILLEGAL_SYMBOL));
			}
			newtoken.setNumber(ch);
			read_char();
			if (ch == '\'') newtoken.setType(CHARCON);
			else {
				error_handler->raise_error(Error(lineNum, ILLEGAL_SYMBOL));
			}
			break;
		case '"':
			while (true) {
				read_char();
				if (ch == '\"') break;
				if (isWrongStrLetter(ch)) {
					error_handler->raise_error(Error(lineNum, ILLEGAL_SYMBOL));
				}
				str.append(1, ch);
			}
			newtoken.setStr(str);
			newtoken.setType(STRCON);	// TODO: raise error?
			break;
		case EOF:
			newtoken.setType(ENDFILE);
			break;
		default:
			error_handler->raise_error(Error(lineNum, ILLEGAL_SYMBOL));
			break;
		}
		read_char();
		return newtoken;
	}
}


// Utils
bool Tokenizer::isLetter_Special(char c) {
	if (isalpha(c))  return true;
	else if (c == '_') return true;
	else return false;
}

bool Tokenizer::isWrongStrLetter(char c) {
	if (c <= 126 && c >= 35) return false;
	else if (c == 32) return false;
	else if (c == 33) return false;
	else return true;
}

bool Tokenizer::isWrongLetter(char c) {
	if (isalpha(c)) return false;
	else if (isdigit(c)) return	false;
	else {
		switch (c) {
		case '_':
		case '+': 
		case '-': 
		case '*': 
		case '/':
			return false;
		default:
			return true;
		}
	}
}

bool Tokenizer::isLetter_General(char c) {
	if (isalpha(c))  return true;
	else if (c == '_') return true;
	else if (isdigit(c)) return true;
	else return false;
}

void Tokenizer::read_char() {
	ch = this->inFile.get();
}

void Tokenizer::un_read_char(char c) {
	this->inFile.putback(c);
}

int Tokenizer::read_number() {
	int num = 0;
	while (isdigit(ch)) {
		num = num * 10 + ch - '0';
		read_char();
	}
	return num;
}

Symbol Tokenizer::reservedWords_lookup(string strIn) {
	map<string, Symbol>::iterator iter;
	iter = reservedWords.find(strIn);
	if (iter == reservedWords.end()) {	// cannt find -> IDENFR
		return IDENFR;
	}
	else {
		return iter->second;
	}
}

list<Token> Tokenizer::getTokenlist() {
	// You should init the tokenizer before call the getTokenlist
	list<Token> tokenlist;
	while (true) {
		Token newToken = this->next_token();
		if (newToken.getType() == ENDFILE) break;
		else {
			tokenlist.push_back(newToken);
		}
	}
	return tokenlist;
}

