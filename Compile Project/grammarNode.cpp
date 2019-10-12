#include"grammarNode.h"

GrammarNode::GrammarNode() {
	this->node_type = MID_NODE;
	this->grammar_value = ILLEGAL;
	this->token = Token();
}

GrammarNode::GrammarNode(NodeType _node_type, Token _token) {
	this->node_type = _node_type;
	this->token = _token;
	this->grammar_value = END_NODE;
}

GrammarNode::GrammarNode(NodeType _node_type, GrammarValue _grammar_value) {
	this->node_type = _node_type;
	this->grammar_value = _grammar_value;
	this->token = Token();
}

void GrammarNode::addChild(GrammarNode* child) {
	this->child_list.push_back(child);
}

string GrammarNode::toStr() {
	if (this->node_type == END_NODE) {
		return this->token.toStr();
	}
	else {
		// TODO: add toString() for the mid_node
		return "";
	}
}