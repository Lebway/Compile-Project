#include "basicBlock.h"
#include <cassert>
using namespace std;

BasicBlock::BasicBlock() {}

BasicBlock::BasicBlock(vector<midCode*> _mid_code_list) {
	this->midCodeList = _mid_code_list;
}

BasicBlock::BasicBlock(list<midCode*> _mid_code_list) {
	for (midCode* tmp : _mid_code_list)
		this->midCodeList.push_back(tmp);
}

BasicBlock::BasicBlock(const vector<midCode*>::iterator _begin, const vector<midCode*>::iterator _end) {
	vector<midCode*>::iterator begin = _begin;
	for (; begin != _end; begin++) {
		this->midCodeList.push_back((*begin));
	}
}

void BasicBlock::addNextBlock(BasicBlock* _next_block) {
	this->nextBlocks.insert(_next_block);
	_next_block->addPrevBlock(this);
}

void BasicBlock::addPrevBlock(BasicBlock* _pre_block) {
	this->prevBlocks.insert(_pre_block);
}

void BasicBlock::setDefUse() {
	for (midCode* mid_code : this->midCodeList) {
		switch (mid_code->instr) {	// we check array specially 
		// 1 def 2 use
		case(midCode::MidCodeInstr::ADD):
		case(midCode::MidCodeInstr::SUB):
		case(midCode::MidCodeInstr::MULT):
		case(midCode::MidCodeInstr::DIV):
		case(midCode::MidCodeInstr::MOD):
			// Notice!!! i = i + 1;
			// firstly we use i, and then define i 
			// if we firstly def i, the answer will be error
			checkUse(mid_code->t1);
			checkUse(mid_code->t2);
			checkDef(mid_code->t0);
			break;
		// 1 def 1 use
		case(midCode::MidCodeInstr::ADDI):
		case(midCode::MidCodeInstr::SUBI):
		case(midCode::MidCodeInstr::MULTI):
		case(midCode::MidCodeInstr::DIVI):
		case(midCode::MidCodeInstr::SUBI_REVERSE):
		case(midCode::MidCodeInstr::DIVI_REVERSE):
		case(midCode::MidCodeInstr::NEG):
			checkUse(mid_code->t1);
			checkDef(mid_code->t0);
			break;
		case(midCode::MidCodeInstr::ASSIGN):
			if (mid_code->t1)
				checkUse(mid_code->t1);
			checkDef(mid_code->t0);
			break;
		// 1 def
		case(midCode::MidCodeInstr::ASSIGN_INT):
		case(midCode::MidCodeInstr::ASSIGN_CHAR):
		case(midCode::MidCodeInstr::INPUT):
			checkDef(mid_code->t0);
			break;
		// 1 use
		case(midCode::MidCodeInstr::PUSH):
		case(midCode::MidCodeInstr::RET):		// can be null
		case(midCode::MidCodeInstr::OUTPUT):	// can be null
		case(midCode::MidCodeInstr::BZ):
		case(midCode::MidCodeInstr::BNZ):
			if (mid_code->t0 != NULL) {
				checkUse(mid_code->t0);
			}
			break;
		// 2 use
		case(midCode::MidCodeInstr::BEQ):
		case(midCode::MidCodeInstr::BNE):
		case(midCode::MidCodeInstr::BGE):
		case(midCode::MidCodeInstr::BGT):
		case(midCode::MidCodeInstr::BLT):
		case(midCode::MidCodeInstr::BLE):
			checkUse(mid_code->t0);
			checkUse(mid_code->t1);
			break;
		// array
		case(midCode::MidCodeInstr::LOAD_IND):
			checkUse(mid_code->t2);	// TODO: we don't need to check the array
			checkDef(mid_code->t0);
			break;
		case(midCode::MidCodeInstr::STORE_IND):
			checkUse(mid_code->t2);
			checkUse(mid_code->t1);
			break;
		default:
			break;
		}
	}
}

void BasicBlock::checkDef(identifier* _identifier) {
	assert(_identifier);
	if (use.count(_identifier) == 0) 
		if (_identifier->kind != CONST_IDENTIFIER && _identifier->kind != ARRAY_IDENTIFIER)
			def.insert(_identifier);
}

void BasicBlock::checkUse(identifier* _identifier) {
	assert(_identifier);
	if (def.count(_identifier) == 0)
		if (_identifier->kind != CONST_IDENTIFIER && _identifier->kind != ARRAY_IDENTIFIER)
			use.insert(_identifier);
}
