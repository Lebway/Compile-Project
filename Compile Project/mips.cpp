#include "mips.h"
#include "objCode.h"

#define NOVALUE 0

// | Operation  | Instruction       | Mips              |
// | ---------- | ----------------- | ----------------- |
// | ADD        | t0 = t1 + t2      | add t0, t1, t2    |
// | ---------- | ----------------- | ----------------- |
// | SUB        | t0 = t1 - t2      | sub t0, t1, t2    |
// | ---------- | ----------------- | ----------------- |
// | MULT       | t0 = t1 * t2      | mul t0, t1, t2    |
// | ---------- | ----------------- | ----------------- |
// | DIV        | t0 = t1 / t2      | div t0, t1, t2    |
// | ---------- | ----------------- | ----------------- |
// | LOAD_IND   | t0 = t1[t2]       | sll t8, t2, 2     |
// |            |                   | add t8, t8, sp    |
// |            |                   | lw t0, t1(t8)     |
// | ---------- | ----------------- | ----------------- |
// | STORE_IND  | t0[t2] = t1       | sll t8, t2, 2     |
// |            |                   | add t8, t8, sp    |
// |            |                   | sw t1, t0(t8)     |
// | ---------- | ----------------- | ----------------- |
// | ASSIGN     | t0 = t1           | move t0, t1       |
// | ---------- | ----------------- | ----------------- |
// | PUSH_ARG   | { push t1 }       | sw a#, #(sp)      |
// | &          | [t0 =] call t3    | sw ra, #(sp)      |
// | CALL       |                   | move a#, t1       |
// |            |                   | [sw t1, #(sp)]    |
// |            |                   | jal t3            |
// |            |                   | lw ra, #(sp)      |
// |            |                   | lw a#, #(sp)      |
// |            |                   | [move t0, v0]     |
// | ---------- | ----------------- | ----------------- |
// | RET        | ret [t1]          | [move v0, t1]     |
// |            |                   | epilogue          |
// |            |                   | jr ra             |
// | ---------- | ----------------- | ----------------- |
// | INPUT      | scanf(t0)         | li v0, 5          |
// |            |                   | syscall           |
// |            |                   | move t0, v0       |
// | ---------- | ----------------- | ----------------- |
// | OUTPUT     | printf(t3, t1)    | move t8, a0       |
// |            |                   | la a0, t3         |
// |            |                   | li v0, 4          |
// |            |                   | syscall           |
// |            |                   | move a0, t1       |
// |            |                   | li v0, 1          |
// |            |                   | syscall           |
// |            |                   | move a0, t8       |
// |            | ----------------- | ----------------- |
// |            | printf(t3)        | move t8, a0       |
// |            |                   | la a0, t3         |
// |            |                   | li v0, 4          |
// |            |                   | syscall           |
// |            |                   | move a0, t8       |
// |            | ----------------- | ----------------- |
// |            | printf(t1)        | move t8, a0       |
// |            |                   | move a0, t1       |
// |            |                   | li v0, 1          |
// |            |                   | syscall           |
// |            |                   | move a0, t8       |
// | ---------- | ----------------- | ----------------- |
// | BGT        | br t3 if t1 > t2  | bgt t1, t2, t3    |
// | ---------- | ----------------- | ----------------- |
// | BGE        | br t3 if t1 >= t2 | bge t1, t2, t3    |
// | ---------- | ----------------- | ----------------- |
// | BLT        | br t3 if t1 < t2  | blt t1, t2, t3    |
// | ---------- | ----------------- | ----------------- |
// | BLE        | br t3 if t1 <= t2 | ble t1, t2, t3    |
// | ---------- | ----------------- | ----------------- |
// | BEQ        | br t3 if t1 == t2 | beq t1, t2, t3    |
// |            | ----------------- | ----------------- |
// |            | br t3 if t1 == 0  | beqz t1, t3       |
// | ---------- | ----------------- | ----------------- |
// | BNE        | br t3 if t1 != t2 | bne t1, t2, t3    |
// |            | ----------------- | ----------------- |
// |            | br t3 if t1 != 0  | bnez t1, t3       |
// | ---------- | ----------------- | ----------------- |
// | GOTO       | goto t3           | j t3              |
// | ---------- | ----------------- | ----------------- |
// | LABEL      | t3:               | t3:               |
// | ---------- | ----------------- | ----------------- |
// 从中间代码生成mips目标代码
// 


Mips::Mips(SymbolTable* _symbol_table) {
	this->symbol_table = _symbol_table;
}

void Mips::mipsGen() {
	list<func>::iterator func_iter;
	list<midCode>::iterator midCode_iter;
	for (func_iter = symbol_table->funcTable.begin(); func_iter!=symbol_table->funcTable.end(); func_iter++) {
		func this_func = *func_iter;
		// 在每个函数的头部， 开辟比较大的空间来存储所有的局部变量
		// 然后每条指令 读取内存 进行计算 输出结果 存回内存
		genBigSpace(&this_func);
		for (midCode_iter = this_func.midCodeList.begin(); midCode_iter != this_func.midCodeList.end(); midCode_iter++) {
			midCode this_midCode = *midCode_iter;
			switch (this_midCode.instr) {
			case(midCode::MidCodeInstr::JUMP):
				toMips_jump(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::LABEL):
				toMips_label(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::RET):
				toMips_ret(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::ADD):
				toMips_add(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::ADDI):
				toMips_addi(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::SUB):
				toMips_sub(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::SUBI):
				toMips_subi(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::MULT):
				toMips_mult(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::DIV):
				toMips_div(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::NEG):
				toMips_neg(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::LOAD_IND):
				toMips_loadInd(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::STORE_IND):
				toMips_storeInd(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::ASSIGN):
				toMips_assign(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::ASSIGN_CHAR):
				toMips_assign_char(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::ASSIGN_INT):
				toMips_assign_int(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::CALL):
				toMips_call(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::BGT):
				toMips_bgt(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::BGE):
				toMips_bge(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::BLT):
				toMips_blt(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::BLE):
				toMips_ble(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::BEQ):
				toMips_beq(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::BNE):
				toMips_bne(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::BZ):
				toMips_bz(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::BNZ):
				toMips_bnz(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::INPUT):
				toMips_input(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::OUTPUT):
				toMips_output(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::PUSH):
				toMips_push(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::FUNC_VOID):
				toMips_func_void(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::FUNC_INT):
				toMips_func_int(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::FUNC_CHAR):
				toMips_func_char(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::PARA_INT):
				toMips_para_int(&this_midCode, &this_func); break;
			case(midCode::MidCodeInstr::PARA_CHAR):
				toMips_para_char(&this_midCode, &this_func); break;
			default:
				break;
			}
		}
	}
}

void Mips::toMips_jump(midCode* code, func* this_func) {
	this->mipsCode.push_back(objCode(objCode::Instr::j, Reg::zero, Reg::zero, Reg::zero, NOVALUE, code->label));
}

void Mips::toMips_label(midCode* code, func* this_func) {
	this->mipsCode.push_back(objCode(objCode::Instr::label, Reg::zero, Reg::zero, Reg::zero, NOVALUE, code->label));
}

void Mips::toMips_ret(midCode* code, func* this_func) {
	// TODO: recover the reg before leave
	if (this_func->name == "main") return;
	if (code->t0 == NULL) this->mipsCode.push_back(objCode(objCode::Instr::jr, Reg::ra, Reg::zero, Reg::zero, NOVALUE, ""));
	else {
		// move to v0
		if (isTmp(code->t0)) {
			// li $t0, tmp*4
			this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::t0, Reg::zero, Reg::zero, isTmp(code->t0)*4, ""));
			// lw $v0, label($t0)
			this->mipsCode.push_back(objCode(objCode::Instr::lw, Reg::v0, Reg::t0, Reg::zero, NOVALUE, this_func->name + "_tmp"));
		} else {
			// lw $v0, variable
			this->mipsCode.push_back(objCode(objCode::Instr::lw, Reg::v0, Reg::zero, Reg::zero, NOVALUE, code->t0->name));
		}
		// recover ra
		toMips_pop_reg(Reg::ra);
		// return 
		this->mipsCode.push_back(objCode(objCode::Instr::jr, Reg::ra, Reg::zero, Reg::zero, NOVALUE, ""));
	}
}

void Mips::loadIdentifier(identifier* id, func* this_func, Reg reg) {
	if (isTmp(id)) {
		this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::t1, Reg::zero, Reg::zero, isTmp(id) * 4, ""));
		this->mipsCode.push_back(objCode(objCode::Instr::lw, reg, Reg::t1, Reg::zero, NOVALUE, this_func->name + "_tmp"));
	} else {
		this->mipsCode.push_back(objCode(objCode::Instr::lw, reg, Reg::zero, Reg::zero, NOVALUE, id->name));
	}
}

void Mips::storeIdentifier(identifier* id, func* this_func, Reg reg) {
	if (isTmp(id)) {
		this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::t1, Reg::zero, Reg::zero, isTmp(id) * 4, ""));
		this->mipsCode.push_back(objCode(objCode::Instr::sw, reg, Reg::t1, Reg::zero, NOVALUE, this_func->name + "_tmp"));
	} else {
		this->mipsCode.push_back(objCode(objCode::Instr::sw, reg, Reg::zero, Reg::zero, NOVALUE, id->name));
	}
}

void Mips::toMips_add(midCode* code, func* this_func) {
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// load s2
	loadIdentifier(code->t2, this_func, Reg::s2);
	// add 
	this->mipsCode.push_back(objCode(objCode::Instr::add, Reg::s0, Reg::s1, Reg::s2, NOVALUE, ""));
	// store s0
	storeIdentifier(code->t0, this_func, Reg::s0);
}

void Mips::toMips_addi(midCode* code, func* this_func) {
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// addi $s0 $s1 100
	this->mipsCode.push_back(objCode(objCode::Instr::addi, Reg::s0, Reg::s1, Reg::zero, code->value, ""));
	// save s0
	storeIdentifier(code->t0, this_func, Reg::s0);
}

void Mips::toMips_sub(midCode* code, func* this_func) {
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// load s2
	loadIdentifier(code->t2, this_func, Reg::s2);
	// sub	
	this->mipsCode.push_back(objCode(objCode::Instr::sub, Reg::s0, Reg::s1, Reg::s2, NOVALUE, ""));
	// store s0
	storeIdentifier(code->t0, this_func, Reg::s0);
}

void Mips::toMips_subi(midCode* code, func* this_func) {
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// subi $t0 $t1 100
	this->mipsCode.push_back(objCode(objCode::Instr::subi, Reg::s0, Reg::s1, Reg::zero, code->value, ""));
	// store s0
	storeIdentifier(code->t0, this_func, Reg::s0);
}

void Mips::toMips_mult(midCode* code, func* this_func) {
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// load s2
	loadIdentifier(code->t2, this_func, Reg::s2);
	// mult
	this->mipsCode.push_back(objCode(objCode::Instr::mul, Reg::s0, Reg::s1, Reg::s2, NOVALUE, ""));
	// store s0
	storeIdentifier(code->t0, this_func, Reg::s0);
}

void Mips::toMips_div(midCode* code, func* this_func) {
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// load s2
	loadIdentifier(code->t2, this_func, Reg::s2);
	// mult
	this->mipsCode.push_back(objCode(objCode::Instr::div, Reg::s0, Reg::s1, Reg::s2, NOVALUE, ""));
	// store s0
	storeIdentifier(code->t0, this_func, Reg::s0);
}

void Mips::toMips_neg(midCode* code, func* this_func) {
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// sub	s0 = 0 - s1
	this->mipsCode.push_back(objCode(objCode::Instr::sub, Reg::s0, Reg::zero, Reg::s1, NOVALUE, ""));
	// store s0
	storeIdentifier(code->t0, this_func, Reg::s0);
}

void Mips::toMips_loadInd(midCode* code, func* this_func) {
	// load s2
	loadIdentifier(code->t2, this_func, Reg::s2);
	// sll s2 s2 2
	this->mipsCode.push_back(objCode(objCode::Instr::sll, Reg::s2, Reg::s2, Reg::zero, 2, ""));
	// lw s0, t1.name[s2]
	this->mipsCode.push_back(objCode(objCode::Instr::lw, Reg::s0, Reg::s2, Reg::zero, NOVALUE, code->t1->name));
	// store s0
	storeIdentifier(code->t0, this_func, Reg::s0);
}

void Mips::toMips_storeInd(midCode* code, func* this_func) {
	// load s2
	loadIdentifier(code->t2, this_func, Reg::s2);
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// sll t1 t1 2
	this->mipsCode.push_back(objCode(objCode::Instr::sll, Reg::s1, Reg::s1, Reg::zero, 2, ""));
	// sw s2 to t0.name[s1]
	this->mipsCode.push_back(objCode(objCode::Instr::sw, Reg::s2, Reg::s1, Reg::zero, NOVALUE, code->t0->name));
}

void Mips::toMips_assign(midCode* code, func* this_func) {
	if (code->t1 == NULL) {
		assert(code->label == "ret");
		storeIdentifier(code->t0, this_func, Reg::v0);
	} else {
		// load s1
		loadIdentifier(code->t1, this_func, Reg::s1);
		// store s1 to t0
		storeIdentifier(code->t0, this_func, Reg::s1);
	}
}

void Mips::toMips_assign_int(midCode* code, func* this_func) {
	// li s1
	this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::s1, Reg::zero, Reg::zero, code->value, ""));
	// store s1 to t0
	storeIdentifier(code->t0, this_func, Reg::s1);
}

void Mips::toMips_assign_char(midCode* code, func* this_func) {
	// li s1
	this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::s1, Reg::zero, Reg::zero, code->value, ""));
	// store s1 to t0
	storeIdentifier(code->t0, this_func, Reg::s1);
}

// TODO: 函数这里还需要改
void Mips::toMips_call(midCode* code, func* this_func) {
	// call function ----> jump to the funciton 
	this->mipsCode.push_back(objCode(objCode::Instr::jal, Reg::zero, Reg::zero, Reg::zero, NOVALUE, code->label));
}

void Mips::toMips_bgt(midCode* code, func* this_func) {
	// bgt t0, t1, label
	// load s0
	loadIdentifier(code->t0, this_func, Reg::s0);
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// branch label
	this->mipsCode.push_back(objCode(objCode::Instr::bgt, Reg::s0, Reg::s1, Reg::zero, NOVALUE, code->label));
}

void Mips::toMips_bge(midCode* code, func* this_func) {
	// bge t0, t1, label
	// load s0
	loadIdentifier(code->t0, this_func, Reg::s0);
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// branch label
	this->mipsCode.push_back(objCode(objCode::Instr::bge, Reg::s0, Reg::s1, Reg::zero, NOVALUE, code->label));
}

void Mips::toMips_blt(midCode* code, func* this_func) {
	// blt t0, t1, label
	// load s0
	loadIdentifier(code->t0, this_func, Reg::s0);
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// branch label
	this->mipsCode.push_back(objCode(objCode::Instr::blt, Reg::s0, Reg::s1, Reg::zero, NOVALUE, code->label));
}

void Mips::toMips_ble(midCode* code, func* this_func) {
	// ble t0, t1, label
	// load s0
	loadIdentifier(code->t0, this_func, Reg::s0);
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// branch label
	this->mipsCode.push_back(objCode(objCode::Instr::ble, Reg::s0, Reg::s1, Reg::zero, NOVALUE, code->label));
}

void Mips::toMips_beq(midCode* code, func* this_func) {
	// beq t0, t1, label
	// load s0
	loadIdentifier(code->t0, this_func, Reg::s0);
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// branch label
	this->mipsCode.push_back(objCode(objCode::Instr::beq, Reg::s0, Reg::s1, Reg::zero, NOVALUE, code->label));
}

void Mips::toMips_bne(midCode* code, func* this_func) {
	// bne t0, t1, label
	// load s0
	loadIdentifier(code->t0, this_func, Reg::s0);
	// load s1
	loadIdentifier(code->t1, this_func, Reg::s1);
	// branch label
	this->mipsCode.push_back(objCode(objCode::Instr::bne, Reg::s0, Reg::s1, Reg::zero, NOVALUE, code->label));
}

void Mips::toMips_bz(midCode* code, func* this_func) {
	// beq t0, 0, label
	// load s0
	loadIdentifier(code->t0, this_func, Reg::s0);
	// branch label
	this->mipsCode.push_back(objCode(objCode::Instr::beq, Reg::s0, Reg::zero, Reg::zero, NOVALUE, code->label));
}

void Mips::toMips_bnz(midCode* code, func* this_func) {
	// bne t0, 0, label
	// load s0
	loadIdentifier(code->t0, this_func, Reg::s0);
	// branch label
	this->mipsCode.push_back(objCode(objCode::Instr::bne, Reg::s0, Reg::zero, Reg::zero, NOVALUE, code->label));
}

void Mips::toMips_input(midCode* code, func* this_func) {
	if (code->t0->type == CHAR_IDENTIFIER) {
		// li v0 12
		this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::v0, Reg::zero, Reg::zero, 12, ""));
		// syscall
		this->mipsCode.push_back(objCode(objCode::Instr::syscall, Reg::zero, Reg::zero, Reg::zero, NOVALUE, ""));
		// store
		storeIdentifier(code->t0, this_func, Reg::v0);
	} else {
		// li v0 5
		this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::v0, Reg::zero, Reg::zero, 5, ""));
		// syscall
		this->mipsCode.push_back(objCode(objCode::Instr::syscall, Reg::zero, Reg::zero, Reg::zero, NOVALUE, ""));
		// store
		storeIdentifier(code->t0, this_func, Reg::v0);
	}
}

void Mips::toMips_output(midCode* code, func* this_func) {
	if (code->label != "") {
		// la a0 label
		this->mipsCode.push_back(objCode(objCode::Instr::la, Reg::a0, Reg::zero, Reg::zero, NOVALUE, code->label));
		// li v0 4
		this->mipsCode.push_back(objCode(objCode::Instr::li, Reg::v0, Reg::zero, Reg::zero, 4, ""));
		// syscall
		this->mipsCode.push_back(objCode(objCode::Instr::syscall, Reg::zero, Reg::zero, Reg::zero, NOVALUE, ""));
	}
	if (code->t0 != NULL) {
		// loadidentifier to a0
		loadIdentifier(code->t0, this_func, Reg::a0);
		// li v0 1 or li v0 11
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

void Mips::toMips_push_reg(Reg reg) {
	this->mipsCode.push_back(objCode(objCode::Instr::sw, reg, Reg::sp, Reg::zero, 0, "0"));
	this->mipsCode.push_back(objCode(objCode::Instr::addi, Reg::sp, Reg::sp, Reg::zero, -4, ""));
}

void Mips::toMips_pop_reg(Reg reg) {
	this->mipsCode.push_back(objCode(objCode::Instr::addi, Reg::sp, Reg::sp, Reg::zero, 4, ""));
	this->mipsCode.push_back(objCode(objCode::Instr::lw, reg, Reg::sp, Reg::zero, 0, "0"));
}

void Mips::toMips_push(midCode* code, func* this_func) {
	// get identifier
	loadIdentifier(code->t0, this_func, Reg::s0);
	if (code->value == 1) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::a0, Reg::s0, Reg::zero, NOVALUE, ""));
	else if (code->value == 2) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::a1, Reg::s0, Reg::zero, NOVALUE, ""));
	else if (code->value == 3) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::a2, Reg::s0, Reg::zero, NOVALUE, ""));
	else if (code->value == 4) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::a3, Reg::s0, Reg::zero, NOVALUE, ""));
	else toMips_push_reg(Reg::s0);
	// sw $ra, 0($sp)
	// addi $sp, $sp, -4
}

void Mips::toMips_func_void(midCode* code, func* this_func) {
	// this->mipsCode.push_back(objCode(objCode::Instr::j, Reg::zero, Reg::zero, Reg::zero, NOVALUE, "main"));
	// set the label
	this->mipsCode.push_back(objCode(objCode::Instr::label, Reg::zero, Reg::zero, Reg::zero, NOVALUE, this_func->name));
	// save the ra
	toMips_push_reg(Reg::ra);
	// TODO:save the reg
}

void Mips::toMips_func_int(midCode* code, func* this_func) {
	this->mipsCode.push_back(objCode(objCode::Instr::j, Reg::zero, Reg::zero, Reg::zero, NOVALUE, "main"));
	// set the label
	this->mipsCode.push_back(objCode(objCode::Instr::label, Reg::zero, Reg::zero, Reg::zero, NOVALUE, this_func->name));
	// save the ra
	toMips_push_reg(Reg::ra);
	// TODO:save the reg
}

void Mips::toMips_func_char(midCode* code, func* this_func) {
	this->mipsCode.push_back(objCode(objCode::Instr::j, Reg::zero, Reg::zero, Reg::zero, NOVALUE, "main"));
	// set the label
	this->mipsCode.push_back(objCode(objCode::Instr::label, Reg::zero, Reg::zero, Reg::zero, NOVALUE, this_func->name));
	// save the ra
	toMips_push_reg(Reg::ra);
	// TODO:save the reg
}

void Mips::toMips_para_int(midCode* code, func* this_func) {
	if (code->value == 1) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a0, Reg::zero, NOVALUE, ""));
	else if (code->value == 2) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a1, Reg::zero, NOVALUE, ""));
	else if (code->value == 3) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a2, Reg::zero, NOVALUE, ""));
	else if (code->value == 4) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a3, Reg::zero, NOVALUE, ""));
	else toMips_pop_reg(Reg::s0);

	storeIdentifier(code->t0, this_func, Reg::s0);
}

void Mips::toMips_para_char(midCode* code, func* this_func) {
	if (code->value == 1) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a0, Reg::zero, NOVALUE, ""));
	else if (code->value == 2) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a1, Reg::zero, NOVALUE, ""));
	else if (code->value == 3) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a2, Reg::zero, NOVALUE, ""));
	else if (code->value == 4) this->mipsCode.push_back(objCode(objCode::Instr::move, Reg::s0, Reg::a3, Reg::zero, NOVALUE, ""));
	else toMips_pop_reg(Reg::s0);

	storeIdentifier(code->t0, this_func, Reg::s0);
}

void Mips::genBigSpace(func* this_func) {
	this->mipsCode.push_back(objCode(objCode::Instr::data, Reg::zero, Reg::zero, Reg::zero, 0, ".data"));
	// space for temp identifier
	this->mipsCode.push_back(objCode(
		objCode::Instr::data_identifier, Reg::zero, Reg::zero, Reg::zero, (this_func->tempIdentifier+10) * 4, this_func->name + "_tmp"));
	// TODO: space for array
	map<string, identifier>::iterator identifier_iter;
	for (identifier_iter = this_func->identifierTable.identifierMap.begin();
		identifier_iter != this_func->identifierTable.identifierMap.end();
		identifier_iter++) {
		// TODO: 解决重名问题
		if ((*identifier_iter).second.kind == ARRAY_IDENTIFIER)
			this->mipsCode.push_back(objCode(objCode::Instr::data_identifier, Reg::zero, Reg::zero, Reg::zero, ((*identifier_iter).second.array_lenth+5) * 4, (*identifier_iter).second.name));
		else   // TODO: change here !
			this->mipsCode.push_back(objCode(objCode::Instr::data_identifier, Reg::zero, Reg::zero, Reg::zero, 4, (*identifier_iter).second.name));
	}
	for (auto string_iter = this_func->strToPrint.begin(); string_iter != this_func->strToPrint.end(); string_iter++) {
		string declare_str = string_iter->first + ": .asciiz " + "\"" + string_iter->second + "\"";
		this->mipsCode.push_back(objCode(objCode::Instr::data_string, Reg::zero, Reg::zero, Reg::zero, NOVALUE, declare_str));
	}
	this->mipsCode.push_back(objCode(objCode::Instr::data_align, Reg::zero, Reg::zero, Reg::zero, 4, ""));
	this->mipsCode.push_back(objCode(objCode::Instr::text, Reg::zero, Reg::zero, Reg::zero, NOVALUE, ""));
}

int Mips::isTmp(identifier* id) {
	if (id->name[0] == '#') {
		int num = 0;
		for (auto str_iter = id->name.begin()+1; str_iter != id->name.end() && *str_iter <= '9' && *str_iter >= '0'; str_iter++) {
			num = num * 10 + *str_iter - '0';
		}
		assert(num!=0);
		return num;
	}
	else return 0;
}

void Mips::output(ofstream& output_file) {
	for (auto iter = mipsCode.begin(); iter != mipsCode.end(); iter++) {
		(*iter).output(output_file);
	}
}

