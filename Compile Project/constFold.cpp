/*
    Const Fold:
	work list:
		+ remove all assign to const in midCode
		+ change a = const1 + const2 -> a = value
		+ change a = b + const1 -> a = b + value
		+ if a is tmpVar: set a to tmpConst and give the value to a
		+ change branch when const1 ?? const2 -> branch 0/1
		+ change branch when const1 ?? a -> branch value ?? a
*/

#include "optimize.h"
#include <cassert>

list<midCode*> Optimizer::constFold(list<midCode*> midCodeList) {
	list<midCode*> newMidCodeList;
	for (auto iter = midCodeList.begin(); iter != midCodeList.end(); iter++) {
		auto instr = (*iter)->instr;
		if (instr == midCode::MidCodeInstr::ASSIGN_CHAR || instr == midCode::MidCodeInstr::ASSIGN_INT) {
			if ((*iter)->t0->kind == CONST_IDENTIFIER) {	// remove const a = 10;
				continue;
			} else if ((*iter)->t0->name[0] == '#') { // tmp = 1
				(*iter)->t0->kind = CONST_IDENTIFIER;
				(*iter)->t0->setValue((*iter)->value);
				continue;
			}
		} else if (instr == midCode::MidCodeInstr::ASSIGN) { // a = const
			if ((*iter)->t1) {
				if ((*iter)->t1->kind == CONST_IDENTIFIER) {
					if ((*iter)->t0->name[0] == '#') {
						(*iter)->t0->kind = CONST_IDENTIFIER;
						(*iter)->t0->setValue((*iter)->t1->getValue());
						continue;
					} else {
						newMidCodeList.push_back(
							new midCode(midCode::MidCodeInstr::ASSIGN_INT, (*iter)->t0, (*iter)->t1->getValue())
						);
						continue;
					}
					assert(0);
				}
			}
		} else if (instr == midCode::MidCodeInstr::ADD) {
			if ((*iter)->t1->kind == CONST_IDENTIFIER && (*iter)->t2->kind == CONST_IDENTIFIER) {
				if ((*iter)->t0->name[0] == '#') {
					(*iter)->t0->kind = CONST_IDENTIFIER;
					(*iter)->t0->setValue((*iter)->t1->getValue() + (*iter)->t2->getValue());
					continue;
				} else {
					newMidCodeList.push_back(
						new midCode(midCode::MidCodeInstr::ASSIGN_INT, (*iter)->t0,
							(*iter)->t1->getValue() + (*iter)->t2->getValue()));
					continue;
				}
				assert(0);
			} else if ((*iter)->t1->kind == CONST_IDENTIFIER) {
				newMidCodeList.push_back(
					new midCode(midCode::MidCodeInstr::ADDI, (*iter)->t0,
						(*iter)->t2, (*iter)->t1->getValue()));
				continue;
			} else if ((*iter)->t2->kind == CONST_IDENTIFIER) {
				newMidCodeList.push_back(
					new midCode(midCode::MidCodeInstr::ADDI, (*iter)->t0,
						(*iter)->t1, (*iter)->t2->getValue()));
				continue;
			}
		} else if (instr == midCode::MidCodeInstr::SUB) {
			if ((*iter)->t1->kind == CONST_IDENTIFIER && (*iter)->t2->kind == CONST_IDENTIFIER) {
				if ((*iter)->t0->name[0] == '#') {
					(*iter)->t0->kind = CONST_IDENTIFIER;
					(*iter)->t0->setValue((*iter)->t1->getValue() - (*iter)->t2->getValue());
					continue;
				} else {
					newMidCodeList.push_back(
						new midCode(midCode::MidCodeInstr::ASSIGN_INT, (*iter)->t0,
							(*iter)->t1->getValue() - (*iter)->t2->getValue()));
					continue;
				}
				assert(0);
			} else if ((*iter)->t1->kind == CONST_IDENTIFIER) {		// sub t0 const a -> li $k1 const sub t0 k1 a
				newMidCodeList.push_back(
					new midCode(midCode::MidCodeInstr::SUBI_REVERSE, (*iter)->t0,
						(*iter)->t2, (*iter)->t1->getValue()));
				continue;
			} else if ((*iter)->t2->kind == CONST_IDENTIFIER) {
				newMidCodeList.push_back(
					new midCode(midCode::MidCodeInstr::ADDI, (*iter)->t0,
						(*iter)->t1, -1 * (*iter)->t2->getValue()));
				continue;
			}
		} else if (instr == midCode::MidCodeInstr::NEG) {
			if ((*iter)->t1->kind == CONST_IDENTIFIER) {
				if ((*iter)->t0->name[0] == '#') {
					(*iter)->t0->kind = CONST_IDENTIFIER;
					(*iter)->t0->setValue(-1 * (*iter)->t1->getValue());
					continue;
				} else {
					newMidCodeList.push_back(
						new midCode(midCode::MidCodeInstr::ASSIGN_INT, (*iter)->t0
							- 1 * (*iter)->t1->getValue()));
					continue;
				}
				assert(0);
			}
		} else if (instr == midCode::MidCodeInstr::MULT) {
			if ((*iter)->t1->kind == CONST_IDENTIFIER && (*iter)->t2->kind == CONST_IDENTIFIER) {
				if ((*iter)->t0->name[0] == '#') {
					(*iter)->t0->kind = CONST_IDENTIFIER;
					(*iter)->t0->setValue((*iter)->t1->getValue() * (*iter)->t2->getValue());
					continue;
				} else {
					newMidCodeList.push_back(
						new midCode(midCode::MidCodeInstr::ASSIGN_INT, (*iter)->t0,
							(*iter)->t1->getValue() * (*iter)->t2->getValue()));
					continue;
				}
				assert(0);
			} else if ((*iter)->t1->kind == CONST_IDENTIFIER) {
				newMidCodeList.push_back(
					new midCode(midCode::MidCodeInstr::MULTI, (*iter)->t0, (*iter)->t2, (*iter)->t1->getValue())
					);
				continue;
			} else if ((*iter)->t2->kind == CONST_IDENTIFIER) {
				newMidCodeList.push_back(
					new midCode(midCode::MidCodeInstr::MULTI, (*iter)->t0, (*iter)->t1, (*iter)->t2->getValue())
					);
				continue;
			}
		} else if (instr == midCode::MidCodeInstr::DIV) {
			if ((*iter)->t1->kind == CONST_IDENTIFIER && (*iter)->t2->kind == CONST_IDENTIFIER) {
				if ((*iter)->t0->name[0] == '#') {
					(*iter)->t0->kind = CONST_IDENTIFIER;
					(*iter)->t0->setValue((*iter)->t1->getValue() / (*iter)->t2->getValue());
					continue;
				} else {
					newMidCodeList.push_back(
						new midCode(midCode::MidCodeInstr::ASSIGN_INT, (*iter)->t0,
							(*iter)->t1->getValue() / (*iter)->t2->getValue())
					);
					continue;
				} assert(0);
			} else if ((*iter)->t1->kind == CONST_IDENTIFIER) {
				newMidCodeList.push_back(new midCode(midCode::MidCodeInstr::DIVI_REVERSE,
					(*iter)->t0, (*iter)->t2, (*iter)->t1->getValue()));
				continue;
			} else if ((*iter)->t2->kind == CONST_IDENTIFIER) {
				newMidCodeList.push_back(
					new midCode(midCode::MidCodeInstr::DIVI, (*iter)->t0, (*iter)->t1, (*iter)->t2->getValue())
				);
				continue;
			}
		} 

		newMidCodeList.push_back((*iter));
	}
	return newMidCodeList;
}