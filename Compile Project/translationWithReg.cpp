#include "mipsWithReg.h"
#include "objCode.h"

#define NOVALUE 0
#define needValue true
#define emptyisOk false 

MipsWithReg::MipsWithReg(SymbolTable * _symbol_table) {
	this->symbol_table = _symbol_table;
}

void MipsWithReg::mipsGen() {

	// establish the flow chart
	for (func* curFunc : symbol_table->funcTable) {
		FlowChart* newFlowChart = new FlowChart(curFunc);
		newFlowChart->livenessAnalysis();
		flowChartMap[curFunc] = newFlowChart;
		/*
		int count = 0;
		for (BasicBlock* block : newFlowChart->blocks) {
			cout << "Block: " << ++count << endl;
			cout << "out: ";
			for (identifier* id : block->out) {
				cout << id->name << " ";
			}
			cout << endl;
		}
		*/
	}

	// optimize
	deadDefOptimize();
	assignOptimize();
	
	for (func* this_func : symbol_table->funcTable) {
		regPool = new registerPool(&this->mipsCode);
		if (this_func->name == "_global_func") {
			genBigSpace(this_func);
			addObjCode(objCode::Instr::j, Reg::zero, Reg::zero, Reg::zero, NOVALUE, "main");
			continue;
		}
		this_func->setOffset();	
		for (BasicBlock* this_block : flowChartMap[this_func]->blocks) {
			for (midCode* this_midCode : this_block->midCodeList) {
				switch (this_midCode->instr) {
				case(midCode::MidCodeInstr::JUMP):
					toMips_jump(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::LABEL):
					toMips_label(this_midCode, this_func); break;
				case(midCode::MidCodeInstr::RET):
					toMips_ret(this_midCode, this_func); break;
				case(midCode::MidCodeInstr::ADD):
					toMips_add(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::ADDI):
					toMips_addi(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::SUB):
					toMips_sub(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::SUBI):
					toMips_subi(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::SUBI_REVERSE):
					toMips_subi_reverse(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::MULT):
					toMips_mult(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::MULTI):
					toMips_multi(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::DIV):
					toMips_div(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::DIVI):
					toMips_divi(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::DIVI_REVERSE):
					toMips_divi_reverse(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::NEG):
					toMips_neg(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::LOAD_IND):
					toMips_loadInd(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::STORE_IND):
					toMips_storeInd(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::ASSIGN):
					toMips_assign(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::ASSIGN_CHAR):
					toMips_assign_char(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::ASSIGN_INT):
					toMips_assign_int(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::CALL):
					toMips_call(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::BGT):
					toMips_bgt(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::BGE):
					toMips_bge(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::BLT):
					toMips_blt(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::BLE):
					toMips_ble(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::BEQ):
					toMips_beq(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::BNE):
					toMips_bne(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::BZ):
					toMips_bz(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::BNZ):
					toMips_bnz(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::INPUT):
					toMips_input(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::OUTPUT):
					toMips_output(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::PUSH):
					toMips_push(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::FUNC_VOID):
					toMips_func_void(this_midCode, this_func);
					regPool->countReference(&flowChartMap[this_func]->blocks, this_func);
					break;
				case(midCode::MidCodeInstr::FUNC_INT):
					toMips_func_int(this_midCode, this_func); 
					regPool->countReference(&flowChartMap[this_func]->blocks, this_func);
					break;
				case(midCode::MidCodeInstr::FUNC_CHAR):
					toMips_func_char(this_midCode, this_func); 
					regPool->countReference(&flowChartMap[this_func]->blocks, this_func);
					break;
				case(midCode::MidCodeInstr::PARA_INT):
					toMips_para_int(this_midCode, this_func); break;
				case(midCode::MidCodeInstr::PARA_CHAR):
					toMips_para_char(this_midCode, this_func); break;
				case(midCode::MidCodeInstr::MOD):
					toMips_mod(this_midCode, this_func, this_block); break;
				case(midCode::MidCodeInstr::SAVE_REG):
					assert(0); break;
					// toMips_save_reg(this_midCode, this_func); break;
				case(midCode::MidCodeInstr::RECOVER_REG):
					// toMips_recover_reg(this_midCode, this_func); break;
					/// For Const Fold:

				default:
					break;
				}
			}
			regPool->clearTemp(this_block);
		}
	} 
}

void MipsWithReg::toMips_jump(midCode* code, func* this_func, BasicBlock* this_block) {
	regPool->clearTemp(this_block);
	addObjCode(objCode::Instr::j, Reg::zero, Reg::zero, Reg::zero, NOVALUE, code->label);
}

void MipsWithReg::toMips_label(midCode* code, func* this_func) {
	addObjCode(objCode::Instr::label, Reg::zero, Reg::zero, Reg::zero, NOVALUE, code->label);
}

void MipsWithReg::toMips_ret(midCode* code, func* this_func) {
	// TODO: recover the reg before leave
	if (this_func->name == "main") return;
	if (code->t0 != NULL) {
		Reg source = regPool->findReg(code->t0);
		if (source != Reg::ILLEGAL_REGISTER) {
			addObjCode(objCode::Instr::move, Reg::v0, source, Reg::zero, NOVALUE, "");
			regPool->readReg(source);
		}
		else loadIdentifierFromMem(code->t0, this_func, Reg::v0);
	}

	regPool->writeBackGlobalVar();

	addObjCode(objCode::Instr::jr, Reg::ra, Reg::zero, Reg::zero, NOVALUE, "");
}

void MipsWithReg::loadIdentifierFromMem(identifier* id, func* this_func, Reg reg) {
	if (id->kind == CONST_IDENTIFIER) {
		this->mipsCode.push_back(objCode(objCode::Instr::li, reg, Reg::zero, Reg::zero, id->getValue(), ""));
	} else if (isTmp(id)) {
		this->mipsCode.push_back(objCode(objCode::Instr::lw, reg, Reg::fp, Reg::zero, id->offset, ""));
	} else if (id->location == GLOBAL_LOCATION) {
		this->mipsCode.push_back(objCode(objCode::Instr::lw, reg, Reg::zero, Reg::zero, NOVALUE, id->name));
	} else {
		this->mipsCode.push_back(objCode(objCode::Instr::lw, reg, Reg::fp, Reg::zero, id->offset, ""));
	}
	if (regPool->allRegPool.count(reg))
		regPool->writeReg(reg);
}

void MipsWithReg::toMips_add(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source1); 
	Reg source2 = regPool->request(code->t2, this_func, this_block, needValue);
	regPool->readReg(source2);
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::add, target, source1, source2, NOVALUE, "");
	regPool->writeReg(target); regPool->setDirty(target);
}

void MipsWithReg::toMips_addi(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source1);
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::addi, target, source1, Reg::zero, code->value, "");
	regPool->writeReg(target);
	regPool->setDirty(target);
}

void MipsWithReg::toMips_sub(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source1);
	Reg source2 = regPool->request(code->t2, this_func, this_block, needValue);
	regPool->readReg(source2);
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::sub, target, source1, source2, NOVALUE, "");
	regPool->writeReg(target); regPool->setDirty(target);
}

void MipsWithReg::toMips_subi(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source1);
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::addi, target, source1, Reg::zero, -1 * code->value, "");
	regPool->writeReg(target);
	regPool->setDirty(target);
}

void MipsWithReg::toMips_subi_reverse(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source);
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::li, Reg::v1, Reg::zero, Reg::zero, code->value, "");	// notice we use $at here
	addObjCode(objCode::Instr::sub, target, Reg::v1, source, NOVALUE, "");
	regPool->writeReg(target);
	regPool->setDirty(target);
}

void MipsWithReg::toMips_mult(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source1);
	Reg source2 = regPool->request(code->t2, this_func, this_block, needValue);
	regPool->readReg(source2);
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::mul, target, source1, source2, NOVALUE, "");
	regPool->writeReg(target); regPool->setDirty(target);
}

void MipsWithReg::toMips_multi(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source);
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::li, Reg::v1, Reg::zero, Reg::zero, code->value, "");
	addObjCode(objCode::Instr::mul, target, source, Reg::v1, NOVALUE, "");
	regPool->writeReg(target);
	regPool->setDirty(target);
}

void MipsWithReg::toMips_div(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source1);
	Reg source2 = regPool->request(code->t2, this_func, this_block, needValue);
	regPool->readReg(source2);
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::div, target, source1, source2, NOVALUE, "");
	regPool->writeReg(target); regPool->setDirty(target);
}

void MipsWithReg::toMips_divi(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source);
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::li, Reg::v1, Reg::zero, Reg::zero, code->value, "");
	addObjCode(objCode::Instr::div, target, source, Reg::v1, NOVALUE, "");

	regPool->writeReg(target);
	regPool->setDirty(target);
}

void MipsWithReg::toMips_divi_reverse(midCode* code, func* this_func, BasicBlock* this_block){
	Reg source = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source);
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::li, Reg::v1, Reg::zero, Reg::zero, code->value, "");
	addObjCode(objCode::Instr::div, target, Reg::v1, source, NOVALUE, "");
	
	regPool->writeReg(target);
	regPool->setDirty(target);
}

void MipsWithReg::toMips_mod(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source1);
	Reg source2 = regPool->request(code->t2, this_func, this_block, needValue);
	regPool->readReg(source2);
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::mod, target, source1, source2, NOVALUE, "");
	regPool->writeReg(target); regPool->setDirty(target);
}

void MipsWithReg::toMips_neg(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source);
	Reg	target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::sub, target, Reg::zero, source, NOVALUE, "");

	regPool->writeReg(target);
	regPool->setDirty(target);
}

void MipsWithReg::toMips_loadInd(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source2 = regPool->request(code->t2, this_func, this_block, needValue);
	regPool->readReg(source2);
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);

	// lw s0, t1.name[s2]
	if (code->t1->location == GLOBAL_LOCATION) {
		addObjCode(objCode::Instr::sll, Reg::v1, source2, Reg::zero, 2, "");
		addObjCode(objCode::Instr::lw, target, Reg::v1, Reg::zero, NOVALUE, code->t1->name);
	} else {
		int offset = code->t1->offset;
		addObjCode(objCode::Instr::sll, Reg::v1, source2, Reg::zero, 2, "");
		addObjCode(objCode::Instr::add, Reg::v1, Reg::v1, Reg::fp, NOVALUE, "");
		addObjCode(objCode::Instr::lw, target, Reg::v1, Reg::zero, offset, "");
	}
	regPool->writeReg(target);
	regPool->setDirty(target);
}

void MipsWithReg::toMips_storeInd(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source2 = regPool->request(code->t2, this_func, this_block, needValue);
	regPool->readReg(source2);
	Reg source1 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source1);
	// t0[t1] = t2
	if (code->t0->location == GLOBAL_LOCATION) {
		addObjCode(objCode::Instr::sll, Reg::v1, source1, Reg::zero, 2, "");
		addObjCode(objCode::Instr::sw, source2, Reg::v1, Reg::zero, NOVALUE, code->t0->name);
	} else {
		int offset = code->t0->offset;
		addObjCode(objCode::Instr::sll, Reg::v1, source1, Reg::zero, 2, "");
		addObjCode(objCode::Instr::add, Reg::v1, Reg::v1, Reg::fp, NOVALUE, "");
		addObjCode(objCode::Instr::sw, source2, Reg::v1, Reg::zero, offset, "");
	}
}

void MipsWithReg::toMips_assign(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source;
	if (code->t1 == NULL) {
		assert(code->label[0] == 'r');
	} else {
		source = regPool->request(code->t1, this_func, this_block, needValue);
		regPool->readReg(source);
	}
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	if (code->t1 == NULL) 
		addObjCode(objCode::Instr::move, target, Reg::v0, Reg::zero, NOVALUE, "");
	else 
		addObjCode(objCode::Instr::move, target, source, Reg::zero, NOVALUE, "");
	regPool->writeReg(target);
	regPool->setDirty(target);
}

void MipsWithReg::toMips_assign_int(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::li, target, Reg::zero, Reg::zero, code->value, "");
	regPool->writeReg(target);
	regPool->setDirty(target);
}

void MipsWithReg::toMips_assign_char(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	addObjCode(objCode::Instr::li, target, Reg::zero, Reg::zero, code->value, "");
	regPool->writeReg(target);
	regPool->setDirty(target);
}

void MipsWithReg::toMips_call(midCode* code, func* this_func, BasicBlock* this_block) {
	// writeBackGlobalVar
	regPool->writeBackGlobalVar();
	
	// save the register
	vector<Reg> needSave;
	needSave.push_back(Reg::ra);
	needSave.push_back(Reg::fp);
	for (Reg reg : regPool->allRegPool) {
		identifier* id = regPool->findIdentifier(reg);
		if (id) {
			if (this_block->out.count(id) > 0) {
				needSave.push_back(reg);
			}
		}
	}
	toMips_save_reg(this_func, this_block, &needSave);
	// TODO: we should save back the global identifier here!

	func* target_func = this->symbol_table->findFunc(code->label);
	this_func->pushNum = this_func->pushNum - target_func->getParamNum();
	this->mipsCode.push_back(objCode(objCode::Instr::addi, Reg::fp, Reg::fp, Reg::zero,
		this_func->getOffsetSum() + this_func->pushNum * 4, ""));

	// call function ----> jump to the funciton 
	this->mipsCode.push_back(objCode(objCode::Instr::jal, Reg::zero, Reg::zero, Reg::zero, NOVALUE, code->label));
	
	// recover the register
	
	toMips_recover_reg(this_func, this_block, &needSave);

}

void MipsWithReg::toMips_bgt(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t0, this_func, this_block, needValue);
	regPool->readReg(source1);
	Reg source2 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source2);
	regPool->clearTemp(this_block);
	addObjCode(objCode::Instr::bgt, source1, source2, Reg::zero, NOVALUE, code->label);
}

void MipsWithReg::toMips_bge(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t0, this_func, this_block, needValue);
	regPool->readReg(source1);
	Reg source2 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source2);
	regPool->clearTemp(this_block);
	addObjCode(objCode::Instr::bge, source1, source2, Reg::zero, NOVALUE, code->label);
}

void MipsWithReg::toMips_blt(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t0, this_func, this_block, needValue);
	regPool->readReg(source1);
	Reg source2 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source2);
	regPool->clearTemp(this_block);
	addObjCode(objCode::Instr::blt, source1, source2, Reg::zero, NOVALUE, code->label);
}

void MipsWithReg::toMips_ble(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t0, this_func, this_block, needValue);
	regPool->readReg(source1);
	Reg source2 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source2);
	regPool->clearTemp(this_block);
	addObjCode(objCode::Instr::ble, source1, source2, Reg::zero, NOVALUE, code->label);
}

void MipsWithReg::toMips_beq(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t0, this_func, this_block, needValue);
	regPool->readReg(source1);
	Reg source2 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source2);
	regPool->clearTemp(this_block);
	addObjCode(objCode::Instr::beq, source1, source2, Reg::zero, NOVALUE, code->label);
}

void MipsWithReg::toMips_bne(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t0, this_func, this_block, needValue);
	regPool->readReg(source1);
	Reg source2 = regPool->request(code->t1, this_func, this_block, needValue);
	regPool->readReg(source2);
	regPool->clearTemp(this_block);
	addObjCode(objCode::Instr::bne, source1, source2, Reg::zero, NOVALUE, code->label);
}

void MipsWithReg::toMips_bz(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t0, this_func, this_block, needValue);
	regPool->readReg(source1);
	regPool->clearTemp(this_block);
	addObjCode(objCode::Instr::beq, source1, Reg::zero, Reg::zero, NOVALUE, code->label);
}

void MipsWithReg::toMips_bnz(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t0, this_func, this_block, needValue);
	regPool->readReg(source1);
	regPool->clearTemp(this_block);
	addObjCode(objCode::Instr::bne, source1, Reg::zero, Reg::zero, NOVALUE, code->label);
}

void MipsWithReg::toMips_input(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg target = regPool->request(code->t0, this_func, this_block, emptyisOk);
	if (code->t0->type == CHAR_IDENTIFIER) {
		// li v0 12
		this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::v0, Reg::zero, Reg::zero, 12, ""));
		// syscall
		this->mipsCode.push_back(objCode(objCode::Instr::syscall, Reg::zero, Reg::zero, Reg::zero, NOVALUE, ""));
		// store
		addObjCode(objCode::Instr::move, target, Reg::v0, Reg::zero, NOVALUE, "");
	} else {
		// li v0 5
		this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::v0, Reg::zero, Reg::zero, 5, ""));
		// syscall
		this->mipsCode.push_back(objCode(objCode::Instr::syscall, Reg::zero, Reg::zero, Reg::zero, NOVALUE, ""));
		// store
		addObjCode(objCode::Instr::move, target, Reg::v0, Reg::zero, NOVALUE, "");
	}
	regPool->writeReg(target);
	regPool->setDirty(target);
}

void MipsWithReg::toMips_output(midCode* code, func* this_func, BasicBlock* this_block) {
	// TODO: we can do a litter opt here, if just output the label, we can add the '\n' to the end of the label
	if (code->label != "") {
		// la a0 label
		this->mipsCode.push_back(objCode(objCode::Instr::la, Reg::a0, Reg::zero, Reg::zero, NOVALUE, code->label));
		// li v0 4
		this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::v0, Reg::zero, Reg::zero, 4, ""));
		// syscall
		this->mipsCode.push_back(objCode(objCode::Instr::syscall, Reg::zero, Reg::zero, Reg::zero, NOVALUE, ""));
	}
	if (code->t0 != NULL) {
		// TODO: 如果有，使用move，如果没有，直接load到a0里面
		Reg source1 = regPool->request(code->t0, this_func, this_block, needValue);
		addObjCode(objCode::Instr::move, Reg::a0, source1, Reg::zero, NOVALUE, "");
		if (code->t0->type == CHAR_IDENTIFIER) this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::v0, Reg::zero, Reg::zero, 11, ""));
		else this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::v0, Reg::zero, Reg::zero, 1, ""));
		// syscall
		this->mipsCode.push_back(objCode(objCode::Instr::syscall, Reg::zero, Reg::zero, Reg::zero, NOVALUE, ""));
	}
	// print('\n')
	// li $a0 10
	this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::a0, Reg::zero, Reg::zero, 10, ""));
	// li $v0 11 
	this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::v0, Reg::zero, Reg::zero, 11, ""));
	// Syscall
	this->mipsCode.push_back(objCode(objCode::Instr::syscall, Reg::zero, Reg::zero, Reg::zero, NOVALUE, ""));
}

void MipsWithReg::toMips_push_reg(Reg reg) {
	this->mipsCode.push_back(objCode(objCode::Instr::sw, reg, Reg::sp, Reg::zero, 0, "0"));
	this->mipsCode.push_back(objCode(objCode::Instr::addi, Reg::sp, Reg::sp, Reg::zero, -4, ""));
}

void MipsWithReg::toMips_pop_reg(Reg reg) {
	this->mipsCode.push_back(objCode(objCode::Instr::addi, Reg::sp, Reg::sp, Reg::zero, 4, ""));
	this->mipsCode.push_back(objCode(objCode::Instr::lw, reg, Reg::sp, Reg::zero, 0, "0"));
}

void MipsWithReg::toMips_push(midCode* code, func* this_func, BasicBlock* this_block) {
	Reg source1 = regPool->request(code->t0, this_func, this_block, needValue);
	regPool->readReg(source1);
	this_func->pushNum++;
	addObjCode(objCode::Instr::sw, source1, Reg::fp, Reg::zero,
		this_func->pushNum * 4 + this_func->getOffsetSum(), "");
}

void MipsWithReg::toMips_func_void(midCode* code, func* this_func) {
	// set the label
	this->mipsCode.push_back(objCode(objCode::Instr::label, Reg::zero, Reg::zero, Reg::zero, NOVALUE, this_func->name));
}

void MipsWithReg::toMips_func_int(midCode* code, func* this_func) {
	// set the label
	this->mipsCode.push_back(objCode(objCode::Instr::label, Reg::zero, Reg::zero, Reg::zero, NOVALUE, this_func->name));
}

void MipsWithReg::toMips_func_char(midCode* code, func* this_func) {
	// set the label
	this->mipsCode.push_back(objCode(objCode::Instr::label, Reg::zero, Reg::zero, Reg::zero, NOVALUE, this_func->name));
}

void MipsWithReg::toMips_save_reg(func* this_func, BasicBlock* this_block, vector<Reg>* needSave) {
	addObjCode(objCode::Instr::addi, Reg::sp, Reg::sp, Reg::zero, -1 * needSave->size() * 4, "");
	for (int index = 0; index < needSave->size(); index++) {
		addObjCode(objCode::Instr::sw, (*needSave)[index], Reg::sp, Reg::zero, index * 4, "");
	}
}

void MipsWithReg::toMips_recover_reg(func* this_func, BasicBlock* this_block, vector<Reg>* needSave) {
	for (int index = 0; index < needSave->size(); index++) {
		addObjCode(objCode::Instr::lw, (*needSave)[index], Reg::sp, Reg::zero, index * 4, "");
	}
	addObjCode(objCode::Instr::addi, Reg::sp, Reg::sp, Reg::zero, needSave->size() * 4, "");
}

void MipsWithReg::toMips_para_int(midCode* code, func* this_func) {
	// now we save the params in the mem, just load is ok
	// if (code->value == 1) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a0, Reg::zero, NOVALUE, ""));
	// else if (code->value == 2) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a1, Reg::zero, NOVALUE, ""));
	// else if (code->value == 3) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a2, Reg::zero, NOVALUE, ""));
	// else if (code->value == 4) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a3, Reg::zero, NOVALUE, ""));
	// else 
	// this->mipsCode.push_back(objCode(objCode::Instr::lw, Reg::s0, Reg::fp, Reg::zero, code->value * 4, ""));
	// toMips_pop_reg(Reg::s0);
	// storeIdentifier(code->t0, this_func, Reg::s0);
}

void MipsWithReg::toMips_para_char(midCode* code, func* this_func) {
	// now we save the params in the mem, just load is ok
	// if (code->value == 1) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a0, Reg::zero, NOVALUE, ""));
	// else if (code->value == 2) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a1, Reg::zero, NOVALUE, ""));
	// else if (code->value == 3) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a2, Reg::zero, NOVALUE, ""));
	// else if (code->value == 4) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a3, Reg::zero, NOVALUE, ""));
	// else 
	// toMips_pop_reg(Reg::s0);

	// storeIdentifier(code->t0, this_func, Reg::s0);
}

void MipsWithReg::genBigSpace(func* this_func) {
	this->mipsCode.push_back(objCode(objCode::Instr::data, Reg::zero, Reg::zero, Reg::zero, 0, ".data"));
	for (identifier* id : this_func->identifierTable.identifierList) {
		if (id->kind == ARRAY_IDENTIFIER) {
			this->mipsCode.push_back(objCode(objCode::Instr::data_identifier, Reg::zero, Reg::zero, Reg::zero,
				(id->array_lenth + 5) * sizeof(int), id->name));
		} else {
			this->mipsCode.push_back(objCode(objCode::Instr::data_identifier, Reg::zero, Reg::zero, Reg::zero, 
				4, id->name));
		}
	}
	for (auto string_iter = this->symbol_table->strToPrint.begin(); string_iter != this->symbol_table->strToPrint.end(); string_iter++) {
		string declare_str = string_iter->first + ": .asciiz " + "\"" + change_backslash(string_iter->second) + "\"";
		this->mipsCode.push_back(objCode(objCode::Instr::data_string, Reg::zero, Reg::zero, Reg::zero, NOVALUE, declare_str));
	}

	this->mipsCode.push_back(objCode(objCode::Instr::data_align, Reg::zero, Reg::zero, Reg::zero, 4, ""));
	this->mipsCode.push_back(objCode(objCode::Instr::data_identifier, Reg::zero, Reg::zero, Reg::zero, 4, "fp"));

	this->mipsCode.push_back(objCode(objCode::Instr::text, Reg::zero, Reg::zero, Reg::zero, NOVALUE, ".text"));
	this->mipsCode.push_back(objCode(objCode::Instr::la, Reg::fp, Reg::zero, Reg::zero, NOVALUE, "fp"));
}

int MipsWithReg::isTmp(identifier* id) {
	if (id->name[0] == '#') {
		int num = 0;
		for (auto str_iter = id->name.begin() + 1; str_iter != id->name.end() && *str_iter <= '9' && *str_iter >= '0'; str_iter++) {
			num = num * 10 + *str_iter - '0';
		}
		assert(num != 0);
		return num;
	} else return 0;
}

void MipsWithReg::output(ofstream& output_file) {
	for (auto iter = mipsCode.begin(); iter != mipsCode.end(); iter++) {
		(*iter).output(output_file);
	}
}

string MipsWithReg::change_backslash(string ori) {
	string res;
	for (auto iter = ori.begin(); iter != ori.end(); iter++) {
		if ((*iter) == '\\') {
			res.append(1, '\\');
		}
		res.append(1, *iter);
	}
	return res;
}

void MipsWithReg::addObjCode(objCode::Instr instr, Reg t0, Reg t1, Reg t2, int value, string str) {
	this->mipsCode.push_back(objCode(instr, t0, t1, t2, value, str));
}

void MipsWithReg::deadDefOptimize() {
	for (func* this_func : symbol_table->funcTable) {
		for (BasicBlock* this_block : flowChartMap[this_func]->blocks) {
			list<midCode*>  newMidCodeList;
			set<identifier*> useSet(this_block->out.begin(), this_block->out.end());

			auto midCodeIter = this_block->midCodeList.end();
			while (midCodeIter != this_block->midCodeList.begin()) {
				midCodeIter--;
				switch ((*midCodeIter)->instr) {
				case(midCode::MidCodeInstr::ADD):
				case(midCode::MidCodeInstr::SUB):
				case(midCode::MidCodeInstr::MULT):
				case(midCode::MidCodeInstr::DIV):
				case(midCode::MidCodeInstr::MOD):
					if (useSet.count((*midCodeIter)->t0) == 0 
						&& (*midCodeIter)->t0->location != GLOBAL_LOCATION) {

						cout << "[DEBUG] deadDef: " << (*midCodeIter)->t0->name << endl;
						// this Code is no use
					} else {
						useSet.erase((*midCodeIter)->t0);
						useSet.insert((*midCodeIter)->t1);
						useSet.insert((*midCodeIter)->t2);
						newMidCodeList.push_front((*midCodeIter));
					}
					break;
				case(midCode::MidCodeInstr::ADDI):
				case(midCode::MidCodeInstr::SUBI):
				case(midCode::MidCodeInstr::MULTI):
				case(midCode::MidCodeInstr::DIVI):
				case(midCode::MidCodeInstr::SUBI_REVERSE):
				case(midCode::MidCodeInstr::DIVI_REVERSE):
				case(midCode::MidCodeInstr::NEG):
					if (useSet.count((*midCodeIter)->t0) == 0 && 
						 (*midCodeIter)->t0->location != GLOBAL_LOCATION) {
						cout << "[DEBUG] deadDef: " << (*midCodeIter)->t0->name << endl;
						// this code is no use
					} else {
						useSet.erase((*midCodeIter)->t0);
						useSet.insert((*midCodeIter)->t1);
						newMidCodeList.push_front((*midCodeIter));
					}
					break;		
				case(midCode::MidCodeInstr::ASSIGN):
					if (useSet.count((*midCodeIter)->t0) == 0 && 
						(*midCodeIter)->t0->location != GLOBAL_LOCATION) {

						cout << "[DEBUG] deadDef: " << (*midCodeIter)->t0->name << endl;
						// this code is no use
					} else {
						useSet.erase((*midCodeIter)->t0);
						if ((*midCodeIter)->t1)
							useSet.insert((*midCodeIter)->t1);
						newMidCodeList.push_front((*midCodeIter));
					}
					break;
				// 1 use
				case(midCode::MidCodeInstr::PUSH):
				case(midCode::MidCodeInstr::RET):		// can be null
				case(midCode::MidCodeInstr::OUTPUT):	// can be null
				case(midCode::MidCodeInstr::BZ):
				case(midCode::MidCodeInstr::BNZ):
					if ((*midCodeIter)->t0) useSet.insert((*midCodeIter)->t0);
					newMidCodeList.push_front((*midCodeIter));
					break;
				// 2 use
				case(midCode::MidCodeInstr::BEQ):
				case(midCode::MidCodeInstr::BNE):
				case(midCode::MidCodeInstr::BGE):
				case(midCode::MidCodeInstr::BGT):
				case(midCode::MidCodeInstr::BLT):
				case(midCode::MidCodeInstr::BLE):
					useSet.insert((*midCodeIter)->t0);
					useSet.insert((*midCodeIter)->t1);
					newMidCodeList.push_front((*midCodeIter));
					break;
				// array
				case(midCode::MidCodeInstr::LOAD_IND):
					if (useSet.count((*midCodeIter)->t0) == 0 && 
						(*midCodeIter)->t0->location != GLOBAL_LOCATION) {
						cout << "[DEBUG] deadDef: " << (*midCodeIter)->t0->name << endl;
						// this code is no use
					} else {
						useSet.erase((*midCodeIter)->t0);
						useSet.insert((*midCodeIter)->t2);
						newMidCodeList.push_front(*midCodeIter);
					}
					break;
				case(midCode::MidCodeInstr::STORE_IND):
					useSet.insert((*midCodeIter)->t2);
					useSet.insert((*midCodeIter)->t1);
					newMidCodeList.push_front(*midCodeIter);
					break;
				default:
					newMidCodeList.push_front(*midCodeIter);
					break;
				}
			}
			vector<midCode*> tempMidCodeList(newMidCodeList.begin(), newMidCodeList.end());
			this_block->midCodeList = tempMidCodeList;

		}
	}
}


void MipsWithReg::assignOptimize() {
	for (func* this_func : symbol_table->funcTable) {
		for (BasicBlock* this_block : flowChartMap[this_func]->blocks) {
			vector<midCode*> newMidCodeList;
			map<identifier*, identifier*> changeMap;
			
			for (auto midCodeIter = this_block->midCodeList.begin();
				midCodeIter != this_block->midCodeList.end(); midCodeIter++) {
				if ((*midCodeIter)->instr == midCode::MidCodeInstr::ASSIGN) {
					bool canChange = true;
					identifier* changeSource = (*midCodeIter)->t1;
					identifier* changeTarget = (*midCodeIter)->t0;
					if (changeSource == NULL) canChange = false;
					else if (this_block->out.count(changeSource)) canChange = false;
					else if (this_block->out.count(changeTarget)) canChange = false;
					else if (changeSource == changeTarget) canChange = false;
					else if (changeSource->kind != changeTarget->kind) canChange = false;
					else if (changeSource->type != changeTarget->type) canChange = false;
					else {
						auto tempIter = midCodeIter;
						tempIter++;
						for (; tempIter != this_block->midCodeList.end(); tempIter++) {
							switch ((*tempIter)->instr) {
								// 1 def 2 use
							case(midCode::MidCodeInstr::ADD):
							case(midCode::MidCodeInstr::SUB):
							case(midCode::MidCodeInstr::MULT):
							case(midCode::MidCodeInstr::DIV):
							case(midCode::MidCodeInstr::MOD):
								// 1 def 1 use
							case(midCode::MidCodeInstr::ADDI):
							case(midCode::MidCodeInstr::SUBI):
							case(midCode::MidCodeInstr::MULTI):
							case(midCode::MidCodeInstr::DIVI):
							case(midCode::MidCodeInstr::SUBI_REVERSE):
							case(midCode::MidCodeInstr::DIVI_REVERSE):
							case(midCode::MidCodeInstr::NEG):
							case(midCode::MidCodeInstr::ASSIGN):
								// 1 def
							case(midCode::MidCodeInstr::ASSIGN_INT):
							case(midCode::MidCodeInstr::ASSIGN_CHAR):
							case(midCode::MidCodeInstr::INPUT):
							case(midCode::MidCodeInstr::LOAD_IND):
								if ((*tempIter)->t0 == changeTarget) {
									canChange = false;
								}
								if ((*tempIter)->t0 == changeSource) {
									canChange = false;
								}
								break;
							default:
								break;
							}
						}
					}

					if (canChange && changeMap.find(changeTarget) == changeMap.end()) {
						if (changeMap.find(changeSource) != changeMap.end()) {
							changeMap[changeTarget] = changeMap[changeSource];
						} else {
							changeMap[changeTarget] = changeSource;
						}
						cout << "[DEBUG] Change " << changeTarget->name << " to " << changeMap[changeTarget]->name << endl;
						continue;
					}
				} 
				bool change = false;
				identifier* t0 = (*midCodeIter)->t0;
				identifier* t1 = (*midCodeIter)->t1;
				identifier* t2 = (*midCodeIter)->t2;
				if (t0 && changeMap.find(t0) != changeMap.end()) {
					t0 = changeMap[t0];
					change = true;
				}
				if (t1 && changeMap.find(t1) != changeMap.end()) {
					t1 = changeMap[t1];
					change = true;
				}
				if (t2 && changeMap.find(t2) != changeMap.end()) {
					t2 = changeMap[t2];
					change = true;
				}
				if (change) {
					newMidCodeList.push_back(
						new midCode(
						(*midCodeIter)->instr,
						t0,t1,t2,
						(*midCodeIter)->label, (*midCodeIter)->value)
					);
				} else {
					newMidCodeList.push_back(*midCodeIter);
				}
			}
			this_block->midCodeList = newMidCodeList;
		}
	}
}