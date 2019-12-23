#include "midCode.h"
#include "symbolTable.h"
#include <list>
#ifndef OPTIMIZE__H
#define OPTIMIZE__H

class Optimizer {
public:
	Optimizer(SymbolTable* _symbol_table) {
		this->symbolTable = _symbol_table;
	};
	void optimize(); 
private:
	SymbolTable* symbolTable;
	static list<midCode*> constFold(list<midCode*>);
	void inlineOptimize();
	void peepHoleOptimize();

	void adjacentAssign();
	void adjacentJump();
	void branchChange();
	void modPeepHole();

	void clean();	// clean the code
};




#endif // !OPTIMIZE__H
