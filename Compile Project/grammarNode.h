#ifndef GRAMMMAR_NODE__H
#define GRAMMMAR_NODE__H

#include<list>
#include"token.h"
#include<string>

using namespace std;

enum NodeType
{
	END_NODE,
	MID_NODE
};

enum GrammarValue 
{
	CONST_DECLARE,
	CONST_DEFINE,
	VARIABLE_DECLARE,
	VARIABLE_DEFINE,
	INTEGER,
	UNSIGNINT,
	STRING,
	STRIDE,
	EXPRESSION,
	FACTOR,
	ITEM,

	SCANF_STATEMENT,
	PRINTF_STATEMENT,
	RETURN_STATEMENT,

	IFCONDITION,
	IF_STATEMENT,
	LOOP_STATEMENT,
	ASSIGN_STATEMENT,
	STATEMENT,
	STATEMENT_COLLECTION,
	RETURN_CALL_STATEMENT,
	NO_RETURN_CALL_STATEMENT,

	MAIN_FUNCTION,
	HEAD_STATE,

	RETURN_FUNCTION,
	NO_RETURN_FUNCTION,

	VALUE_PARAMETER_TABLE,
	PARAMETER_TABLE,

	PROGRAM,
	COMPOSITE_STATEMENT,

	ILLEGAL_GRAMMAR,
	END_NODE_GRAMMAR,
};

class GrammarNode {
public:
	GrammarNode();
	GrammarNode(NodeType _node_type, Token _token);
	GrammarNode(NodeType _node_type, GrammarValue _grammar_value);
	void addChild(GrammarNode* child);
	string toStr();
	NodeType getType();
	list<GrammarNode*> getChildList();
private:
	NodeType node_type;
	Token token;
	GrammarValue grammar_value;
	list<GrammarNode*> child_list;
};

#endif // !GRAMMMAR_NODE__H

