#include "parser.h"

using namespace std;

Parser::Parser(list<Token> _tokenlist) {
	this->tokenlist = _tokenlist;
	this->iter = this->tokenlist.begin();
	this->root = GrammarNode(MID_NODE, PROGRAM);
	programParser(&(this->root));
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
		cout << (*iter).toStr() << endl;
		cout << "[DEBUG] got wrong token here" << endl;
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
		appendEnd(father, CONSTTK);
		GrammarNode* thisNode = appendMidNode(father, CONST_DEFINE);
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
			appendEnd(father, COMMA);
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
			// appendEnd(father, INTTK);
			appendEnd(father, COMMA);
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
		variableDefineParser(thisNode);
	}
	appendEnd(father, SEMICN);

	while ((*iter).getType() == INTTK || (*iter).getType() == CHARTK) {
		iter++; iter++;
		if ((*iter).getType() == LPARENT) {
			iter--; iter--;
			return;
		} else {
			iter--; iter--;
		}
		GrammarNode* newNode = appendMidNode(father, VARIABLE_DEFINE);
		variableDefineParser(newNode);
		appendEnd(father, SEMICN);
	}
}

// 变量定义
void Parser::variableDefineParser(GrammarNode* father) {
	if ((*iter).getType() == INTTK) {
		appendEnd(father, INTTK);
		variableIdentifier(father);
		while ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
			variableIdentifier(father);
		}
	} else if ((*iter).getType() == CHARTK) {
		appendEnd(father, CHARTK);
		variableIdentifier(father);
		while ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
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
// ＜因子＞=＜标识符＞｜＜标识符＞'['＜表达式＞']'｜＜有返回值函数调用语句＞
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
		GrammarNode* thisNode = appendMidNode(father, RETURN_CALL_STATEMENT);
		returnCallStatementParser(thisNode);
	} else {		// ＜标识符＞｜＜标识符＞'['＜表达式＞']'
		auto find_func = funcMap.find((*iter).getStr());
		if (find_func == funcMap.end()) {
			identifierParser(father);
			if ((*iter).getType() == LBRACK) {
				appendEnd(father, LBRACK);
				{
					GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
					expressionParser(thisNode);
				}
				appendEnd(father, RBRACK);
			}
		} else if (find_func->second == RETURN_FUNCTION) {
			GrammarNode* thisNode = appendMidNode(father, RETURN_CALL_STATEMENT);
			returnCallStatementParser(thisNode);
		} else {
			// TODO: raise error
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
		GrammarNode* thisNode = appendMidNode(father, STATEMENT);
		statementParser(thisNode);
	}
	if ((*iter).getType() == ELSETK) {
		appendEnd(father, ELSETK);
		GrammarNode* thisNode = appendMidNode(father, STATEMENT);
		statementParser(thisNode);
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
		{
			GrammarNode* thisNode = appendMidNode(father, STATEMENT);
			statementParser(thisNode);
		}
	} else if ((*iter).getType() == DOTK) {
		appendEnd(father, DOTK);
		{
			GrammarNode* thisNode = appendMidNode(father, STATEMENT);
			statementParser(thisNode);
		}
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
		{
			GrammarNode* thisNode = appendMidNode(father, STATEMENT);
			statementParser(thisNode);
		}
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
	{
		GrammarNode* thisNode = appendMidNode(father, COMPOSITE_STATEMENT);
		compositeStatementParser(thisNode);
	}
	appendEnd(father, RBRACE);
}

// ＜声明头部＞   ::=  int＜标识符＞ |char＜标识符＞
void Parser::headStateParser(GrammarNode* father) {
	if ((*iter).getType() == CHARTK) {
		appendEnd(father, CHARTK);
	} else if ((*iter).getType() == INTTK) {
		appendEnd(father, INTTK);
	} else {
		// TODO: raise error
	}
	// 插入FuncMap
	funcMap.insert(pair<string, GrammarValue>((*iter).getStr(), RETURN_FUNCTION));
	identifierParser(father);
}

// ＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}’
void Parser::returnFunctionParser(GrammarNode* father) {
	{
		GrammarNode* thisNode = appendMidNode(father, HEAD_STATE);
		headStateParser(thisNode);
	}
	appendEnd(father, LPARENT);
	{
		GrammarNode* thisNode = appendMidNode(father, PARAMETER_TABLE);
		parameterTableParser(thisNode);
	}
	appendEnd(father, RPARENT);
	appendEnd(father, LBRACE);
	{
		GrammarNode* thisNode = appendMidNode(father, COMPOSITE_STATEMENT);
		compositeStatementParser(thisNode);
	}
	appendEnd(father, RBRACE);
}

// ＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}’
void Parser::noReturnFunctionParser(GrammarNode* father) {
	appendEnd(father, VOIDTK);
	funcMap.insert(pair <string, GrammarValue>((*iter).getStr(), NO_RETURN_FUNCTION));
	identifierParser(father);
	appendEnd(father, LPARENT);
	{
		GrammarNode* thisNode = appendMidNode(father, PARAMETER_TABLE);
		parameterTableParser(thisNode);
	}
	appendEnd(father, RPARENT);
	appendEnd(father, LBRACE);
	{
		GrammarNode* thisNode = appendMidNode(father, COMPOSITE_STATEMENT);
		compositeStatementParser(thisNode);
	}
	appendEnd(father, RBRACE);
}

// ＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
void Parser::valueParameterTableParser(GrammarNode* father) {
	// 如果下一个token是右括号，则可以直接返回
	if ((*iter).getType() == RPARENT) return;
	else {
		{
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			expressionParser(thisNode);
		}
		while ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			expressionParser(thisNode);
		}
	}
}

// ＜参数表＞    :: = ＜类型标识符＞＜标识符＞{ ,＜类型标识符＞＜标识符＞ } | ＜空＞
void Parser::parameterTableParser(GrammarNode* father) {
	// 如果下一个token是右括号，则可以直接返回
	if ((*iter).getType() == RPARENT) return;
	else {
		{
			if ((*iter).getType() == CHARTK) appendEnd(father, CHARTK);
			else if ((*iter).getType() == INTTK) appendEnd(father, INTTK);
			else {
				// TODO: raise error
			}
			identifierParser(father);
			while ((*iter).getType() == COMMA) {
				appendEnd(father, COMMA);
				if ((*iter).getType() == CHARTK) appendEnd(father, CHARTK);
				else if ((*iter).getType() == INTTK) appendEnd(father, INTTK);
				else {
					// TODO: raise error
				}
				identifierParser(father);
			}
		}
	}
}

// ＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')’
void Parser::returnCallStatementParser(GrammarNode* father) {
	identifierParser(father);
	appendEnd(father, LPARENT);
	{
		GrammarNode* thisNode = appendMidNode(father, VALUE_PARAMETER_TABLE);
		valueParameterTableParser(thisNode);
	}
	appendEnd(father, RPARENT);
}

// ＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')’
void Parser::noReturnCallStatementParser(GrammarNode* father) {
	identifierParser(father);
	appendEnd(father, LPARENT);
	{
		GrammarNode* thisNode = appendMidNode(father, VALUE_PARAMETER_TABLE);
		valueParameterTableParser(thisNode);
	}
	appendEnd(father, RPARENT);
}

//＜语句＞::= ＜条件语句＞｜＜循环语句＞| '{'＜语句列＞'}'| ＜有返回值函数调用语句＞;
// |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜空＞;|＜返回语句＞;
void Parser::statementParser(GrammarNode* father) {
	switch ((*iter).getType()) {
	case(SEMICN):
		appendEnd(father, SEMICN);
		break;
	case(IFTK):
		{
			GrammarNode* thisNode = appendMidNode(father, IF_STATEMENT);
			ifStatementParser(thisNode); 
		}
		break;	
	case(WHILETK):
	case(DOTK):
	case(FORTK):
		{
			GrammarNode* thisNode = appendMidNode(father, LOOP_STATEMENT);
			loopStatementParser(thisNode);
		} 
		break;
	case(LBRACE):
		appendEnd(father, LBRACE);
		{
			GrammarNode* thisNode = appendMidNode(father, STATEMENT_COLLECTION);
			statementCollectionParser(thisNode);
		}
		appendEnd(father, RBRACE);
		break;
	case(SCANFTK):
		{
			GrammarNode* thisNode = appendMidNode(father, SCANF_STATEMENT);
			scanfStatementParser(thisNode);
			appendEnd(father, SEMICN);
		}
		break;
	case(PRINTFTK):
		{
			GrammarNode* thisNode = appendMidNode(father, PRINTF_STATEMENT);
			printfStatementParser(thisNode);
			appendEnd(father, SEMICN);
		}
		break;
	case(RETURNTK):
		{
			GrammarNode* thisNode = appendMidNode(father, RETURN_STATEMENT);
			returnStatementParser(thisNode);	
			appendEnd(father, SEMICN);
		}
		break;
	default:
		auto find_func = funcMap.find((*iter).getStr());
		if (find_func == funcMap.end()) {
			GrammarNode* thisNode = appendMidNode(father, ASSIGN_STATEMENT);
			assignStatementParser(thisNode);
		} else if (find_func->second == RETURN_FUNCTION) {
			GrammarNode* thisNode = appendMidNode(father, RETURN_CALL_STATEMENT);
			returnCallStatementParser(thisNode);
		} else if (find_func->second == NO_RETURN_FUNCTION) {
			GrammarNode* thisNode = appendMidNode(father, NO_RETURN_CALL_STATEMENT);
			noReturnCallStatementParser(thisNode);
		} else {
			// TODO: raise error
		}
		appendEnd(father, SEMICN);
		break;
	}
}

// ＜语句列＞   ::= ｛＜语句＞｝
void Parser::statementCollectionParser(GrammarNode* father) {
	while ((*iter).getType() != RBRACE) {
		GrammarNode* thisNode = appendMidNode(father, STATEMENT);
		statementParser(thisNode);
	}
}

// ＜程序＞    ::= ［＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
void Parser::programParser(GrammarNode* father) {
	// 常量说明
	if ((*iter).getType() == CONSTTK) {
		GrammarNode* thisNode = appendMidNode(father, CONST_DECLARE);
		constDeclareParser(thisNode);
	}
	// 变量说明
	if ((*iter).getType() == INTTK || (*iter).getType() == CHARTK) {
		// TODO: 这里是否可能出现溢出呢？
		iter++; iter++;
		if ((*iter).getType() != LPARENT) {
			iter--; iter--;
			GrammarNode* thisNode = appendMidNode(father, VARIABLE_DECLARE);
			variableDeclareParser(thisNode);
		}
		else {
			iter--; iter--;
		}

	}
	while (true) {
		if ((*iter).getType() == INTTK || (*iter).getType() == CHARTK) {
			GrammarNode* thisNode = appendMidNode(father, RETURN_FUNCTION);
			returnFunctionParser(thisNode);
		} else if ((*iter).getType() == VOIDTK) {
			iter++;
			if ((*iter).getType() == MAINTK) {
				iter--;
				break;
			} else {
				iter--;
			}
			GrammarNode* thisNode = appendMidNode(father, NO_RETURN_FUNCTION);
			noReturnFunctionParser(thisNode);
		} else {
			// TODO: raise error
		}
	}
	{
		GrammarNode* thisNode = appendMidNode(father, MAIN_FUNCTION);
		mainFunctionParser(thisNode);
	}
}

// ＜复合语句＞   ::=  ［＜常量说明＞］［＜变量说明＞］＜语句列＞

void Parser::compositeStatementParser(GrammarNode* father) {
	// 常量说明
	if ((*iter).getType() == CONSTTK) {
		GrammarNode* thisNode = appendMidNode(father, CONST_DECLARE);
		constDeclareParser(thisNode);
	}
	// 变量说明
	if ((*iter).getType() == INTTK || (*iter).getType() == CHARTK) {
		GrammarNode* thisNode = appendMidNode(father, VARIABLE_DECLARE);
		variableDeclareParser(thisNode);
	}
	// 语句列
	{
		GrammarNode* thisNode = appendMidNode(father, STATEMENT_COLLECTION);
		statementCollectionParser(thisNode);
	}
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

/* Print Out*/
void Parser::printWholeTree(string _out_file_string) {
	outFile.open(_out_file_string);
	printTree(&root);
}

void Parser::printTree(GrammarNode* father) {
	if (father->getType() == END_NODE) {
		outFile << father->toStr() << endl;

	} else {
		list<GrammarNode*> childList = father->getChildList();
		for (list<GrammarNode*>::iterator child = childList.begin(); child != childList.end(); child++) {
			printTree(*child);
		}
		outFile << father->toStr() << endl;
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




