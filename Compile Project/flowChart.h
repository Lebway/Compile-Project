#ifndef FLOW_CHART__H
#define FLOW_CHART__H

#include "basicBlock.h"
#include "symbolTable.h"
#include <map>

class FlowChart {
public:
	FlowChart(func* _cur_func) {
		this->curFunction = _cur_func;
		this->endBlock = new BasicBlock();
		foundChart();
	}
	void livenessAnalysis();
	vector<BasicBlock*> blocks;
private:
	void foundChart();
	func* curFunction;
	BasicBlock* endBlock;
};




#endif // !FLOW_CHART__H
