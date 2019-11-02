#include "parser.h"

using namespace std;

Parser::Parser(list<Token> _tokenlist, Error_handler* _error_handler) {
	this->error_handler = _error_handler;
	this->tokenlist = _tokenlist;
	this->iter = this->tokenlist.begin();
	this->root = GrammarNode(MID_NODE, PROGRAM);

	this->level = 1;
	// TODO: change it !
	this->identifier_table = new identifierTable();
	this->func_table = new funcTable();
	this->current_func = NULL;


	programParser(&(this->root));

}

void Parser::appendEnd(GrammarNode* father) {
	GrammarNode* end_node = new GrammarNode(END_NODE, *(this->iter));
	father->addChild(end_node);
	(iter)++;
}

void Parser::appendEnd(GrammarNode* father, Symbol token_type) {
	if ((*iter).getType() != token_type) {
		if (token_type == SEMICN) {
			// TODO: TOO UGLY!
			this->error_handler->raise_error((*iter).getLineNum()-1, MISSING_SEMICN); return;
		} else if (token_type == RPARENT) {
			this->error_handler->raise_error((*iter).getLineNum(), MISSING_RPARENT_RIGHT_SMALL); return;
		} else if (token_type == RBRACK) {
			this->error_handler->raise_error((*iter).getLineNum(), MISSING_RBRACK_RIGHT_MIDDLE); return;
		} else {
			cout << "[DEBUG] get unexpected token " << (*iter).toStr() << endl;
		}
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
// ＜常量说明＞ ::=  const＜常量定义＞;{ const＜常量定义＞;}
void Parser::constDeclareParser(GrammarNode* father) {			
	while ((*iter).getType() == CONSTTK) {
		appendEnd(father, CONSTTK);
		GrammarNode* thisNode = appendMidNode(father, CONST_DEFINE);
		constDefineParser(thisNode);
		appendEnd(father, SEMICN);
	}
}

// 常量定义
// ＜常量定义＞::= int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞} | char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
void Parser::constDefineParser(GrammarNode* father) {
	if ((*iter).getType() == CHARTK) {		// char
		appendEnd(father, CHARTK);
		declareIdentifierParser(CONST_IDENTIFIER, CHAR_IDENTIFIER);
		appendEnd(father, IDENFR);
		appendEnd(father, ASSIGN);
		if ((*iter).getType() != CHARCON) {
			this->error_handler->raise_error((*iter).getLineNum(), ONLY_INT_CHAR_ASSIGN_CONST);
		} 
		appendEnd(father, CHARCON);
		while ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
			declareIdentifierParser(CONST_IDENTIFIER, CHAR_IDENTIFIER);
			appendEnd(father, IDENFR);
			appendEnd(father, ASSIGN);
			if ((*iter).getType() != CHARCON) {
				this->error_handler->raise_error((*iter).getLineNum(), ONLY_INT_CHAR_ASSIGN_CONST);
			}
			appendEnd(father, CHARCON);
		}
	} else if ((*iter).getType() == INTTK) {		// int
		appendEnd(father, INTTK);
		declareIdentifierParser(CONST_IDENTIFIER, INT_IDENTIFIER);
		appendEnd(father, IDENFR);
		appendEnd(father, ASSIGN);
		if ((*iter).getType() == INTCON) {
			GrammarNode* thisNode = appendMidNode(father, INTEGER);
			integerParser(thisNode);
		} else if ((*iter).getType() == PLUS || (*iter).getType() == MINU) {
			iter++;
			if ((*iter).getType() == INTCON) {
				iter--;
				GrammarNode* thisNode = appendMidNode(father, INTEGER);
				integerParser(thisNode);
			} else {
				this->error_handler->raise_error((*iter).getLineNum(), ONLY_INT_CHAR_ASSIGN_CONST);
			}
		} else {
			iter++;
			this->error_handler->raise_error((*iter).getLineNum(), ONLY_INT_CHAR_ASSIGN_CONST);
		}
		
		while ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
			declareIdentifierParser(CONST_IDENTIFIER, INT_IDENTIFIER);
			appendEnd(father, IDENFR);
			appendEnd(father, ASSIGN);
			if ((*iter).getType() == INTCON) {
				GrammarNode* thisNode = appendMidNode(father, INTEGER);
				integerParser(thisNode);
			} else if ((*iter).getType() == PLUS || (*iter).getType() == MINU) {
				iter++;
				if ((*iter).getType() == INTCON) {
					iter--;
					GrammarNode* thisNode = appendMidNode(father, INTEGER);
					integerParser(thisNode);
				} else {
					this->error_handler->raise_error((*iter).getLineNum(), ONLY_INT_CHAR_ASSIGN_CONST);
				}
			} else {
				iter++;
				this->error_handler->raise_error((*iter).getLineNum(), ONLY_INT_CHAR_ASSIGN_CONST);
			}
		}
	} else {
		// TODO: raise error
	}
}

// 变量说明
// ＜变量说明＞  ::= ＜变量定义＞;{＜变量定义＞;}
void Parser::variableDeclareParser(GrammarNode* father) {
	//  Must have at least one variable define
	{
		GrammarNode* thisNode = appendMidNode(father, VARIABLE_DEFINE);
		variableDefineParser(thisNode);
	}
	appendEnd(father, SEMICN);

	// read 3 words, deal with int func() and int a;
	while ((*iter).getType() == INTTK || (*iter).getType() == CHARTK) {	
		iter++; iter++;
		if ((*iter).getType() == LPARENT) {		// is a function
			iter--; iter--;
			return;
		} else {
			iter--; iter--;
		}
		GrammarNode* thisNode = appendMidNode(father, VARIABLE_DEFINE);
		variableDefineParser(thisNode);
		appendEnd(father, SEMICN);
	}
}

// 变量定义
//＜变量定义＞::= ＜类型标识符＞(＜标识符＞|＜标识符＞'['＜无符号整数＞']'){,(＜标识符＞|＜标识符＞'['＜无符号整数＞']' )} 
void Parser::variableDefineParser(GrammarNode* father) {
	if ((*iter).getType() == INTTK) {
		appendEnd(father, INTTK);
		variableIdentifier(father, INT_IDENTIFIER);
		while ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
			variableIdentifier(father, INT_IDENTIFIER);
		}
	} else if ((*iter).getType() == CHARTK) {
		appendEnd(father, CHARTK);
		variableIdentifier(father, CHAR_IDENTIFIER);
		while ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
			variableIdentifier(father, CHAR_IDENTIFIER);
		}	
	} else {
		// TODO: raise error
	}
}

// 表达式
// ＜表达式＞ ::= ［＋｜－］＜项＞{＜加法运算符＞＜项＞} 
Expression_Type Parser::expressionParser(GrammarNode* father) {
	Expression_Type return_type = CHAR_EXPRESSION;
	if ((*iter).getType() == PLUS) {
		return_type = INT_EXPERSSION;
		appendEnd(father, PLUS);
	} else if ((*iter).getType() == MINU) {
		return_type = INT_EXPERSSION;
		appendEnd(father, MINU);
	}
	{
		GrammarNode* thisNode = appendMidNode(father, ITEM);
		Expression_Type item_type = itemParser(thisNode);
		if (item_type == INT_EXPERSSION) {
			return_type = INT_EXPERSSION;
		}
	}
	while ((*iter).getType() == PLUS || (*iter).getType() == MINU) {
		return_type = INT_EXPERSSION;
		appendEnd(father);	// + | -
		GrammarNode* thisNode = appendMidNode(father, ITEM);
		itemParser(thisNode);
	}
	return return_type;
}

// 项
// ＜项＞ ::= ＜因子＞{＜乘法运算符＞＜因子＞}
Expression_Type Parser::itemParser(GrammarNode* father) {
	Expression_Type return_type = CHAR_EXPRESSION;
	{
		GrammarNode* thisNode = appendMidNode(father, FACTOR);
		return_type = factorParser(thisNode);
	}
	while ((*iter).getType() == MULT || (*iter).getType() == DIV) {
		return_type = INT_EXPERSSION;
		appendEnd(father);	// * | /
		GrammarNode* thisNode = appendMidNode(father, FACTOR);
		factorParser(thisNode);
	}
	return return_type;
}

// 因子
// ＜因子＞ ::= ＜标识符＞｜＜标识符＞'['＜表达式＞']'|'('＜表达式＞')'｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞
Expression_Type Parser::factorParser(GrammarNode* father) {
	if ((*iter).getType() == CHARCON) {		// ＜字符＞
		appendEnd(father, CHARCON);
		return CHAR_EXPRESSION;
	} else if ((*iter).getType() == LPARENT) {	// '('＜表达式＞')'
		appendEnd(father, LPARENT);
		{
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			expressionParser(thisNode);
		}
		appendEnd(father, RPARENT);
		return INT_EXPERSSION;
	} else if ((*iter).getType() == PLUS || (*iter).getType() == MINU || (*iter).getType() == INTCON) {
		GrammarNode* thisNode = appendMidNode(father, INTEGER);
		integerParser(thisNode);
		return INT_EXPERSSION;
	} else {		// ＜标识符＞｜＜标识符＞'['＜表达式＞']'
		// TODO: change here
		const func* func_res = analyseFunctionParser();
		if (func_res->type == ILLEGAL_FUNC) {
			identifier result = analyseIdentifierParser();
			if (result.kind == ILLEGAL_KIND) {
				this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
			}
			appendEnd(father, IDENFR);
			if ((*iter).getType() == LBRACK) {
				appendEnd(father, LBRACK);
				{
					Expression_Type array_index_type;
					GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
					array_index_type = expressionParser(thisNode);
					if (array_index_type == CHAR_EXPRESSION) {
						this->error_handler->raise_error((*iter).getLineNum(), ILLEGAL_ARRAY_INDEX);
					}
				}
				appendEnd(father, RBRACK);
			}

			if (result.type == CHAR_IDENTIFIER) {
				return CHAR_EXPRESSION;
			} else {
				return INT_EXPERSSION;
			}

		} else if (func_res->type == VOID_FUNC) {
			// TODO: raise error
		} else { // return function
			GrammarNode* thisNode = appendMidNode(father, RETURN_CALL_STATEMENT);
			func* call_func = returnCallStatementParser(thisNode);
			if (call_func->type == CHAR_FUNC) {
				return CHAR_EXPRESSION;
			} else {
				return INT_EXPERSSION;
			}
		}
	}
	return INT_EXPERSSION;
}

// 整数
// ＜整数＞ ::= ［＋｜－］＜无符号整数＞
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
	// TODO: raiser error when not a legal Unsigned Int
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
// ＜条件＞    ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
void Parser::ifConditionParser(GrammarNode* father) {
	// TODO: 整型表达式之间才能进行关系运算
	Expression_Type type1 = INT_EXPERSSION;
	Expression_Type type2 = INT_EXPERSSION;
	{
		GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
		type1 = expressionParser(thisNode);
		if (type1 == CHAR_EXPRESSION) {
			this->error_handler->raise_error((*iter).getLineNum(), ILLEGAL_IF_CONDITION);
		}
	}
	if (isRelationOperator((*iter).getType())) {
		appendEnd(father);
		{
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			type2 = expressionParser(thisNode);
			if (type2 == CHAR_EXPRESSION) {
				this->error_handler->raise_error((*iter).getLineNum(), ILLEGAL_IF_CONDITION);
			}
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

// 赋值语句 ＜赋值语句＞ ::=  ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞
void Parser::assignStatementParser(GrammarNode* father) {
	identifier result = analyseIdentifierParser();
	if (result.kind == ILLEGAL_KIND) {
		this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
	}
	appendEnd(father, IDENFR);
	if ((*iter).getType() == LBRACK) {
		appendEnd(father, LBRACK);
		{
			Expression_Type array_index_type;
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			array_index_type = expressionParser(thisNode);
			if (array_index_type == CHAR_EXPRESSION) {
				this->error_handler->raise_error((*iter).getLineNum(), ILLEGAL_ARRAY_INDEX);
			}
		}
		appendEnd(father, RBRACK);
	} else if (result.kind == CONST_IDENTIFIER) {
		this->error_handler->raise_error((*iter).getLineNum(), ASSIGN_TO_CONST);
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
		if ((*iter).getType() != WHILETK) {
			this->error_handler->raise_error((*iter).getLineNum(), MISSING_WHILE_IN_DOWHILE);
			return;
		} else {
			appendEnd(father, WHILETK);
		}
		appendEnd(father, LPARENT);
		{
			GrammarNode* thisNode = appendMidNode(father, IFCONDITION);
			ifConditionParser(thisNode);
		}
		appendEnd(father, RPARENT);
	} else if ((*iter).getType() == FORTK) {
		appendEnd(father, FORTK);
		appendEnd(father, LPARENT);
		identifier result_init = analyseIdentifierParser();
		if (result_init.kind == ILLEGAL_KIND) {
			this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
		}
		appendEnd(father, IDENFR);
		if (result_init.kind == CONST_IDENTIFIER) {
			this->error_handler->raise_error((*iter).getLineNum(), ASSIGN_TO_CONST);
		}
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
		identifier result_refresh = analyseIdentifierParser();
		if (result_refresh.kind == ILLEGAL_KIND) {
			this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
		}
		appendEnd(father, IDENFR);
		if (result_refresh.kind == CONST_IDENTIFIER) {
			this->error_handler->raise_error((*iter).getLineNum(), ASSIGN_TO_CONST);
		}
		appendEnd(father, ASSIGN);
		{
			identifier result_refresh_right = analyseIdentifierParser();
			if (result_refresh_right.kind == ILLEGAL_KIND) {
				this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
			}
			appendEnd(father, IDENFR);
		}
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
// ＜读语句＞::=  scanf '('＜标识符＞{,＜标识符＞}')’
void Parser::scanfStatementParser(GrammarNode* father) {
	appendEnd(father, SCANFTK);
	appendEnd(father, LPARENT);
	{
		identifier result = analyseIdentifierParser();
		if (result.kind == ILLEGAL_KIND) {
			this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
		}
		appendEnd(father, IDENFR);
	}
	while ((*iter).getType() == COMMA) {
		appendEnd(father, COMMA);
		identifier result = analyseIdentifierParser();
		if (result.kind == ILLEGAL_KIND) {
			this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
		}
		appendEnd(father, IDENFR);
	}
	appendEnd(father, RPARENT);

}

// 写语句
// ＜写语句＞    ::= printf '('＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'| printf '('＜表达式＞')’
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
// ＜返回语句＞   ::=  return['('＜表达式＞')']  
void Parser::returnStatementParser(GrammarNode* father) {
	appendEnd(father, RETURNTK);
	if ((*iter).getType() == LPARENT) {
		appendEnd(father, LPARENT);
		{
			Expression_Type return_expression_type;
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			return_expression_type = expressionParser(thisNode);
			if (return_expression_type == INT_EXPERSSION) {
				this->current_func->setStatus(RETURN_INT);
			} else {
				this->current_func->setStatus(RETURN_CHAR);
			}
		}
		appendEnd(father, RPARENT);
	} else {
		this->current_func->setStatus(RETURN_VOID);
	}
}

// main void main‘(’‘)’ ‘{’＜复合语句＞‘}’
void Parser::mainFunctionParser(GrammarNode* father) {
	// Get into Function: level++
	this->level++;
	appendEnd(father, VOIDTK);
	func* func_res = declareFunctionParser(VOID_FUNC);
	this->current_func = func_res;

	appendEnd(father, MAINTK);
	appendEnd(father, LPARENT);
	appendEnd(father, RPARENT);
	appendEnd(father, LBRACE);
	{
		GrammarNode* thisNode = appendMidNode(father, COMPOSITE_STATEMENT);
		compositeStatementParser(thisNode);
	}

	if (this->current_func->status == RETURN_INT || this->current_func->status == RETURN_CHAR) {
		this->error_handler->raise_error((*iter).getLineNum(), RETURN_IN_NON_RETURN_FUNC);
	}

	appendEnd(father, RBRACE);
	// Leave the Function: level--
	this->identifier_table->cleanLevel(this->level);
	this->level--;
	this->current_func = NULL;
}

// ＜声明头部＞   ::=  int＜标识符＞ | char＜标识符＞
func* Parser::headStateParser(GrammarNode* father) {
	if ((*iter).getType() == CHARTK) {
		appendEnd(father, CHARTK);
		func* func_res = declareFunctionParser(CHAR_FUNC);
		if (func_res->type == ILLEGAL_FUNC) {
			this->error_handler->raise_error((*iter).getLineNum(), REDEFINITION);
		}
		appendEnd(father, IDENFR);
		return func_res;
	} else if ((*iter).getType() == INTTK) {
		appendEnd(father, INTTK);
		func* func_res = declareFunctionParser(INT_FUNC);
		if (func_res->type == ILLEGAL_FUNC) {
			this->error_handler->raise_error((*iter).getLineNum(), REDEFINITION);
		}
		appendEnd(father, IDENFR);
		return func_res;
	}
	return NULL;
}

// ＜有返回值函数定义＞  ::=  ＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}’
void Parser::returnFunctionParser(GrammarNode* father) {
	this->level++;
	GrammarNode* thisNode = appendMidNode(father, HEAD_STATE);
	this->current_func = headStateParser(thisNode);
	
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
	
	// check before leave
	// TODO: 这里太丑了！有时间改一下!
	if (this->current_func->status == NON_RETURN || this->current_func->status == RETURN_VOID) {
		this->error_handler->raise_error((*iter).getLineNum()-1, NON_RETURN_IN_RETURN_FUNC);
	} else if (this->current_func->type == INT_FUNC && this->current_func->status == RETURN_CHAR) {
		this->error_handler->raise_error((*iter).getLineNum()-1, NON_RETURN_IN_RETURN_FUNC);
	} else if (this->current_func->type == CHAR_FUNC && this->current_func->status == RETURN_INT) {
		this->error_handler->raise_error((*iter).getLineNum()-1, NON_RETURN_IN_RETURN_FUNC);
	}
	
	appendEnd(father, RBRACE);

	// Leave the function: level--
	this->identifier_table->cleanLevel(this->level);
	this->level--;
	this->current_func = NULL;
}

// ＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}’
void Parser::noReturnFunctionParser(GrammarNode* father) {
	this->level++;

	appendEnd(father, VOIDTK);
	func* func_res = declareFunctionParser(VOID_FUNC);
	this->current_func = func_res;

	if (func_res->type == ILLEGAL_FUNC) {
		this->error_handler->raise_error((*iter).getLineNum(), REDEFINITION);
	}
	appendEnd(father, IDENFR);
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

	// check before leave
	if (this->current_func->status == RETURN_INT || this->current_func->status == RETURN_CHAR) {
		this->error_handler->raise_error((*iter).getLineNum()-1, RETURN_IN_NON_RETURN_FUNC);
	}
	appendEnd(father, RBRACE);
	
	// Leave the function: level--
	this->identifier_table->cleanLevel(this->level);
	this->level--;
	this->current_func = NULL;
}

// ＜值参数表＞   ::= ＜表达式＞{,＜表达式＞}｜＜空＞
void Parser::valueParameterTableParser(GrammarNode* father, func* call_function) {
	// 如果下一个token是右括号，则可以直接返回
	list<Symbol> paramList = call_function->getParamList();
	int need_param_num = call_function->getParamNum();
	int give_param_num = 0;

	if ((*iter).getType() == RPARENT) {
		if (need_param_num != give_param_num) {
			this->error_handler->raise_error((*iter).getLineNum(), PARAMETER_NUMBER_DONT_MATCH);
		}
		return;
	} else {
		{//	TODO: 这里还是要判断表达式的类型
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			Expression_Type param_expression_type = expressionParser(thisNode);
			give_param_num++;
			if (!paramList.empty()) {
				Symbol param_type = paramList.front();
				paramList.pop_front();
				if ((param_type == INTTK && param_expression_type == CHAR_EXPRESSION)
					|| (param_type == CHARTK && param_expression_type == INT_EXPERSSION)) {
					this->error_handler->raise_error((*iter).getLineNum(), PARAMETER_TYPE_DONT_MATCH);
				}
			}
		}
		while ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			Expression_Type param_expression_type = expressionParser(thisNode);
			give_param_num++;
			if (!paramList.empty()) {
				Symbol param_type = paramList.front();
				paramList.pop_front();
				if ((param_type == INTTK && param_expression_type == CHAR_EXPRESSION)
					|| (param_type == CHARTK && param_expression_type == INT_EXPERSSION)) {
					this->error_handler->raise_error((*iter).getLineNum(), PARAMETER_TYPE_DONT_MATCH);
				}
			}
		}
		if (need_param_num != give_param_num) {
			this->error_handler->raise_error((*iter).getLineNum(), PARAMETER_NUMBER_DONT_MATCH);
		}
	}
}

// ＜参数表＞    :: = ＜类型标识符＞＜标识符＞{ ,＜类型标识符＞＜标识符＞ } | ＜空＞
void Parser::parameterTableParser(GrammarNode* father) {
	// 如果下一个token是右括号，则可以直接返回
	
	if ((*iter).getType() == RPARENT) return;
	else {
		if ((*iter).getType() == CHARTK) {
			this->current_func->addParam(CHARTK);
			appendEnd(father, CHARTK);
			declareIdentifierParser(VAR_IDENTIFIER, CHAR_IDENTIFIER);
			appendEnd(father, IDENFR);
		}
		else if ((*iter).getType() == INTTK) {
			this->current_func->addParam(INTTK);
			appendEnd(father, INTTK);
			declareIdentifierParser(VAR_IDENTIFIER, INT_IDENTIFIER);
			appendEnd(father, IDENFR);
		} else {
			// TODO: raise error
		}
		while ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
			if ((*iter).getType() == CHARTK) {
				this->current_func->addParam(CHARTK);
				appendEnd(father, CHARTK);
				declareIdentifierParser(VAR_IDENTIFIER, CHAR_IDENTIFIER);
				appendEnd(father, IDENFR);
			}
			else if ((*iter).getType() == INTTK) {
				this->current_func->addParam(INTTK);
				appendEnd(father, INTTK);
				declareIdentifierParser(VAR_IDENTIFIER, INT_IDENTIFIER);
				appendEnd(father, IDENFR);
			} else {
				// TODO: raise error
			}
		}
	}
}

// ＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')’
func* Parser::returnCallStatementParser(GrammarNode* father) {
	func* func_res = analyseFunctionParser();
	appendEnd(father, IDENFR);
	appendEnd(father, LPARENT);
	{	// TODO: 传入值参数表 或者传出值参数表进行解析
		GrammarNode* thisNode = appendMidNode(father, VALUE_PARAMETER_TABLE);
		valueParameterTableParser(thisNode, func_res);
	}
	appendEnd(father, RPARENT);
	return func_res;
}

// ＜无返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')’
void Parser::noReturnCallStatementParser(GrammarNode* father) {
	func* func_res = analyseFunctionParser();
	appendEnd(father, IDENFR);
	
	appendEnd(father, LPARENT);
	{
		// TODO: 传入值参数表 或者传出值参数表进行解析
		GrammarNode* thisNode = appendMidNode(father, VALUE_PARAMETER_TABLE);
		valueParameterTableParser(thisNode, func_res);
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
		}
		appendEnd(father, SEMICN);
		break;
	case(PRINTFTK):
		{
			GrammarNode* thisNode = appendMidNode(father, PRINTF_STATEMENT);
			printfStatementParser(thisNode);
		}
		appendEnd(father, SEMICN);
		break;
	case(RETURNTK):
		{
			GrammarNode* thisNode = appendMidNode(father, RETURN_STATEMENT);
			returnStatementParser(thisNode);	
		}
		appendEnd(father, SEMICN);
		break;
	default:
		iter++;
		if ((*iter).getType() == LPARENT) {
			// function
			iter--;
			const func* func_res = analyseFunctionParser();
			if (func_res->type == ILLEGAL_FUNC) {
				this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
				while ((*iter).getType() != RPARENT) iter++;
				iter++;
			} else if (func_res->type == VOID_FUNC) {
				GrammarNode* thisNode = appendMidNode(father, NO_RETURN_CALL_STATEMENT);
				noReturnCallStatementParser(thisNode);
			} else { // return func
				GrammarNode* thisNode = appendMidNode(father, RETURN_CALL_STATEMENT);
				returnCallStatementParser(thisNode);
			}
		} else {
			// identifier
			iter--;
			GrammarNode* thisNode = appendMidNode(father, ASSIGN_STATEMENT);
			assignStatementParser(thisNode);
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
		// 向后看 区分变量说明和函数声明
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
			// 向后看 区分main函数和其他void函数
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
// Identifier
void Parser::declareIdentifierParser(IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type) {
	string name = (*iter).getStr();
	identifier result = identifier_table->findIdentifier(name);
	if (result.kind == ILLEGAL_KIND) {	// Dont have in identifier table now
		identifier_table->addIdentifier(name, _kind, _type, this->level);
	} else {
		this->error_handler->raise_error((*iter).getLineNum(), REDEFINITION);
	}
	// appendEnd(father, IDENFR);
}

identifier Parser::analyseIdentifierParser() {
	string name = (*iter).getStr();
	identifier result = identifier_table->findIdentifier(name);
	// TODO: appendEnd(father, IDENFR);
	return result;
}

// Function
func* Parser::declareFunctionParser(FUNC_TYPE _type) {
	string name = (*iter).getStr();
	func* result = func_table->findFunc(name);
	if (result->type == ILLEGAL_FUNC) {
		func_table->addFunc(name, _type);
	} else {
		this->error_handler->raise_error((*iter).getLineNum(), REDEFINITION);
	}
// TODO:	appendEnd(father, IDENFR);
	return func_table->findFunc(name);
}

func* Parser::analyseFunctionParser() {
	string name = (*iter).getStr();
	func* result = func_table->findFunc(name);
	return result;
}


/*
void Parser::identifierParser(GrammarNode* father) {
	// 插入符号表
	appendEnd(father, IDENFR);
}
*/

void Parser::variableIdentifier(GrammarNode* father, IDENTIFIER_TYPE _type) {
	declareIdentifierParser(VAR_IDENTIFIER, _type);
	appendEnd(father, IDENFR);
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