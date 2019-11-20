#include "parser.h"
#include <cassert>

using namespace std;

Parser::Parser(list<Token> _tokenlist, Error_handler* _error_handler, SymbolTable* _symbol_table) {
	this->error_handler = _error_handler;
	this->tokenlist = _tokenlist;
	this->iter = this->tokenlist.begin();
	this->root = GrammarNode(MID_NODE, PROGRAM);

	this->func_table = _symbol_table;
	func_table->addFunc("_global_func", GLOBAL_FUNC);
	this->_golabl_func = func_table->findFunc("_global_func");
	this->current_func = func_table->findFunc("_global_func");

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
		identifier* t0;
		t0 = declareIdentifierParser(CONST_IDENTIFIER, CHAR_IDENTIFIER);
		appendEnd(father, IDENFR);
		appendEnd(father, ASSIGN);
		if ((*iter).getType() != CHARCON) {
			this->error_handler->raise_error((*iter).getLineNum(), ONLY_INT_CHAR_ASSIGN_CONST);
		} 
		midCodeAdd(midCode(midCode::MidCodeInstr::ASSIGN_CHAR, t0, (*iter).getNum()));
		appendEnd(father, CHARCON);
		while ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
			t0 = declareIdentifierParser(CONST_IDENTIFIER, CHAR_IDENTIFIER);
			appendEnd(father, IDENFR);
			appendEnd(father, ASSIGN);
			if ((*iter).getType() != CHARCON) {
				this->error_handler->raise_error((*iter).getLineNum(), ONLY_INT_CHAR_ASSIGN_CONST);
			}
			midCodeAdd(midCode(midCode::MidCodeInstr::ASSIGN_CHAR, t0, (*iter).getNum()));
			appendEnd(father, CHARCON);
		}
	} else if ((*iter).getType() == INTTK) {		// int
		appendEnd(father, INTTK);
		identifier* t0;
		int value = 1;
		t0 = declareIdentifierParser(CONST_IDENTIFIER, INT_IDENTIFIER);
		appendEnd(father, IDENFR);
		appendEnd(father, ASSIGN);
		if ((*iter).getType() == INTCON) {
			GrammarNode* thisNode = appendMidNode(father, INTEGER);
			value = integerParser(thisNode);
		} else if ((*iter).getType() == PLUS || (*iter).getType() == MINU) {
			iter++;
			if ((*iter).getType() == INTCON) {
				iter--;
				GrammarNode* thisNode = appendMidNode(father, INTEGER);
				value = integerParser(thisNode);
			} else {
				this->error_handler->raise_error((*iter).getLineNum(), ONLY_INT_CHAR_ASSIGN_CONST);
			}
		} else {
			iter++;
			this->error_handler->raise_error((*iter).getLineNum(), ONLY_INT_CHAR_ASSIGN_CONST);
		}
		midCodeAdd(midCode(midCode::MidCodeInstr::ASSIGN_INT, t0, value));

		
		while ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
			value = 1;
			t0 = declareIdentifierParser(CONST_IDENTIFIER, INT_IDENTIFIER);
			appendEnd(father, IDENFR);
			appendEnd(father, ASSIGN);
			if ((*iter).getType() == INTCON) {
				GrammarNode* thisNode = appendMidNode(father, INTEGER);
				value = integerParser(thisNode);
			} else if ((*iter).getType() == PLUS || (*iter).getType() == MINU) {
				iter++;
				if ((*iter).getType() == INTCON) {
					iter--;
					GrammarNode* thisNode = appendMidNode(father, INTEGER);
					value = integerParser(thisNode);
				} else {
					this->error_handler->raise_error((*iter).getLineNum(), ONLY_INT_CHAR_ASSIGN_CONST);
				}
			} else {
				iter++;
				this->error_handler->raise_error((*iter).getLineNum(), ONLY_INT_CHAR_ASSIGN_CONST);
			}
			midCodeAdd(midCode(midCode::MidCodeInstr::ASSIGN_INT, t0, value));
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
Expression_Type Parser::expressionParser(GrammarNode* father, identifier* t0) {
	Expression_Type return_type = CHAR_EXPRESSION;
	identifier* t1 = this->current_func->genTempVar(INT_IDENTIFIER);
	bool plus_flag = true;
	if ((*iter).getType() == PLUS) {
		return_type = INT_EXPERSSION;
		appendEnd(father, PLUS);
	} else if ((*iter).getType() == MINU) {
		return_type = INT_EXPERSSION;
		appendEnd(father, MINU);
		plus_flag = false;
	}
	{
		GrammarNode* thisNode = appendMidNode(father, ITEM);
		Expression_Type item_type = itemParser(thisNode, t1);
		if (item_type == INT_EXPERSSION) return_type = INT_EXPERSSION;
		if (plus_flag) midCodeAdd(midCode(midCode::MidCodeInstr::ASSIGN, t0, t1));
		else midCodeAdd(midCode(midCode::MidCodeInstr::NEG, t0, t1));
		plus_flag = true;
	}
	while ((*iter).getType() == PLUS || (*iter).getType() == MINU) {
		if ((*iter).getType() == MINU) plus_flag = false;
		return_type = INT_EXPERSSION;
		appendEnd(father);	// + | -
		GrammarNode* thisNode = appendMidNode(father, ITEM);
		itemParser(thisNode, t1);
		if (plus_flag) midCodeAdd(midCode(midCode::MidCodeInstr::ADD, t0, t0, t1));
		else midCodeAdd(midCode(midCode::MidCodeInstr::SUB, t0, t0, t1));
		plus_flag = true;
	}
	if (return_type == CHAR_EXPRESSION) t0->type = CHAR_IDENTIFIER;
	return return_type;
}

// 项
// ＜项＞ ::= ＜因子＞{＜乘法运算符＞＜因子＞}
Expression_Type Parser::itemParser(GrammarNode* father, identifier* t0) {
	Expression_Type return_type = CHAR_EXPRESSION;
	identifier* t1 = this->current_func->genTempVar(INT_IDENTIFIER);
	{
		GrammarNode* thisNode = appendMidNode(father, FACTOR);
		return_type = factorParser(thisNode, t1);
		midCodeAdd(midCode(midCode::MidCodeInstr::ASSIGN, t0, t1));
	}
	while ((*iter).getType() == MULT || (*iter).getType() == DIV) {
		Symbol factor_type = (*iter).getType();
		return_type = INT_EXPERSSION;
		appendEnd(father);	// * | /
		GrammarNode* thisNode = appendMidNode(father, FACTOR);
		factorParser(thisNode, t1);
		if (factor_type == MULT) midCodeAdd(midCode(midCode::MidCodeInstr::MULT, t0, t0, t1));
		else midCodeAdd(midCode(midCode::MidCodeInstr::DIV, t0, t0, t1));
	}
	if (return_type == CHAR_EXPRESSION) t0->type = CHAR_IDENTIFIER;
	return return_type;
}

// 因子
// ＜因子＞ ::= ＜标识符＞｜＜标识符＞'['＜表达式＞']'|'('＜表达式＞')'｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞
Expression_Type Parser::factorParser(GrammarNode* father, identifier* t0) {
	if ((*iter).getType() == CHARCON) {		// ＜字符＞
		midCodeAdd(midCode(midCode::MidCodeInstr::ASSIGN_CHAR, t0, (*iter).getNum()));
		appendEnd(father, CHARCON);
		t0->type = CHAR_IDENTIFIER;
		return CHAR_EXPRESSION;
	} else if ((*iter).getType() == LPARENT) {	// '('＜表达式＞')'
		appendEnd(father, LPARENT);
		identifier* t1 = this->current_func->genTempVar(INT_IDENTIFIER);
		{
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			expressionParser(thisNode, t1);
		}
		appendEnd(father, RPARENT);
		midCodeAdd(midCode(midCode::MidCodeInstr::ASSIGN, t0, t1));
		return INT_EXPERSSION;
	} else if ((*iter).getType() == PLUS || (*iter).getType() == MINU || (*iter).getType() == INTCON) {
		GrammarNode* thisNode = appendMidNode(father, INTEGER);
		int value = integerParser(thisNode);
		midCodeAdd(midCode(midCode::MidCodeInstr::ASSIGN_INT, t0, value));
		return INT_EXPERSSION;
	} else {		// ＜标识符＞｜＜标识符＞'['＜表达式＞']'
		// TODO: change here
		const func* func_res = analyseFunctionParser();
		if (func_res->type == ILLEGAL_FUNC) {
			identifier* result = analyseIdentifierParser();
			assert(result->kind != ILLEGAL_KIND);
			if (result->kind == ILLEGAL_KIND) {
				this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
			}
			appendEnd(father, IDENFR);
			if ((*iter).getType() == LBRACK) {
				// t0 = a[t2]
				identifier* t2 = this->current_func->genTempVar(INT_IDENTIFIER);
				appendEnd(father, LBRACK);
				{
					Expression_Type array_index_type;
					GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
					array_index_type = expressionParser(thisNode, t2);
					if (array_index_type == CHAR_EXPRESSION) {
						this->error_handler->raise_error((*iter).getLineNum(), ILLEGAL_ARRAY_INDEX);
					}
					midCodeAdd(midCode(midCode::MidCodeInstr::LOAD_IND, t0, result, t2));
				}
				appendEnd(father, RBRACK);
			} else {	// t0 = a
				midCodeAdd(midCode(midCode::MidCodeInstr::ASSIGN, t0, result));
			}
			if (result->type == CHAR_IDENTIFIER) {
				t0->type = CHAR_IDENTIFIER;
				return CHAR_EXPRESSION;
			}
			else return INT_EXPERSSION;
		} else if (func_res->type == VOID_FUNC) {
			assert(0);
		} else { // return function
			GrammarNode* thisNode = appendMidNode(father, RETURN_CALL_STATEMENT);
			func* call_func = returnCallStatementParser(thisNode, t0);
			if (call_func->type == CHAR_FUNC) {
				t0->type = CHAR_IDENTIFIER;
				return CHAR_EXPRESSION;
			}
			else return INT_EXPERSSION;
		}
	}
	return INT_EXPERSSION;
}

// 整数
// ＜整数＞ ::= ［＋｜－］＜无符号整数＞
int Parser::integerParser(GrammarNode* father) {
	int value = 1;
	if ((*iter).getType() == PLUS) {
		appendEnd(father, PLUS);
	} else if ((*iter).getType() == MINU) {
		appendEnd(father, MINU);
		value = -1;
	}
	GrammarNode* thisNode = appendMidNode(father, UNSIGNINT);
	value = value * unsignIntParser(thisNode);
	return value;
}

// 无符号整数
int Parser::unsignIntParser(GrammarNode* father) {
	// TODO: raiser error when not a legal Unsigned Int
	int value = (*iter).getNum();
	appendEnd(father, INTCON);
	return value;
}

// 字符串
string Parser::stringParser(GrammarNode* father) {
	string str = (*iter).getStr();
	appendEnd(father, STRCON);
	return str;
}

// 步长
int Parser::strideParser(GrammarNode* father) {
	GrammarNode* thisNode = appendMidNode(father, UNSIGNINT);
	return unsignIntParser(thisNode);
}

// 条件
// ＜条件＞  ::=  ＜表达式＞＜关系运算符＞＜表达式＞｜＜表达式＞
void Parser::ifConditionParser(GrammarNode* father, string label_else, bool true_jump) {
	// TODO: 整型表达式之间才能进行关系运算
	identifier* t1 = this->current_func->genTempVar(INT_IDENTIFIER);
	identifier* t2 = NULL;
	Expression_Type type1 = INT_EXPERSSION;
	Expression_Type type2 = INT_EXPERSSION;
	{ 
		GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
		type1 = expressionParser(thisNode, t1);
		if (type1 == CHAR_EXPRESSION) {
			this->error_handler->raise_error((*iter).getLineNum(), ILLEGAL_IF_CONDITION);
		}
	}
	if (isRelationOperator((*iter).getType())) {
		Symbol branch_type = (*iter).getType();
		t2 = this->current_func->genTempVar(INT_IDENTIFIER);
		appendEnd(father);
		{
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			type2 = expressionParser(thisNode, t2);
			if (type2 == CHAR_EXPRESSION) {
				this->error_handler->raise_error((*iter).getLineNum(), ILLEGAL_IF_CONDITION);
			}
		}

		switch (branch_type) {
		case LSS:	// if(a<b) -> if a >= b branch to else
			if(true_jump) midCodeAdd(midCode(midCode::MidCodeInstr::BLT, t1, t2, NULL, label_else));
			else midCodeAdd(midCode(midCode::MidCodeInstr::BGE, t1, t2, NULL, label_else));
			break;
		case LEQ:   // if(a<=b) -> if a > b branch to else
			if (true_jump) midCodeAdd(midCode(midCode::MidCodeInstr::BLE, t1, t2, NULL, label_else));
			else midCodeAdd(midCode(midCode::MidCodeInstr::BGT, t1, t2, NULL, label_else));
			break;
		case GRE:	// if(a>b) -> if a <= b branch to else
			if (true_jump) midCodeAdd(midCode(midCode::MidCodeInstr::BGT, t1, t2, NULL, label_else));
			else midCodeAdd(midCode(midCode::MidCodeInstr::BLE, t1, t2, NULL, label_else));
			break;
		case GEQ:	// if(a>=b) -> if a < b branch to else
			if (true_jump) midCodeAdd(midCode(midCode::MidCodeInstr::BGE, t1, t2, NULL, label_else));
			else midCodeAdd(midCode(midCode::MidCodeInstr::BLT, t1, t2, NULL, label_else));
			break;
		case EQL:	// if(a==b) -> if a != b branch to else
			if (true_jump) midCodeAdd(midCode(midCode::MidCodeInstr::BEQ, t1, t2, NULL, label_else));
			else midCodeAdd(midCode(midCode::MidCodeInstr::BNE, t1, t2, NULL, label_else));
			break;
		case NEQ:	// if(a!=b) -> if a == b branch to else
			if (true_jump) midCodeAdd(midCode(midCode::MidCodeInstr::BNE, t1, t2, NULL, label_else));
			else midCodeAdd(midCode(midCode::MidCodeInstr::BEQ, t1, t2, NULL, label_else));
			break;
		default:
			assert(0);
		}
	} else {
		if (true_jump) midCodeAdd(midCode(midCode::MidCodeInstr::BNZ, t1, NULL, NULL, label_else));
		else midCodeAdd(midCode(midCode::MidCodeInstr::BZ, t1, NULL, NULL, label_else));
	}
}

// 条件语句
// if '('＜条件＞')'＜语句＞［else＜语句＞］
void Parser::ifStatementParser(GrammarNode* father) {
	string label_else = midCode::genLabel();
	string label_end = midCode::genLabel();
	appendEnd(father, IFTK);
	appendEnd(father, LPARENT);
	{
		GrammarNode* thisNode = appendMidNode(father, IFCONDITION);
		ifConditionParser(thisNode, label_else, false); // ifCondition if false, branch to the label_else
	}
	appendEnd(father, RPARENT);
	{
		GrammarNode* thisNode = appendMidNode(father, STATEMENT);
		statementParser(thisNode);
	}
	midCodeAdd(midCode(midCode::MidCodeInstr::JUMP, label_end));		// jump label_end
	midCodeAdd(midCode(midCode::MidCodeInstr::LABEL, label_else));		// label_else
	if ((*iter).getType() == ELSETK) {
		appendEnd(father, ELSETK);
		GrammarNode* thisNode = appendMidNode(father, STATEMENT);
		statementParser(thisNode);
	}
	midCodeAdd(midCode(midCode::MidCodeInstr::LABEL, label_end));		// label_end
}

// 赋值语句 ＜赋值语句＞ ::=  ＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞
void Parser::assignStatementParser(GrammarNode* father) {
	identifier* t0 = analyseIdentifierParser();
	identifier* t2 = this->current_func->genTempVar(INT_IDENTIFIER);
	identifier* t1 = NULL;
	if (t0->kind == ILLEGAL_KIND) {
		this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
	}
	appendEnd(father, IDENFR);
	if ((*iter).getType() == LBRACK) {	// t0[t1] = t2
		t1 = this->current_func->genTempVar(INT_IDENTIFIER);
		appendEnd(father, LBRACK);
		{
			Expression_Type array_index_type;
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			array_index_type = expressionParser(thisNode, t1);
			if (array_index_type == CHAR_EXPRESSION) {
				this->error_handler->raise_error((*iter).getLineNum(), ILLEGAL_ARRAY_INDEX);
			}
		}
		appendEnd(father, RBRACK);
	} else if (t0->kind == CONST_IDENTIFIER) {
		this->error_handler->raise_error((*iter).getLineNum(), ASSIGN_TO_CONST);
	}
	appendEnd(father, ASSIGN);
	{
		GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
		expressionParser(thisNode, t2);
	}

	if (t1 == NULL) midCodeAdd(midCode(midCode::MidCodeInstr::ASSIGN, t0, t2));	// t0 = t2
	else midCodeAdd(midCode(midCode::MidCodeInstr::STORE_IND, t0, t1, t2));		// t0[t1] = t2
}

// ＜循环语句＞   ::=  while '('＜条件＞')'＜语句＞| do＜语句＞while '('＜条件＞')' |for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
void Parser::loopStatementParser(GrammarNode* father) {
	if ((*iter).getType() == WHILETK) {
		string label_head = midCode::genLabel();
		string label_end = midCode::genLabel();
		midCodeAdd(midCode(midCode::MidCodeInstr::LABEL, label_head));
		appendEnd(father, WHILETK);
		appendEnd(father, LPARENT);
		{
			GrammarNode* thisNode = appendMidNode(father, IFCONDITION);
			ifConditionParser(thisNode, label_end, false);
		}
		appendEnd(father, RPARENT);
		{
			GrammarNode* thisNode = appendMidNode(father, STATEMENT);
			statementParser(thisNode);
		}
		midCodeAdd(midCode(midCode::MidCodeInstr::JUMP, label_head));
		midCodeAdd(midCode(midCode::MidCodeInstr::LABEL, label_end));
	} else if ((*iter).getType() == DOTK) {
		string label_head = midCode::genLabel();
		midCodeAdd(midCode(midCode::MidCodeInstr::LABEL, label_head));
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
			ifConditionParser(thisNode, label_head, true);
		}
		appendEnd(father, RPARENT);
	} else if ((*iter).getType() == FORTK) {
		string label_head = midCode::genLabel();
		string label_end = midCode::genLabel();
		appendEnd(father, FORTK);
		appendEnd(father, LPARENT);
		// init
		identifier* result_init = analyseIdentifierParser();
		if (result_init->kind == ILLEGAL_KIND) {
			this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
		}
		appendEnd(father, IDENFR);
		if (result_init->kind == CONST_IDENTIFIER) {
			this->error_handler->raise_error((*iter).getLineNum(), ASSIGN_TO_CONST);
		}
		appendEnd(father, ASSIGN);
		identifier* t1 = this->current_func->genTempVar(INT_IDENTIFIER);
		{
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			expressionParser(thisNode, t1);
		}
		midCodeAdd(midCode(midCode::MidCodeInstr::ASSIGN, result_init, t1));
		appendEnd(father, SEMICN);
		// condition
		midCodeAdd(midCode(midCode::MidCodeInstr::LABEL, label_head));	// head
		{
			GrammarNode* thisNode = appendMidNode(father, IFCONDITION);
			ifConditionParser(thisNode, label_end, false);
		}
		appendEnd(father, SEMICN);
		// update
		identifier* update_t0 = analyseIdentifierParser();
		identifier* update_t1;
		if (update_t0->kind == ILLEGAL_KIND) {
			this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
		}
		appendEnd(father, IDENFR);
		if (update_t0->kind == CONST_IDENTIFIER) {
			this->error_handler->raise_error((*iter).getLineNum(), ASSIGN_TO_CONST);
		}
		appendEnd(father, ASSIGN);
		{
			update_t1 = analyseIdentifierParser();
			if (update_t1->kind == ILLEGAL_KIND) {
				this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
			}
			appendEnd(father, IDENFR);
		}
		Symbol update_op = (*iter).getType();
		int update_value;
		assert(update_op == PLUS || update_op == MINU);
		appendEnd(father);
		{
			GrammarNode* thisNode = appendMidNode(father, STRIDE);
			update_value = strideParser(thisNode);
		}
		appendEnd(father, RPARENT);
		{
			GrammarNode* thisNode = appendMidNode(father, STATEMENT);
			statementParser(thisNode);
		}
		// update
		if (update_op == PLUS) midCodeAdd(midCode(midCode::MidCodeInstr::ADDI, update_t0, update_t1, update_value));
		else midCodeAdd(midCode(midCode::MidCodeInstr::SUBI, update_t0, update_t1, update_value));

		// goto head
		midCodeAdd(midCode(midCode::MidCodeInstr::JUMP, label_head));

		midCodeAdd(midCode(midCode::MidCodeInstr::LABEL, label_end));
	} else assert(0);
}

// 读语句
// ＜读语句＞::=  scanf '('＜标识符＞{,＜标识符＞}')’
void Parser::scanfStatementParser(GrammarNode* father) {
	appendEnd(father, SCANFTK);
	appendEnd(father, LPARENT);
	{
		identifier* result = analyseIdentifierParser();
		if (result->kind == ILLEGAL_KIND) {
			this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
		}
		midCodeAdd(midCode(midCode::MidCodeInstr::INPUT, result));
		appendEnd(father, IDENFR);
	}
	while ((*iter).getType() == COMMA) {
		appendEnd(father, COMMA);
		identifier* result = analyseIdentifierParser();
		if (result->kind == ILLEGAL_KIND) {
			this->error_handler->raise_error((*iter).getLineNum(), UNDEFINEDNAME);
		}
		midCodeAdd(midCode(midCode::MidCodeInstr::INPUT, result));
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
		static int strIndex = 0;
		strIndex++;
		string strName = "str" + to_string(strIndex);
		GrammarNode* thisNode = appendMidNode(father, STRING);
		string str = stringParser(thisNode);
		this->current_func->strToPrint.insert(pair<string, string>(strName, str));
		if ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
			identifier* t1 = this->current_func->genTempVar(INT_IDENTIFIER);
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			expressionParser(thisNode, t1);
			midCodeAdd(midCode(midCode::MidCodeInstr::OUTPUT, t1, strName));
		} else {
			midCodeAdd(midCode(midCode::MidCodeInstr::OUTPUT, strName));
		}
	} else {
		identifier* t1 = this->current_func->genTempVar(INT_IDENTIFIER);
		GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
		expressionParser(thisNode, t1);
		midCodeAdd(midCode(midCode::MidCodeInstr::OUTPUT, t1));
	}
	appendEnd(father, RPARENT);
}

// 返回语句
// ＜返回语句＞   ::=  return['('＜表达式＞')']  
void Parser::returnStatementParser(GrammarNode* father) {
	appendEnd(father, RETURNTK);
	if ((*iter).getType() == LPARENT) {
		appendEnd(father, LPARENT);
		identifier* tmp = this->current_func->genTempVar(INT_IDENTIFIER);
		{
			Expression_Type return_expression_type;
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			return_expression_type = expressionParser(thisNode, tmp);
			if (return_expression_type == INT_EXPERSSION) {
				this->current_func->setStatus(RETURN_INT);
			} else {
				this->current_func->setStatus(RETURN_CHAR);
			}
		}
		midCodeAdd(midCode(midCode::MidCodeInstr::RET, tmp));
		appendEnd(father, RPARENT);
	} else {
		this->current_func->setStatus(RETURN_VOID);
		midCodeAdd(midCode(midCode::MidCodeInstr::RET, "void"));
	}
}

// main void main‘(’‘)’ ‘{’＜复合语句＞‘}’
void Parser::mainFunctionParser(GrammarNode* father) {
	appendEnd(father, VOIDTK);
	func* func_res = declareFunctionParser(VOID_FUNC);
	this->current_func = func_res;

	midCodeAdd(midCode(midCode::MidCodeInstr::FUNC_VOID, "main"));

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
	this->current_func = this->_golabl_func;
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
	GrammarNode* thisNode = appendMidNode(father, HEAD_STATE);
	this->current_func = headStateParser(thisNode);

	if (this->current_func->type == INT_FUNC)
		midCodeAdd(midCode(midCode::MidCodeInstr::FUNC_INT, this->current_func->name));
	else
		midCodeAdd(midCode(midCode::MidCodeInstr::FUNC_CHAR, this->current_func->name));
	
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
	// TODO: check

	appendEnd(father, RBRACE);
	this->current_func = this->_golabl_func;
}

// ＜无返回值函数定义＞  ::= void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}’
void Parser::noReturnFunctionParser(GrammarNode* father) {

	appendEnd(father, VOIDTK);
	func* func_res = declareFunctionParser(VOID_FUNC);
	this->current_func = func_res;
	// Add Mid Code
	midCodeAdd(midCode(midCode::MidCodeInstr::FUNC_VOID, this->current_func->name));

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

	if (this->current_func->status == NON_RETURN) {
		midCodeAdd(midCode(midCode::MidCodeInstr::RET));
	}

	appendEnd(father, RBRACE);

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
			assert(!paramList.empty());
			Symbol param_type = paramList.front();
			identifier* tmp;
			if (param_type == INTTK) tmp = this->current_func->genTempVar(INT_IDENTIFIER);
			else tmp = this->current_func->genTempVar(CHAR_IDENTIFIER);
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			Expression_Type param_expression_type = expressionParser(thisNode, tmp);
			give_param_num++;
			paramList.pop_front();
			midCodeAdd(midCode(midCode::MidCodeInstr::PUSH, tmp, give_param_num));		// PUSH t1
			if ((param_type == INTTK && param_expression_type == CHAR_EXPRESSION)
				|| (param_type == CHARTK && param_expression_type == INT_EXPERSSION)) {
				this->error_handler->raise_error((*iter).getLineNum(), PARAMETER_TYPE_DONT_MATCH);
			}
			
		}
		while ((*iter).getType() == COMMA) {
			assert(!paramList.empty());
			appendEnd(father, COMMA);
			Symbol param_type = paramList.front();
			identifier* tmp;
			if (param_type == INTTK) tmp = this->current_func->genTempVar(INT_IDENTIFIER);
			else tmp = this->current_func->genTempVar(CHAR_IDENTIFIER);
			GrammarNode* thisNode = appendMidNode(father, EXPRESSION);
			Expression_Type param_expression_type = expressionParser(thisNode, tmp);
			give_param_num++;
			paramList.pop_front();
			midCodeAdd(midCode(midCode::MidCodeInstr::PUSH, tmp, give_param_num));		// PUSH t1
			if ((param_type == INTTK && param_expression_type == CHAR_EXPRESSION)
				|| (param_type == CHARTK && param_expression_type == INT_EXPERSSION)) {
				this->error_handler->raise_error((*iter).getLineNum(), PARAMETER_TYPE_DONT_MATCH);
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
	int para_index = 0;
	if ((*iter).getType() == RPARENT) return;
	else {
		if ((*iter).getType() == CHARTK) {
			para_index++;
			this->current_func->addParam(CHARTK);
			appendEnd(father, CHARTK);
			declareParaIdentifierParser(VAR_IDENTIFIER, CHAR_IDENTIFIER, para_index);
			appendEnd(father, IDENFR);
		}
		else if ((*iter).getType() == INTTK) {
			para_index++;
			this->current_func->addParam(INTTK);
			appendEnd(father, INTTK);
			declareParaIdentifierParser(VAR_IDENTIFIER, INT_IDENTIFIER, para_index);
			appendEnd(father, IDENFR);
		} else {
			// TODO: raise error
		}
		while ((*iter).getType() == COMMA) {
			appendEnd(father, COMMA);
			if ((*iter).getType() == CHARTK) {
				para_index++;
				this->current_func->addParam(CHARTK);
				appendEnd(father, CHARTK);
				declareParaIdentifierParser(VAR_IDENTIFIER, CHAR_IDENTIFIER, para_index);
				appendEnd(father, IDENFR);
			}
			else if ((*iter).getType() == INTTK) {
				para_index++;
				this->current_func->addParam(INTTK);
				appendEnd(father, INTTK);
				declareParaIdentifierParser(VAR_IDENTIFIER, INT_IDENTIFIER, para_index);
				appendEnd(father, IDENFR);
			} else {
				// TODO: raise error
			}
		}
	}
}

// ＜有返回值函数调用语句＞ ::= ＜标识符＞'('＜值参数表＞')’
func* Parser::returnCallStatementParser(GrammarNode* father, identifier* _tmp) {
	func* func_res = analyseFunctionParser();
	appendEnd(father, IDENFR);
	appendEnd(father, LPARENT);
	{	// TODO: 传入值参数表 或者传出值参数表进行解析
		GrammarNode* thisNode = appendMidNode(father, VALUE_PARAMETER_TABLE);
		valueParameterTableParser(thisNode, func_res);
	}
	appendEnd(father, RPARENT);
	midCodeAdd(midCode(midCode::MidCodeInstr::CALL, func_res->name));		// CALL FUNC 
	if (_tmp) midCodeAdd(midCode(midCode::MidCodeInstr::ASSIGN, _tmp, NULL, NULL, "ret"));	// t0=ret
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
	midCodeAdd(midCode(midCode::MidCodeInstr::CALL, func_res->name));		// CALL FUNC 
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
				returnCallStatementParser(thisNode, NULL);
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
void Parser::declareParaIdentifierParser(IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type, int num) {
	string name = (*iter).getStr();
	if (this->current_func != this->_golabl_func)
		name += this->current_func->name;
	identifier* result = this->current_func->findIdentifier(name);
	assert(result->kind == ILLEGAL_KIND);
	this->current_func->addIdentifier(name, _kind, _type);
	result = this->current_func->findIdentifier(name);
	
	if (_type == INT_IDENTIFIER) 
		midCodeAdd(midCode(midCode::MidCodeInstr::PARA_INT, result, num));
	else 
		midCodeAdd(midCode(midCode::MidCodeInstr::PARA_CHAR, result, num));
}

identifier* Parser::declareIdentifierParser(IDENTIFIER_KIND _kind, IDENTIFIER_TYPE _type) {
	string name = (*iter).getStr();
	if (this->current_func != this->_golabl_func) 
		name += this->current_func->name;
	identifier* result = this->current_func->findIdentifier(name);
	assert(result->kind == ILLEGAL_KIND);
	this->current_func->addIdentifier(name, _kind, _type);
	result = this->current_func->findIdentifier(name);
	return result;
}

identifier* Parser::analyseIdentifierParser() {
	string name = (*iter).getStr();
	string local_name = name;
	if (this->current_func != this->_golabl_func)
		local_name += this->current_func->name;
	identifier* result = this->current_func->findIdentifier(local_name);
	if (result->kind != ILLEGAL_KIND) return result;
	else return this->_golabl_func->findIdentifier(name);
}

// Function
func* Parser::declareFunctionParser(FUNC_TYPE _type) {
	string name = (*iter).getStr();
	func* result = func_table->findFunc(name);
	if (result->type == ILLEGAL_FUNC) func_table->addFunc(name, _type);
	else this->error_handler->raise_error((*iter).getLineNum(), REDEFINITION);
// TODO:	appendEnd(father, IDENFR);
	return func_table->findFunc(name);
}

func* Parser::analyseFunctionParser() {
	string name = (*iter).getStr();
	func* result = func_table->findFunc(name);
	return result;
}


void Parser::variableIdentifier(GrammarNode* father, IDENTIFIER_TYPE _type) {
	identifier* res = declareIdentifierParser(VAR_IDENTIFIER, _type);
	if (_type == INT_IDENTIFIER) midCodeAdd(midCode(midCode::MidCodeInstr::VAR_INT, res));
	else midCodeAdd(midCode(midCode::MidCodeInstr::VAR_CHAR, res));

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

void Parser::midCodeAdd(midCode _mid_code) {
	this->current_func->midCodeList.push_back(_mid_code);
}