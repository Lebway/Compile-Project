#include "optimize.h"
#include <cassert>

void Optimizer::inlineOptimize() {
	for (auto func_iter = this->symbolTable->funcTable.begin(); func_iter != this->symbolTable->funcTable.end(); func_iter++) {
		list<midCode*> newMidCode;
		func* cur_func = (*func_iter);
		for (auto midCode_iter = (*func_iter)->midCodeList.begin();
			midCode_iter != (*func_iter)->midCodeList.end(); midCode_iter++) {
			if ((*midCode_iter)->instr == midCode::MidCodeInstr::PUSH || 
				(*midCode_iter)->instr == midCode::MidCodeInstr::CALL) {
				// get the func we want to call
				func* call_func;
				auto tmp_iter = midCode_iter;
				while ((*tmp_iter)->instr != midCode::MidCodeInstr::CALL) tmp_iter++;
				call_func = this->symbolTable->findFunc((*tmp_iter)->label);
				// TODO: we add the size control here
				if (call_func->canInline && call_func->midCodeList.size() <= 50) {
					map<identifier*, identifier*> identifierChangeMap;
					map<string, string> label_change_map;
					list<midCode*> call_func_midCode;

					// add the identifier to current function
					for (identifier* _id : call_func->identifierTable.identifierList) {
						if (_id->location == LOCAL_LOCATION) {
							identifier* newIdentifier = new identifier(_id);
							newIdentifier->name = newIdentifier->name + "_" + call_func->name;
							identifierChangeMap[_id] = newIdentifier;
							cur_func->identifierTable.identifierList.push_back(newIdentifier);
						}
					}
					//  change the identifier and the label
					
					for (midCode* _code : call_func->midCodeList) {
						midCode::MidCodeInstr instr = _code->instr;
						identifier* t0 = _code->t0;
						identifier* t1 = _code->t1;
						identifier* t2 = _code->t2;
						int value = _code->value;
						string label = _code->label;
						if (t0 && t0->location == LOCAL_LOCATION) t0 = identifierChangeMap[t0];
						if (t1 && t1->location == LOCAL_LOCATION) t1 = identifierChangeMap[t1];
						if (t2 && t2->location == LOCAL_LOCATION) t2 = identifierChangeMap[t2];
						string label_new = "";
						map<string, string>::iterator find_label_change;
						switch (instr) {
						case (midCode::MidCodeInstr::BGT):
						case (midCode::MidCodeInstr::BGE):
						case (midCode::MidCodeInstr::BLT):
						case (midCode::MidCodeInstr::BLE):
						case (midCode::MidCodeInstr::BEQ):
						case (midCode::MidCodeInstr::BNE):
						case (midCode::MidCodeInstr::JUMP):
						case (midCode::MidCodeInstr::LABEL):
							find_label_change = label_change_map.find(label);
							if (find_label_change == label_change_map.end()) {
								label_new = midCode::genLabel();
								label_change_map.insert(pair<string, string>(label, label_new));
							} else {
								label_new = find_label_change->second;
							}
							label = label_new;
							break;
						default:
							break;
						}
						midCode* tmpCode = new midCode(
							instr, t0, t1, t2, label, value
						);
						call_func_midCode.push_back(tmpCode);
					}
					
					auto call_func_midCode_iter = call_func_midCode.begin();
					call_func_midCode_iter++;	// get over the label
					// convert push to assign
					for (int i = 0; i < (call_func->getParamNum()); i++) {
						newMidCode.push_back(
							new midCode(midCode::MidCodeInstr::ASSIGN, (*call_func_midCode_iter)->t0, (*midCode_iter)->t0)
						);
						call_func_midCode_iter++;
						midCode_iter++;
					}
					assert((*midCode_iter)->instr == midCode::MidCodeInstr::CALL);
					midCode_iter++;
					
					string endLabel = midCode::genLabel();

					identifier* target = NULL;
					if (midCode_iter != (*func_iter)->midCodeList.end()) {
						if ((*midCode_iter)->instr == midCode::MidCodeInstr::ASSIGN &&
							(*midCode_iter)->label == "ret_" + call_func->name) {
							target = (*midCode_iter)->t0;
							midCode_iter++;
						}
					}

					while (call_func_midCode_iter != call_func_midCode.end()) {
						if ((*call_func_midCode_iter)->instr == midCode::MidCodeInstr::RET) {
							if (target && (*call_func_midCode_iter)->t0) {
								newMidCode.push_back(new midCode(midCode::MidCodeInstr::ASSIGN,
									target, (*call_func_midCode_iter)->t0));
							}
							newMidCode.push_back(new midCode(midCode::MidCodeInstr::JUMP, endLabel));
						} else {
							newMidCode.push_back((*call_func_midCode_iter));
						}
						call_func_midCode_iter++;
					}

					newMidCode.push_back(new midCode(midCode::MidCodeInstr::LABEL, endLabel));
					midCode_iter--;
					continue;
				}
			}
			newMidCode.push_back((*midCode_iter));			
		}
		(*func_iter)->midCodeList = newMidCode;
	}	
}