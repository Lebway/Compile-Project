#ifndef MID_CODE__H
#define MID_CODE__H

#include <string>
#include "identifier.h"

using namespace std;

class midCode {
public:
	enum class MidCodeInstr
	{
		ADD,		// t0 = t1 + t2
		ADDI,
		SUB,		// t0 = t1 - t2
		SUBI,
		NEG,		// t0 = - t1
		MULT,		// t0 = t1 * t2
		DIV,		// t0 = t1 / t2
		LOAD_IND,	// t0 = t1[t2]
		STORE_IND,	// t0[t1] = t2
		ASSIGN,		// t0 = t1
		ASSIGN_INT,		// t0 = i
		ASSIGN_CHAR,	// t0 ='c'

		PUSH,	// push t1
		CALL,		// t0(nullable) = call t3
		RET,		// return t1(nullable)

		INPUT,		// scanf(t0)
		OUTPUT,		// printf(t3(nullable), t1(nullable)) cannot have both entry being null

		BGT,		// branch to t3 if t1 > t2
		BGE,		// branch to t3 if t1 >= t2
		BLT,		// branch to t3 if t1 < t2
		BLE,		// branch to t3 if t1 <= t2
		BEQ,		// branch to t3 if t1 == t2(nullable)
		BNE,		// branch to t3 if t1 != t2(nullable)
		BZ,			// branch if t1 == zero
		BNZ,		// branch if t1 != zero
		JUMP,		// jump t3
		LABEL,		// lab t3

		FUNC_VOID,
		FUNC_INT,
		FUNC_CHAR,
		PARA_INT,
		PARA_CHAR,
		SAVE_REG,
		RECOVER_REG,
		CONST_INT,
		CONST_CHAR,

		VAR_INT,
		VAR_CHAR,
	};


	MidCodeInstr instr;
	std::string label;
	int value;
	identifier* t0;
	identifier* t1;
	identifier* t2;
	midCode(MidCodeInstr _instr, identifier* t0=NULL, identifier* t1=NULL,
		identifier* t2=NULL, std::string _str="", int _value=0);
	/*midCode(MidCodeInstr _instr, identifier* rd, identifier* rs);
	midCode(MidCodeInstr _instr, identifier* rd);*/
	midCode(MidCodeInstr _instr, identifier*, int, string _str="");
	midCode(MidCodeInstr _instr, identifier*, std::string);
	midCode(MidCodeInstr _instr, identifier*, identifier*, int);
	midCode(MidCodeInstr _instr, std::string _str);
	midCode(MidCodeInstr _instr, int, std::string _str);

	// midCode(MidCodeInstr _instr);

	static std::string genLabel(void);
	void output(ofstream&);
private:
	
};



#endif // !MID_CODE__H