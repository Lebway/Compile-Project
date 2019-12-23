#ifndef MIPS_WITH_REG__H
#define MIPS_WITH_REG__H

#include <list>
#include <cassert>
#include <map>
#include "reg.h"
#include "registerPool.h"
#include "regRecord.h"
#include "symbolTable.h"
#include "objCode.h"
#include "flowChart.h"

using namespace std;

class MipsWithReg
{
public:
	MipsWithReg(SymbolTable*);
	void mipsGen();
	void output(ofstream&);
private:
	map<func*, FlowChart*> flowChartMap;
	registerPool* regPool;

	void addObjCode(objCode::Instr, Reg, Reg, Reg, int, string);
	SymbolTable* symbol_table;
	list<objCode> mipsCode;

	void toMips_jump(midCode*, func*, BasicBlock*);
	void toMips_label(midCode*, func*);
	void toMips_ret(midCode*, func*);
	void toMips_add(midCode*, func*, BasicBlock*);
	void toMips_addi(midCode*, func*, BasicBlock*);
	void toMips_sub(midCode*, func*, BasicBlock*);
	void toMips_subi(midCode*, func*, BasicBlock*);
	void toMips_subi_reverse(midCode*, func*, BasicBlock*);
	void toMips_mult(midCode*, func*, BasicBlock*);
	void toMips_multi(midCode*, func*, BasicBlock*);
	void toMips_div(midCode*, func*, BasicBlock*);
	void toMips_divi(midCode*, func*, BasicBlock*);
	void toMips_divi_reverse(midCode*, func*, BasicBlock*);
	void toMips_neg(midCode*, func*, BasicBlock*);
	void toMips_loadInd(midCode*, func*, BasicBlock*);
	void toMips_storeInd(midCode*, func*, BasicBlock*);
	void toMips_assign(midCode*, func*, BasicBlock*);
	void toMips_assign_int(midCode*, func*, BasicBlock*);
	void toMips_assign_char(midCode*, func*, BasicBlock*);
	void toMips_call(midCode*, func*, BasicBlock*);
	void toMips_bgt(midCode*, func*, BasicBlock*);
	void toMips_bge(midCode*, func*, BasicBlock*);
	void toMips_blt(midCode*, func*, BasicBlock*);
	void toMips_ble(midCode*, func*, BasicBlock*);
	void toMips_beq(midCode*, func*, BasicBlock*);
	void toMips_bne(midCode*, func*, BasicBlock*);
	void toMips_bz(midCode*, func*, BasicBlock*);
	void toMips_bnz(midCode*, func*, BasicBlock*);
	void toMips_input(midCode*, func*, BasicBlock*);
	void toMips_output(midCode*, func*, BasicBlock*);
	void toMips_push(midCode*, func*, BasicBlock*);
	void toMips_push_reg(Reg);
	void toMips_pop_reg(Reg);
	void toMips_func_void(midCode*, func*);
	void toMips_func_int(midCode*, func*);
	void toMips_func_char(midCode*, func*);
	void toMips_para_int(midCode*, func*);
	void toMips_para_char(midCode*, func*);
	void toMips_save_reg(func*, BasicBlock*, vector<Reg>*);
	void toMips_recover_reg(func*, BasicBlock*, vector<Reg>*);

	void toMips_mod(midCode*, func*, BasicBlock*);



	void genBigSpace(func*);
	void loadIdentifier(identifier*, func*, Reg);
	void loadIdentifierFromMem(identifier*, func*, Reg);
	void storeIdentifier(identifier*, func*, Reg);
	int isTmp(identifier* id);
	string change_backslash(string ori);

	// optimize
	void deadDefOptimize();
	void assignOptimize();

};



#endif // !MIPS_WITH_REG__H
