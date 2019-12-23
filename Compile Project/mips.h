#ifndef MIPS__H
#define MIPS__H
#include <list>
#include "reg.h"
#include "symbolTable.h"
#include <cassert>
#include "objCode.h"

using namespace std;

class Mips
{
public:
	Mips(SymbolTable*);
	void mipsGen();
	void output(ofstream &);
private:
	SymbolTable* symbol_table;
	list<objCode> mipsCode;

	void toMips_jump(midCode*, func*);
	void toMips_label(midCode*, func*);
	void toMips_ret(midCode*, func*);
	void toMips_add(midCode*, func*);
	void toMips_addi(midCode*, func*);
	void toMips_sub(midCode*, func*);
	void toMips_subi(midCode*, func*);
	void toMips_subi_reverse(midCode*, func*);
	void toMips_mult(midCode*, func*);
	void toMips_multi(midCode*, func*);
	void toMips_div(midCode*, func*);
	void toMips_divi(midCode*, func*);
	void toMips_divi_reverse(midCode*, func*);
	void toMips_neg(midCode*, func*);
	void toMips_loadInd(midCode*, func*);
	void toMips_storeInd(midCode*, func*);
	void toMips_assign(midCode*, func*);
	void toMips_assign_int(midCode*, func*);
	void toMips_assign_char(midCode*, func*);
	void toMips_call(midCode*, func*);
	void toMips_bgt(midCode*, func*);
	void toMips_bge(midCode*, func*);
	void toMips_blt(midCode*, func*);
	void toMips_ble(midCode*, func*);
	void toMips_beq(midCode*, func*);
	void toMips_bne(midCode*, func*);
	void toMips_bz(midCode*, func*);
	void toMips_bnz(midCode*, func*);
	void toMips_input(midCode*, func*);
	void toMips_output(midCode*, func*);
	void toMips_push(midCode*, func*);
	void toMips_push_reg(Reg);
	void toMips_pop_reg(Reg);
	void toMips_func_void(midCode*, func*);
	void toMips_func_int(midCode*, func*);
	void toMips_func_char(midCode*, func*);
	void toMips_para_int(midCode*, func*);
	void toMips_para_char(midCode*, func*);
	void toMips_save_reg(midCode*, func*);
	void toMips_recover_reg(midCode*, func*);
	
	void genBigSpace(func*);
	void loadIdentifier(identifier*, func*, Reg);
	void storeIdentifier(identifier*, func*, Reg);
	int isTmp(identifier* id);
	string change_backslash(string ori);
};

#endif // !MIPS__H


