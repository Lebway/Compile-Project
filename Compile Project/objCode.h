#ifndef OBJCODE__H
#define OBJCODE__H
#include <list>
#include "reg.h"
#include "symbolTable.h"
#include <cassert>

class objCode
{
public:
	enum class Instr {
		add, addi,
		sub, subi,
		mul, div,
		lw, sw,
		mod,

		bgt, bge,
		blt, ble,
		beq, 
		bne,

		jal, jr, j,

		la, li, move, sll,

		syscall,

		label,
		data,
		data_identifier,
		data_string,
		data_align,
		text,
	};

	Instr instr;
	Reg t0;
	Reg t1;
	Reg t2;
	int value;
	std::string label;

	objCode(Instr, Reg, Reg, Reg, int, std::string);

	void output(ofstream&);
	static string reg2string(Reg);
};

string removeSpace(string _str);


#endif // !OBJCODE__H

