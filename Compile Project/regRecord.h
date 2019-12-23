#ifndef REGISTER__H
#define REGISTER__H

#include "reg.h"
#include "symbolTable.h"
#include "objCode.h"
#include "basicBlock.h"
#include <map>

class regRecord {
public:
	regRecord(Reg);
	void occupy(identifier*, func*);
	void release(list<objCode>*);
	void clear();

	identifier* id;
	bool alloced;
	bool dirty;
	int write;
	int read;
	func* function;
private:
	Reg reg;
};

#endif // !REGISTER__H
