#include "optimize.h"

void Optimizer::optimize() { 
	// Const Fold
	
	for (auto func_iter = this->symbolTable->funcTable.begin(); func_iter != this->symbolTable->funcTable.end(); func_iter++) {
		(*func_iter)->midCodeList = Optimizer::constFold((*func_iter)->midCodeList);
	}
	
	// inLine expand
	inlineOptimize();

	// peep hole 
	peepHoleOptimize();
}