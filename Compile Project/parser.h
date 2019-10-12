#ifndef PARSER__H
#define PARSER__H

#include"tokenize.h"
#include"grammarNode.h"
#include<list>
using namespace std;

class Parser {
public:
	Parser(list<Token>);

private:
	list<Token> tokenlist;
	list<Token>::iterator iter;
	GrammarNode root;
	
	void appendEnd(GrammarNode* father);
	void appendEnd(GrammarNode* father, Symbol token_type);
	GrammarNode* appendMidNode(GrammarNode* father, GrammarValue _grammar_value);

	void constDeclareParser(GrammarNode* father);
	void constDefineParser(GrammarNode* father); 
	void variableDeclareParser(GrammarNode* father);
	void variableDefineParser(GrammarNode* father);

	void expressionParser(GrammarNode* father);
	void itemParser(GrammarNode* father);
	void factorParser(GrammarNode* father);

	void integerParser(GrammarNode* father);
	void unsignIntParser(GrammarNode* father);
	void stringParser(GrammarNode* father);
	void strideParser(GrammarNode* father);

	void identifierParser(GrammarNode* father);
	void variableIdentifier(GrammarNode* father);

	void ifConditionParser(GrammarNode* father);
	void ifStatementParser(GrammarNode* father);
	void loopStatementParser(GrammarNode* father);
	void assignStatementParser(GrammarNode* father);
	void statementParser(GrammarNode* father);
	void statementCollectionParser(GrammarNode* father);
	void returnCallStatementParser(GrammarNode* father);
	void noReturnCallStatementParser(GrammarNode* father);

	void mainFunctionParser(GrammarNode* father);

	void scanfStatementParser(GrammarNode* father);
	void printfStatementParser(GrammarNode* father);
	void returnStatementParser(GrammarNode* father);
};

#endif // !PARSER__H

