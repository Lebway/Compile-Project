#include "optimize.h"

void Optimizer::peepHoleOptimize() {
	adjacentJump();
	adjacentAssign();
	// change bge to 
	// sub temp a b
	// bgez temp label
	// branchChange();
	
	modPeepHole();
}

/*
	#16 = a + b
	c = #16
*/
void Optimizer::adjacentAssign() {
	for (func* curFunc : this->symbolTable->funcTable) {
		list<midCode*> newMidCode;
		auto midCode_iter = curFunc->midCodeList.begin();
		bool change;
		list<midCode*>::iterator tmp_iter;
		list<midCode*>::iterator loop_check_iter;
		while (midCode_iter != curFunc->midCodeList.end()) {
			switch ((*midCode_iter)->instr) {
				// 1 def 2 use
			case(midCode::MidCodeInstr::ADD):
			case(midCode::MidCodeInstr::SUB):
			case(midCode::MidCodeInstr::MULT):
			case(midCode::MidCodeInstr::DIV):
			case(midCode::MidCodeInstr::ADDI):
			case(midCode::MidCodeInstr::SUBI):
			case(midCode::MidCodeInstr::MULTI):
			case(midCode::MidCodeInstr::DIVI):
			case(midCode::MidCodeInstr::SUBI_REVERSE):
			case(midCode::MidCodeInstr::DIVI_REVERSE):
			case(midCode::MidCodeInstr::NEG):
			case(midCode::MidCodeInstr::ASSIGN):
			case(midCode::MidCodeInstr::ASSIGN_INT):
			case(midCode::MidCodeInstr::ASSIGN_CHAR):
			case(midCode::MidCodeInstr::INPUT):
			case(midCode::MidCodeInstr::LOAD_IND):
				change = true;
				tmp_iter = midCode_iter;
				tmp_iter++;		// the adjacent midCode
				if (tmp_iter == curFunc->midCodeList.end()) {
					change = false;
				} else if ((*tmp_iter)->instr != midCode::MidCodeInstr::ASSIGN) {
					change = false;
				} else if ((*tmp_iter)->t1 != (*midCode_iter)->t0) {
					change = false;
				} else if ((*midCode_iter)->t0->location == GLOBAL_LOCATION){
					change = false;
				} else {
					loop_check_iter = tmp_iter; loop_check_iter++;
					while (loop_check_iter != curFunc->midCodeList.end()) {
						if ((*loop_check_iter)->t0 == (*midCode_iter)->t0) change = false;
						if ((*loop_check_iter)->t1 == (*midCode_iter)->t0) change = false;
						if ((*loop_check_iter)->t2 == (*midCode_iter)->t0) change = false;
						loop_check_iter++;
					}
				}

				if (change) {
					newMidCode.push_back(
						new midCode((*midCode_iter)->instr, (*tmp_iter)->t0, 
						(*midCode_iter)->t1, (*midCode_iter)->t2, 
							(*midCode_iter)->label, (*midCode_iter)->value)
					);
					midCode_iter++;
				} else {
					newMidCode.push_back(*midCode_iter);
				}
				break;
			default:
				newMidCode.push_back(*midCode_iter);
				break;
			}
			midCode_iter++;
		}

		curFunc->midCodeList = newMidCode;

	}
}

void Optimizer::adjacentJump() {
	for (func* curFunc : this->symbolTable->funcTable) {
		list<midCode*> newMidCode;
		auto midCode_iter = curFunc->midCodeList.begin();
		list<midCode*>::iterator tmp_iter;
		bool needIt;
		while (midCode_iter != curFunc->midCodeList.end()) {
			switch ((*midCode_iter)->instr) {
			case (midCode::MidCodeInstr::BGT):
			case (midCode::MidCodeInstr::BGE):
			case (midCode::MidCodeInstr::BLT):
			case (midCode::MidCodeInstr::BLE):
			case (midCode::MidCodeInstr::BEQ):
			case (midCode::MidCodeInstr::BNE):
			case (midCode::MidCodeInstr::BZ):
			case (midCode::MidCodeInstr::BNZ):
			case (midCode::MidCodeInstr::JUMP):
				tmp_iter = midCode_iter;
				tmp_iter++;
				needIt = true;
				while (tmp_iter!=curFunc->midCodeList.end()
					&& (*tmp_iter)->instr == midCode::MidCodeInstr::LABEL) {
					if ((*tmp_iter)->label == (*midCode_iter)->label) {
						needIt = false;
					}
					tmp_iter++;
				}
				if (needIt)
					newMidCode.push_back(*midCode_iter);
				break;
			default:
				newMidCode.push_back(*midCode_iter);
				break;
			}
			midCode_iter++;
		}
		
		curFunc->midCodeList = newMidCode;

	}
}


void Optimizer::modPeepHole() {
	//  #1 = x / y
	// 	#2 = #1 * y
	// 	res = x - #2
	for (func* curFunc : this->symbolTable->funcTable) {
		list<midCode*> newMidCode;
		auto midCodeIter = curFunc->midCodeList.begin();
		for (; midCodeIter != curFunc->midCodeList.end(); midCodeIter++) {
			auto iter1 = midCodeIter;
			auto iter2 = iter1; iter2++;
			if (iter2 == curFunc->midCodeList.end()) {
				newMidCode.push_back(*midCodeIter);
				continue;
			}
			auto iter3 = iter2; iter3++;
			if (iter3 == curFunc->midCodeList.end()) {
				newMidCode.push_back(*midCodeIter);
				continue;
			}
			if ((*iter1)->instr != midCode::MidCodeInstr::DIV) {
				newMidCode.push_back(*midCodeIter);
				continue;
			}

			if ((*iter2)->instr != midCode::MidCodeInstr::MULT) {
				newMidCode.push_back(*midCodeIter);
				continue;
			}

			if ((*iter3)->instr != midCode::MidCodeInstr::SUB) {
				newMidCode.push_back(*midCodeIter);
				continue;
			}

			identifier* temp1_1 = (*iter1)->t0;
			identifier* temp1_2 = (*iter1)->t1;
			identifier* temp1_3 = (*iter1)->t2;
			identifier* temp2_1 = (*iter2)->t0;
			identifier* temp2_2 = (*iter2)->t1;
			identifier* temp2_3 = (*iter2)->t2;
			identifier* temp3_1 = (*iter3)->t0;
			identifier* temp3_2 = (*iter3)->t1;
			identifier* temp3_3 = (*iter3)->t2;

			if (temp1_1 != temp2_2) {
				newMidCode.push_back(*midCodeIter);
				continue;
			}
			if (temp1_2 != temp3_2) {
				newMidCode.push_back(*midCodeIter);
				continue;
			}
			if (temp1_3 != temp2_3) {
				newMidCode.push_back(*midCodeIter);
				continue;
			}
			if (temp2_1 != temp3_3) {
				newMidCode.push_back(*midCodeIter);
				continue;
			}

			cout << "[DEBUG] PeepHole: find mod!" << endl;

			newMidCode.push_back(
				new midCode(midCode::MidCodeInstr::MOD,
					temp3_1,	// res
					temp1_2,
					temp1_3)
			);
			midCodeIter = iter3;
		}
		curFunc->midCodeList = newMidCode;
	}
}