#include "flowChart.h"
#include <algorithm>
/*
	+ remove the array and the const variable 
	+ do the back prop
	+ check if have update

	- out[B] = U{in[P}}
	- in[B] = use[B] U (out[B] - def[B])
	 
	- IN[Exit]={};
	- for(除Exit之外的每个基块B) IN[B]={};
	- while(某个IN值发生了改变)
		- for（除了Exit之外的每个基本块B）
		- update
*/


// TODO: 为什么全局变量要单独分析呢？

void FlowChart::livenessAnalysis() {
	for (BasicBlock* block : this->blocks) {
		block->setDefUse();
	}

	// do the back prop
	// TODO: IN[Exit]={};
	bool update;
	do {
		update = false;
		int index = blocks.size() - 1;
		for (; index >= 0; index--) {
			BasicBlock* block = blocks[index];

			// for (BasicBlock* block : blocks) {
				// out[B] = U{in[P}}
			for (BasicBlock* son_block : block->nextBlocks) {
				// TODO: if exit block continue
				block->out.insert(son_block->in.begin(), son_block->in.end());
			}

			for (identifier* id : block->use) {
				if (block->in.count(id) == 0) update = true;
				block->in.insert(id);
			}

			for (identifier* id : block->out) {
				if (block->def.count(id)) continue;
				if (block->in.count(id) == 0) update = true;
				block->in.insert(id);
			}
			// }
		}
	} while (update);

}