
#ifndef REGISTER_POOL__H
#define REGISTER_POOL__H
#include "reg.h"
#include "symbolTable.h"
#include "objCode.h"
#include "basicBlock.h"
#include "regRecord.h"
#include <map>


class registerPool {

public:
	registerPool(list<objCode>*);
	Reg request(identifier*, func*, BasicBlock*, bool);
	void clearTemp(BasicBlock*);
	
	// void saveTemp(BasicBlock*);

	void readReg(Reg);
	void writeReg(Reg);
	void setDirty(Reg);
	Reg findReg(identifier*);
	identifier* findIdentifier(Reg);
	void countReference(vector<BasicBlock*>*, func*);
	void writeBackGlobalVar();
	
	set<Reg> allRegPool;

private:
	bool isTempId(identifier*);
	Reg allocTempReg();
	void useTempReg(Reg);
	void relaxDIVtemp();
	void setBlockAcrossVar();
	void checkRefer(identifier*, list<midCode*>::iterator);
	// void saveReg(Reg);


	list<Reg> tempLRU;
	list<objCode>* mipscode;

	map<identifier*, Reg> accrossVarMap;
	map<Reg, regRecord*> recordMap;
	map<identifier*, int> referCounter;
	vector<BasicBlock*>* blocks;
	func* curFunction;
	set<identifier*> blockAcrossVar;

	set<Reg> paraRegPool;
	set<Reg> globRegPool;
	set<Reg> tempRegPool;
};

#endif // !REGISTER_POOL__H
