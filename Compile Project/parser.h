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
	Parser(list<Token>, Error_handler* _error_handler);
	void printWholeTree(string _out_file_string);

private:
	list<Token> tokenlist;
	list<Token>::iterator iter;
	GrammarNode root;
	// map<string, GrammarValue> funcMap;	// TODO: too Ugly!
	ofstream outFile;
	Error_handler* error_handler;
	identifierTable* identifier_table;
	funcTable* func_table;
	func* current_func;
	int level;
	
	void appendEnd(GrammarNode* father);
	void appendEnd(GrammarNode* father, Symbol token_type);
	GrammarNode* appendMidNode(GrammarNode* father, GrammarValue _grammar_value);

	void constDeclareParser(GrammarNode* father);
	void constDefineParser(GrammarNode* father); 
	void variableDeclareParser(GrammarNode* father);
	void variableDefineParser(GrammarNode* father);

	Expression_Type expressionParser(GrammarNode* father);
	Expression_Type itemParser(GrammarNode* father);
	Expression_Type factorParser(GrammarNode* father);

	void integerParser(GrammarNode* father);
	void unsignIntParser(GrammarNode* father);
	void stringParser(GrammarNode* father);
	void strideParser(GrammarNode* father);

	void variableIdentifier(GrammarNode* father, IDENTIFIER_TYPE _type);

	void ifConditionParser(GrammarNode* father);
	void ifStatementParser(GrammarNode* father);
	void loopStatementParser(GrammarNode* father);
	void assignStatementParser(GrammarNode* father);
	void statementParser(GrammarNode* father);
	void statementCollectionParser(GrammarNode* father);
	func* returnCallStatementParser(GrammarNode* father);
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

	void declareIdentifierParser(IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type);
	identifier analyseIdentifierParser();
	func* declareFunctionParser(FUNC_TYPE _type);
	func* analyseFunctionParser();

	void printTree(GrammarNode* father);
};

bool isRelationOperator(Symbol op);

#endif // !PARSER__H

