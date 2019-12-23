/*
	+ find if have allocate , if have , return 
	+ get a empty reg
	+ load to it 

	+ clean the use after the store 

*/
#include "regRecord.h"
#include "objCode.h"
#include <cassert>
#include <set>
#define NOVALUE 0

regRecord::regRecord(Reg _reg) {
	this->reg = _reg;

	this->alloced = false;
	this->dirty = false;
	this->write = 0;
	this->read = 0;
	this->function = nullptr;
	this->id = nullptr;
}

// get a reg and set the id&func, notice: need lw to the reg
void regRecord::occupy(identifier* _id, func* _func) {
	this->alloced = true;
	this->dirty = false;
	this->write = 0;
	this->read = 0;
	this->id = _id;
	this->function = _func;
}

void regRecord::release(list<objCode>* _mips_code) {
	bool write_back;
	// check here
	/*
		if (this->id->name[0] == '#' && read >= 1) {
			assert(read == 1);
			write_back = false;
			cout << "[DEBUG] we don't write back 1 reg" << endl;
		} else 
	*/
	
	if (write == 0){
		write_back = false;
	} else if (!dirty) {
		write_back = false;
	} else { //	TODO: other conditions
		write_back = true;
	}

	if (write_back) {
		if (id->location == GLOBAL_LOCATION) {
			_mips_code->push_back(objCode(objCode::Instr::sw, this->reg, Reg::zero, Reg::zero, NOVALUE, id->name));
		} else {
			_mips_code->push_back(objCode(objCode::Instr::sw, this->reg, Reg::fp, Reg::zero, id->offset, ""));
		}
	}

	this->alloced = false;
	this->write = 0;
	this->read = 0;
	this->function = nullptr;
	this->id = nullptr;
}

void regRecord::clear() {
	this->alloced = false;
	this->dirty = false;
	this->write = 0;
	this->read = 0;
	this->function = nullptr;
	this->id = nullptr;
}