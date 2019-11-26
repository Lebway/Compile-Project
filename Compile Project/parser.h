#ifndef PARSER__H
#define PARSER__H

#include"tokenize.h"
#include"grammarNode.h"
#include"symbolTable.h"
#include"error_handler.h"
#include<list>
#include<iostream>
#include<fstream>

using namespace std;


enum Expression_Type
{
	CHAR_EXPRESSION,
	INT_EXPERSSION
};

class Parser {
public:
	Parser(list<Token>, Error_handler* _error_handler, SymbolTable*);
	void printWholeTree(string _out_file_string);

private:
	list<Token> tokenlist;
	list<Token>::iterator iter;
	GrammarNode root;
	ofstream outFile;
	Error_handler* error_handler;
	SymbolTable* func_table;
	func* current_func;
	func* _global_func;
	
	void appendEnd(GrammarNode* father);
	void appendEnd(GrammarNode* father, Symbol token_type);
	GrammarNode* appendMidNode(GrammarNode* father, GrammarValue _grammar_value);

	void constDeclareParser(GrammarNode* father);
	void constDefineParser(GrammarNode* father); 
	void variableDeclareParser(GrammarNode* father);
	void variableDefineParser(GrammarNode* father);

	Expression_Type expressionParser(GrammarNode* father, identifier**);
	Expression_Type itemParser(GrammarNode* father, identifier**);
	Expression_Type factorParser(GrammarNode* father, identifier**);

	int integerParser(GrammarNode* father);
	int unsignIntParser(GrammarNode* father);
	string stringParser(GrammarNode* father);
	int strideParser(GrammarNode* father);

	void variableIdentifier(GrammarNode* father, IDENTIFIER_TYPE _type);

	void ifConditionParser(GrammarNode* father, string, bool);
	void ifStatementParser(GrammarNode* father);
	void loopStatementParser(GrammarNode* father);
	void assignStatementParser(GrammarNode* father);
	void statementParser(GrammarNode* father);
	void statementCollectionParser(GrammarNode* father);
	func* returnCallStatementParser(GrammarNode* father, identifier* t0);
	void noReturnCallStatementParser(GrammarNode* father);

	void mainFunctionParser(GrammarNode* father);
	func* headStateParser(GrammarNode* father);

	void returnFunctionParser(GrammarNode* father);
	void noReturnFunctionParser(GrammarNode* father);

	void valueParameterTableParser(GrammarNode* father, func* call_function);
	void parameterTableParser(GrammarNode* father);

	void scanfStatementParser(GrammarNode* father);
	void printfStatementParser(GrammarNode* father);
	void returnStatementParser(GrammarNode* father);

	void compositeStatementParser(GrammarNode* father);
	void programParser(GrammarNode* father);

	identifier* declareIdentifierParser(IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type);
	void declareParaIdentifierParser(IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type, string _name);

	identifier* analyseIdentifierParser();
	func* declareFunctionParser(FUNC_TYPE _type);
	func* analyseFunctionParser();

	void printTree(GrammarNode* father);
	void midCodeAdd(midCode _mid_code);
	identifier* getTempVar(IDENTIFIER_TYPE _type);
	identifier* genTempConst(IDENTIFIER_TYPE _type, int value);
};

bool isRelationOperator(Symbol op);

#endif // !PARSER__H

