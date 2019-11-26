#include "midCode.h"
#include "parser.h"
#include <cassert>

midCode::midCode(MidCodeInstr _instr, identifier* t0, identifier* t1, identifier* t2, std::string _str, int _value) {
	this->instr = _instr;
	this->t0 = t0;
	this->t1 = t1;
	this->t2 = t2;
	this->label = _str;
	this->value = _value;
}

midCode::midCode(MidCodeInstr _instr, identifier* t0, int _value, string str) {
	this->instr = _instr;
	this->t0 = t0;
	this->t1 = NULL;
	this->t2 = NULL;
	this->label = str;
	this->value = _value;
}

midCode::midCode(MidCodeInstr _instr, identifier* t0, std::string _str) {
	this->instr = _instr;
	this->t0 = t0;
	this->t1 = NULL;
	this->t2 = NULL;
	this->label = _str;
	this->value = 0;
}

midCode::midCode(MidCodeInstr _instr, identifier* t0, identifier* t1, int _value) {
	this->instr = _instr;
	this->t0 = t0;
	this->t1 = t1;
	this->t2 = NULL;
	this->label = "";
	this->value = _value;
}

midCode::midCode(MidCodeInstr _instr, std::string _str) {
	this->instr = _instr;
	this->t0 = NULL;
	this->t1 = NULL;
	this->t2 = NULL;	
	this->label = _str;
	this->value = 0;
}

midCode::midCode(MidCodeInstr _instr, int _value, std::string _str) {
	this->instr = _instr;
	this->t0 = NULL;
	this->t1 = NULL;
	this->t2 = NULL;
	this->label = _str;
	this->value = _value;
}


std::string midCode::genLabel() {
	static int counter = 1;
	return "label_" + std::to_string(counter++);
}

void midCode::output(ofstream& output_handler) {
	switch (this->instr) {
	case(MidCodeInstr::ADD):
		output_handler << t0->name << " = " << t1->name << " + " << t2->name;
		break;
	case(MidCodeInstr::ADDI):
		output_handler << t0->name << " = " << t1->name << " + " << value;
		break;
	case(MidCodeInstr::SUB):
		output_handler << t0->name << " = " << t1->name << " - " << t2->name;
		break;
	case(MidCodeInstr::SUBI):
		output_handler << t0->name << " = " << t1->name << " - " << value;
		break;
	case(MidCodeInstr::NEG):
		output_handler << t0->name << " = " << "-" << t1->name;
		break;
	case(MidCodeInstr::MULT):
		output_handler << t0->name << " = " << t1->name << " * " << t2->name;
		break;
	case(MidCodeInstr::DIV):
		output_handler << t0->name << " = " << t1->name << " / " << t2->name;
		break;
	case(MidCodeInstr::LOAD_IND):
		output_handler << t0->name << " = " << t1->name << "[" << t2->name << "]";
		break;
	case(MidCodeInstr::STORE_IND):
		output_handler << t0->name << "[" << t1->name << "]" <<  " = " << t2->name;
		break;
	case(MidCodeInstr::ASSIGN):
		if(t1) output_handler << t0->name << " = " << t1->name;
		else output_handler << t0->name << " = " << label;
		break;
	case(MidCodeInstr::ASSIGN_INT):
		output_handler << t0->name << " = " << value;
		break;
	case(MidCodeInstr::ASSIGN_CHAR):
		output_handler << t0->name << " = " << '\'' << char(value) << '\'';
		break;
	case(MidCodeInstr::PUSH):
		output_handler << "PUSH " << t0->name;
		break;
	case(MidCodeInstr::CALL):
		output_handler << "CALL " << label;
		break;
	case(MidCodeInstr::RET):
		if(t0==NULL) output_handler << "RET";
		else output_handler << "RET " << t0->name;
		break;
	case(MidCodeInstr::INPUT):
		output_handler << "INPUT " << t0->name;
		break;
	case(MidCodeInstr::OUTPUT):
		if (label != "") output_handler << "OUTPUT " << label;
		if (t0 != NULL)  output_handler << "OUTPUT " << t0->name;
		break;
	case(MidCodeInstr::BGT):
		output_handler << "BGT " << t0->name << " > " << t1->name << " -> " << label;
		break;
	case(MidCodeInstr::BGE):
		output_handler << "BGE " << t0->name << " >= " << t1->name << " -> " << label;
		break;
	case(MidCodeInstr::BLT):
		output_handler << "BLT " << t0->name << " < " << t1->name << " -> " << label;
		break;
	case(MidCodeInstr::BLE):
		output_handler << "BLE " << t0->name << " <= " << t1->name << " -> " << label;
		break;
	case(MidCodeInstr::BEQ):
		output_handler << "BEQ " << t0->name << " == " << t1->name << " -> " << label;
		break;
	case(MidCodeInstr::BNE):
		output_handler << "BNE " << t0->name << " != " << t1->name << " -> " << label;
		break;
	case(MidCodeInstr::BZ):
		output_handler << "BZ " << t0->name << " == " << "0" << " -> " << label;
		break;
	case(MidCodeInstr::BNZ):
		output_handler << "BNZ " << t0->name << " != " << "0" << " -> " << label;
		break;
	case(MidCodeInstr::JUMP):
		output_handler << "JUMP " << label;
		break;
	case(MidCodeInstr::LABEL):
		output_handler << "LABEL: " << label;
		break;
	case(MidCodeInstr::FUNC_VOID):
		output_handler << "VOID_FUNC " << label;
		break;
	case(MidCodeInstr::FUNC_INT):
		output_handler << "INT_FUNC " << label;
		break;
	case(MidCodeInstr::FUNC_CHAR):
		output_handler << "CHAR_FUNC " << label;
		break;
	case(MidCodeInstr::PARA_INT):
		output_handler << "PARA INT " << t0->name;
		break;
	case(MidCodeInstr::PARA_CHAR):
		output_handler << "PARA CHAR " << t0->name;
		break;
	case(MidCodeInstr::VAR_INT):
		output_handler << "VAR INT " << t0->name;
		break;
	case(MidCodeInstr::VAR_CHAR):
		output_handler << "VAR CHAR " << t0->name;
		break;

	default:
		break;
	}
}
