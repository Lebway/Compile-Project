#ifndef BASIC_BLOCK__H
#define BASIC_BLOCK__H

#include<vector>
#include<set>
#include"midCode.h"



class BasicBlock {
public:
	BasicBlock();
	BasicBlock(vector<midCode*>);
	BasicBlock(list<midCode*>);
	BasicBlock(vector<midCode*>::iterator, vector<midCode*>::iterator);
	void addPrevBlock(BasicBlock*);
	void addNextBlock(BasicBlock*);

	vector<midCode*> midCodeList;
	set<BasicBlock*> prevBlocks;
	set<BasicBlock*> nextBlocks;

	void setDefUse();
	set<identifier*> def;
	set<identifier*> use;

	set<identifier*> in;
	set<identifier*> out;
private:
	void checkDef(identifier*);
	void checkUse(identifier*);
};


#endif // !BASIC_BLOCK__h
