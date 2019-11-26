#include "objCode.h"

objCode::objCode(Instr _instr, Reg _t0, Reg _t1, Reg _t2, int _mem, std::string _str) {
	this->instr = _instr;
	this->t0 = _t0;
	this->t1 = _t1;
	this->t2 = _t2;
	this->value = _mem;
	this->label = _str;
}

void objCode::output(ofstream& output_file) {
	switch (this->instr) {
	case(Instr::add):
		output_file << "add " << reg2string(t0) << " " << reg2string(t1) << " " << reg2string(t2) << endl;
		break;
	case(Instr::addi):
		output_file << "addi " << reg2string(t0) << " " << reg2string(t1) << " " << value << endl;
		break;
	case(Instr::sub):
		output_file << "sub " << reg2string(t0) << " " << reg2string(t1) << " " << reg2string(t2) << endl;
		break;
	case(Instr::subi):
		output_file << "addi " << reg2string(t0) << " " << reg2string(t1) << " " << -1 * value << endl;
		break;
	case(Instr::mul):
		output_file << "mul " << reg2string(t0) << " " << reg2string(t1) << " " << reg2string(t2) << endl;
		break;
	case(Instr::div):
		output_file << "div " << reg2string(t1) << " " << reg2string(t2) << endl;
		output_file << "mflo " << reg2string(t0) << endl;
		break;
	case(Instr::lw):
		if (value != 0) {
			output_file << "lw " << reg2string(t0) << " " << value << "(" << reg2string(t1) << ")" << endl;
		} else {
			if (t1 == Reg::zero) {
				output_file << "lw " << reg2string(t0) << " " << label << endl;
			} else {
				output_file << "lw " << reg2string(t0) << " " << label << "(" << reg2string(t1) << ")" << endl;
			}
		}
		break;
	case(Instr::sw) :
		if (value != 0) {
			output_file << "sw " << reg2string(t0) << " " << value << "(" << reg2string(t1) << ")" << endl;
		} else {
			if (t1 == Reg::zero) {
				output_file << "sw " << reg2string(t0) << " " << label << endl;
			} else {
				output_file << "sw " << reg2string(t0) << " " << label << "(" << reg2string(t1) << ")" << endl;
			}
		}
		break;
	case(Instr::bgt):
		output_file << "bgt " << reg2string(t0) << " " << reg2string(t1) << " " << label << endl;
		break;
	case(Instr::bge):
		output_file << "bge " << reg2string(t0) << " " << reg2string(t1) << " " << label << endl;
		break;
	case(Instr::blt):
		output_file << "blt " << reg2string(t0) << " " << reg2string(t1) << " " << label << endl;
		break;
	case(Instr::ble):
		output_file << "ble " << reg2string(t0) << " " << reg2string(t1) << " " << label << endl;
		break;
	case(Instr::beq):
		output_file << "beq " << reg2string(t0) << " " << reg2string(t1) << " " << label << endl;
		break;
	case(Instr::bne):
		output_file << "bne " << reg2string(t0) << " " << reg2string(t1) << " "  << label << endl;
		break;
	case(Instr::jal):
		output_file << "jal " << label << endl;
		break;
	case(Instr::jr):
		output_file << "jr " << reg2string(t0) << endl;
		break;
	case(Instr::j):
		output_file << "j " << label << endl;
		break;
	case(Instr::la) :
		output_file << "la " << reg2string(t0) << " " << label << endl;
		break;
	case(Instr::li):
		output_file << "li " << reg2string(t0) << " " << value << endl;
		break;
	case(Instr::move):
		output_file << "move " << reg2string(t0) << " " << reg2string(t1) << endl;
		break;
	case(Instr::sll):
		output_file << "sll " << reg2string(t0) << " " << reg2string(t1) << " " << value << endl;
		break;
	case(Instr::syscall):
		output_file << "syscall" << endl;
		break;
	case(Instr::label):
		output_file << endl << label << ": " << endl;
		break;
	case(Instr::data):
		output_file << endl << ".data" << endl;
		break;
	case(Instr::data_identifier):
		// array: .space 128
		output_file << '\t' << label << ": .space " << value << endl;
		break;
	case(Instr::data_string) :
		// string:  .asciiz  "nihao"
		output_file << '\t' << label << endl;
		break;
	case(Instr::data_align):
		output_file << '\t' << ".align " << value << endl;
		break;
	case(Instr::text):
		output_file << ".text" << endl << endl;
		break;
	default:
		assert(0);
	}
}



string objCode::reg2string(Reg reg) {
	switch (reg) {
	case Reg::zero:
		return "$zero";
	case Reg::at:
		return "$at";
	case Reg::v0:
		return "$v0";
	case Reg::v1:
		return "$v1";
	case Reg::a0:
		return "$a0";
	case Reg::a1:
		return "$a1";
	case Reg::a2:
		return "$a2";
	case Reg::a3:
		return "$a3";
	case Reg::s0:
		return "$s0";
	case Reg::s1:
		return "$s1";
	case Reg::s2:
		return "$s2";
	case Reg::s3:
		return "$s3";
	case Reg::s4:
		return "$s4";
	case Reg::s5:
		return "$s5";
	case Reg::s6:
		return "$s6";
	case Reg::s7:
		return "$s7";
	case Reg::t0:
		return "$t0";
	case Reg::t1:
		return "$t1";
	case Reg::t2:
		return "$t2";
	case Reg::t3:
		return "$t3";
	case Reg::t4:
		return "$t4";
	case Reg::t5:
		return "$t5";
	case Reg::t6:
		return "$t6";
	case Reg::t7:
		return "$t7";
	case Reg::t8:
		return "$t8";
	case Reg::t9:
		return "$t9";
	case Reg::gp:
		return "$gp";
	case Reg::sp:
		return "$sp";
	case Reg::fp:
		return "$fp";
	case Reg::ra:
		return "$ra";
	default:
		assert(0);
	}
	return "";
}




/*
string removeSpace(string _str) {
	while (1) {
		size_t pos = _str.find(" ");
		if (pos == string::npos) break;
		_str.erase(pos, 1);
	}
	while (1) {
		size_t pos = _str.find("\t");
		if (pos == string::npos) break;
		_str.erase(pos, 1);
	}
	while (1) {
		size_t pos = _str.find("\n");
		if (pos == string::npos) break;
		_str.erase(pos, 1);
	}
	return "_"+_str;
}*/