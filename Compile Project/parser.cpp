#include "parser.h"

using namespace std;

Parser::Parser(list<Token> _tokenlist) {
	this->tokenlist = _tokenlist;
	this->iter = this->tokenlist.begin();
	// TODO: switch case?
}

void Parser::appendEnd(GrammarNode* father) {
	GrammarNode* end_node = new GrammarNode(END_NODE, *(this->iter));
	father->addChild(end_node);
	(iter)++;
}

void Parser::appendEnd(GrammarNode* father, Symbol token_type) {
	if ((*iter).getType() != token_type) {
		// TODO: raise error
	}
	appendEnd(father);
}

GrammarNode* Parser::appendMidNode(GrammarNode* father, GrammarValue _grammar_value) {
	GrammarNode* thisNode = new GrammarNode(MID_NODE, _grammar_value);
	father->addChild(thisNode);
	return thisNode;
}



/******** Parser ********/

// 常量说明
void Parser::constDeclareParser(GrammarNode* father) {			
	while ((*iter).getType() == CONSTTK) {

		GrammarNode* thisNode = appendMidNode(father, CONST_DEFINE);
		appendEnd(father, CONSTTK);
		constDefineParser(thisNode);
		appendEnd(father, SEMICN);	// ;
	}
}

// 常量定义
void Parser::constDefineParser(GrammarNode* father) {
	if ((*iter).getType() == CHARTK) {		// char
		appendEnd(father, CHARTK);
		identifierParser(father);			// 同级，不建树
		appendEnd(father, ASSIGN);
		appendEnd(father, CHARCON);
		while ((*iter).getType() == COMMA) {
			appendEnd(father, CHARTK);
			identifierParser(father);
			appendEnd(father, ASSIGN);
			appendEnd(father, CHARCON);
		}
	} else if ((*iter).getType() == INTTK) {		// int
		appendEnd(father, INTTK);
		identifierParser(father);
		appendEnd(father, ASSIGN);
		{
			GrammarNode* thisNode = appendMidNode(father, INTEGER);
			integerParser(thisNode);
		}
		while ((*iter).getType() == COMMA) {
			appendEnd(father, INTTK);
			identifierParser(father);
			appendEnd(father, ASSIGN);
			{
				GrammarNode* thisNode = appendMidNode(father, INTEGER);
				integerParser(thisNode);
			}
		}
	} else {
		// TODO: raise error
	}
}

// 变量说明
void Parser::variableDeclareParser(GrammarNode* father) {
	//  Must have at least one variable define
	{
		GrammarNode* thisNode = appendMidNode(father, VARIABLE_DEFINE);
		variableDeclareParser(thisNode);
		appendEnd(father, SEMICN);
	}

	while ((*iter).getType() == INTTK || (*iter).getType() == CHARTK) {
		GrammarNode* newNode = appendMidNode(father, VARIABLE_DEFINE);
		variableDeclareParser(newNode);
		appendEnd(father, SEMICN);
	}
}

// 变量定义
void Parser::variableDefineParser(GrammarNode* father) {
	if ((*iter).getType() == INTTK) {
		appendEnd(father, INTTK);
		variableIdentifier(father);
		while ((*iter).getType() == COMMA) {
			variableIdentifier(father);
		}
	} else if ((*iter).getType() == CHARTK) {
		appendEnd(father, CHARTK);
		variableIdentifier(father);
		while ((*iter).getType() == COMMA) {
			variableIdentifier(father);
		}	
	} else {
		// TODO: raise error
	}
}

// 表达式
void Parser::expressionParser(GrammarNode* father) {
	if ((*iter).getType() == PLUS) {
		appendEnd(father, PLUS);
	} else if ((*iter).getType() == MINU) {
		appendEnd(father, MINU);
	}
	{
		GrammarNode* thisNode = appendMidNode(father, ITEM);
		itemParser(thisNode);
	}
	while ((*iter).getType() == PLUS || (*iter).getType() == MINU) {
		appendEnd(father);	// + | -
		GrammarNode* thisNode = appendMidNode(father, ITEM);
		itemParser(thisNode);
	}
}

// 项
void Parser::itemParser(GrammarNode* father) {
	{
		GrammarNode* thisNode = appendMidNode(father, FACTOR);
		factorParser(thisNode);
	}
	while ((*iter).getType() == MULT || (*iter).getType() == DIV) {
		appendEnd(father);	// * | /
		GrammarNode* thisNode = appendMidNode(father, FACTOR);
		factorParser(thisNode);
	}
}

// 因子
void Parser::factorParser(GrammarNode* father) {
	if ((*iter).getType() == CHARCON) {		// ＜字符＞
		appendEnd(father, CHARCON);
	} else if ((*iter).getType() == LPARENT) {	// '('＜表达式＞')'
		appendEnd(father, LPARENT);
		{
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			expressionParser(thisNode);
		}
		appendEnd(father, RPARENT);
	} else if ((*iter).getType() == PLUS || (*iter).getType() == MINU || (*iter).getType() == INTCON) {
		GrammarNode* thisNode = appendMidNode(father, INTEGER);
		integerParser(thisNode);
	} else if ((*iter).getType() == INTTK || (*iter).getType() == CHARTK) {
		// TODO: 有返回值函数调用语句
	} else {		// ＜标识符＞｜＜标识符＞'['＜表达式＞']'
		identifierParser(father);
		if ((*iter).getType() == LBRACK) {
			appendEnd(father, LBRACK);
			{
				GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
				expressionParser(thisNode);
			}
			appendEnd(father, RBRACK);
		}
	}
}

// 整数
void Parser::integerParser(GrammarNode* father) {
	if ((*iter).getType() == PLUS) {
		appendEnd(father, PLUS);
	} else if ((*iter).getType() == MINU) {
		appendEnd(father, MINU);
	}
	GrammarNode* thisNode = appendMidNode(father, UNSIGNINT);
	unsignIntParser(thisNode);
}

// 无符号整数
void Parser::unsignIntParser(GrammarNode* father) {
	// TODO: 如果不符合无符号整数的条件，则报错
	appendEnd(father, INTCON);
}

// 字符串
void Parser::stringParser(GrammarNode* father) {
	appendEnd(father, STRCON);
}

// 步长
void Parser::strideParser(GrammarNode* father) {
	GrammarNode* thisNode = appendMidNode(father, UNSIGNINT);
	unsignIntParser(thisNode);
}

// 条件
void Parser::ifConditionParser(GrammarNode* father) {
	// TODO: 整型表达式之间才能进行关系运算
	{
		GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
		expressionParser(thisNode);
	}
	if (isRelationOperator((*iter).getType())) {
		appendEnd(father);
		{
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			expressionParser(thisNode);
		}
	}
}

// 条件语句
// if '('＜条件＞')'＜语句＞［else＜语句＞］
void Parser::ifStatementParser(GrammarNode* father) {
	appendEnd(father, IFTK);
	appendEnd(father, LPARENT);
	{
		GrammarNode* thisNode = appendMidNode(father, IFCONDITION);
		ifConditionParser(thisNode);
	}
	appendEnd(father, RPARENT);
	{
		// TODO: 语句
	}
	if ((*iter).getType() == ELSETK) {
		appendEnd(father, ELSETK);
		// TODO: 语句
	}
}

// 赋值语句 ＜赋值语句＞   ::=  ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞
void Parser::assignStatementParser(GrammarNode* father) {
	identifierParser(father);
	if ((*iter).getType() == LBRACK) {
		appendEnd(father, LBRACK);
		{
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			expressionParser(thisNode);
		}
		appendEnd(father, RBRACK);
	}
	appendEnd(father, ASSIGN);
	{
		GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
		expressionParser(thisNode);
	}
}

// ＜循环语句＞   ::=  while '('＜条件＞')'＜语句＞| do＜语句＞while '('＜条件＞')' |for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
void Parser::loopStatementParser(GrammarNode* father) {
	if ((*iter).getType() == WHILETK) {
		appendEnd(father, WHILETK);
		appendEnd(father, LPARENT);
		{
			GrammarNode* thisNode = appendMidNode(father, IFCONDITION);
			ifConditionParser(thisNode);
		}
		appendEnd(father, RPARENT);
		// TODO: 语句

	} else if ((*iter).getType() == DOTK) {
		appendEnd(father, DOTK);
		// TODO: 语句
		appendEnd(father, WHILETK);
		appendEnd(father, LPARENT);
		{
			GrammarNode* thisNode = appendMidNode(father, IFCONDITION);
			ifConditionParser(thisNode);
		}
		appendEnd(father, RPARENT);
	} else if ((*iter).getType() == FORTK) {
		appendEnd(father, FORTK);
		appendEnd(father, LPARENT);
		identifierParser(father);
		appendEnd(father, ASSIGN);
		{
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			expressionParser(thisNode);
		}
		appendEnd(father, SEMICN);
		{
			GrammarNode* thisNode = appendMidNode(father, IFCONDITION);
			ifConditionParser(thisNode);
		}
		appendEnd(father, SEMICN);
		identifierParser(father);
		appendEnd(father, ASSIGN);
		identifierParser(father);
		if ((*iter).getType() == PLUS || (*iter).getType() == MINU) {
			appendEnd(father);
		} else {
			// TODO: raise error
		}
		{
			GrammarNode* thisNode = appendMidNode(father, STRIDE);
			strideParser(thisNode);
		}
		appendEnd(father, RPARENT);
		// TODO: 语句
	} else {
		// TODO: raise error
	}

}




// 读语句
void Parser::scanfStatementParser(GrammarNode* father) {
	appendEnd(father, SCANFTK);
	appendEnd(father, LPARENT);
	{
		identifierParser(father);
	}
	while ((*iter).getType() == COMMA) {
		appendEnd(father, COMMA);
		identifierParser(father);
	}
	appendEnd(father, RPARENT);

}

// 写语句
void Parser::printfStatementParser(GrammarNode* father) {
	appendEnd(father, PRINTFTK);
	appendEnd(father, LPARENT);
	if ((*iter).getType() == STRCON) {
		GrammarNode* thisNode = appendMidNode(father, STRING);
		stringParser(thisNode);
		if ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			expressionParser(thisNode);
		}
	} else {	
		GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
		expressionParser(thisNode);
	}
	appendEnd(father, RPARENT);
}

// 返回语句
void Parser::returnStatementParser(GrammarNode* father) {
	appendEnd(father, RETURNTK);
	if ((*iter).getType() == LPARENT) {
		appendEnd(father, LPARENT);
		{
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			expressionParser(thisNode);
		}
		appendEnd(father, RPARENT);
	}
}

// main void main‘(’‘)’ ‘{’＜复合语句＞‘}’
void Parser::mainFunctionParser(GrammarNode* father) {
	appendEnd(father, VOIDTK);
	appendEnd(father, MAINTK);
	appendEnd(father, LPARENT);
	appendEnd(father, RPARENT);
	appendEnd(father, LBRACE);
	// TODO: 复合语句
	appendEnd(father, RBRACE);
}




/** Same Level **/
// 标识符
void Parser::identifierParser(GrammarNode* father) {
	// 插入符号表
	appendEnd(father, IDENFR);
}

void Parser::variableIdentifier(GrammarNode* father) {
	identifierParser(father);
	if ((*iter).getType() == LBRACK) {
		appendEnd(father, LBRACK);
		{
			GrammarNode* thisNode = new GrammarNode(MID_NODE, UNSIGNINT);
			father->addChild(thisNode);
			unsignIntParser(thisNode);
		}
		appendEnd(father, RBRACK);
	}
}

/***** Utils *****/

bool isRelationOperator(Symbol op) {
	switch (op) {
	case LSS:
	case LEQ:
	case GRE:
	case GEQ:
	case EQL:
	case NEQ:
		return true;
	default:
		return false;
	}
}




