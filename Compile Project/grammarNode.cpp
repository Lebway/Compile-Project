#include"grammarNode.h"

GrammarNode::GrammarNode() {
	this->node_type = MID_NODE;
	this->grammar_value = ILLEGAL_GRAMMAR;
	this->token = Token();
}

GrammarNode::GrammarNode(NodeType _node_type, Token _token) {
	this->node_type = _node_type;
	this->token = _token;
	this->grammar_value = END_NODE_GRAMMAR;
}

GrammarNode::GrammarNode(NodeType _node_type, GrammarValue _grammar_value) {
	this->node_type = _node_type;
	this->grammar_value = _grammar_value;
	this->token = Token();
}

void GrammarNode::addChild(GrammarNode* child) {
	this->child_list.push_back(child);
}

NodeType GrammarNode::getType() {
	return this->node_type;
}

list<GrammarNode*> GrammarNode::getChildList() {
	return this->child_list;
}

string GrammarNode::toStr() {
	if (this->node_type == END_NODE) {
		return this->token.toStr();
	}
	else {
		switch (this->grammar_value) {
		case(CONST_DECLARE):
			return "<常量说明>";
		case(CONST_DEFINE):
			return "<常量定义>";
		case(VARIABLE_DECLARE):
			return "<变量说明>";
		case(VARIABLE_DEFINE):
			return "<变量定义>";
		case(INTEGER):
			return "<整数>";
		case(UNSIGNINT):
			return "<无符号整数>";
		case(STRING):
			return "<字符串>";
		case(STRIDE):
			return "<步长>";
		case(EXPRESSION):
			return "<表达式>";
		case(FACTOR):
			return "<因子>";
		case(ITEM):
			return "<项>";
		case(SCANF_STATEMENT):
			return "<读语句>";
		case(PRINTF_STATEMENT):
			return "<写语句>";
		case(RETURN_STATEMENT):
			return "<返回语句>";
		case(IFCONDITION):
			return "<条件>";
		case(IF_STATEMENT):
			return "<条件语句>";
		case(LOOP_STATEMENT):
			return "<循环语句>";
		case(ASSIGN_STATEMENT):
			return "<赋值语句>";
		case(STATEMENT):
			return "<语句>";
		case(STATEMENT_COLLECTION):
			return "<语句列>";
		case(RETURN_CALL_STATEMENT):
			return "<有返回值函数调用语句>";
		case(NO_RETURN_CALL_STATEMENT):
			return "<无返回值函数调用语句>";
		case(MAIN_FUNCTION):
			return "<主函数>";
		case(HEAD_STATE):
			return "<声明头部>";
		case(RETURN_FUNCTION):
			return "<有返回值函数定义>";
		case(NO_RETURN_FUNCTION):
			return "<无返回值函数定义>";
		case(VALUE_PARAMETER_TABLE):
			return "<值参数表>";
		case(PARAMETER_TABLE):
			return "<参数表>";
		case(PROGRAM):
			return "<程序>";
		case(COMPOSITE_STATEMENT):
			return "<复合语句>";
		case(ILLEGAL_GRAMMAR):
			return "ILLEGAL_GRAMMAR";
		case(END_NODE_GRAMMAR):
			return "END_NODE_GRAMMAR";
		default:
			break;
		}
		return "";
	}
}