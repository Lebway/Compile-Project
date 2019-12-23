#include "flowChart.h"
#include <cassert>

using namespace std;

void FlowChart::foundChart() {
	vector<midCode*> midCodes;
	// label_name -- midcode index
	map<string, int> label_midCode_map;
	// labelname -- block index
	map<string, int> label_block_map;
	// entrances 
	set<int> entrances;

	for (midCode* tmp : this->curFunction->midCodeList) {
		midCodes.push_back(tmp);
		if (tmp->instr == midCode::MidCodeInstr::LABEL)
			label_midCode_map[tmp->label] = midCodes.size() - 1;
	}

	for (int index = 0; index < midCodes.size(); index++) {
		switch (midCodes[index]->instr) {
		// function call
		case(midCode::MidCodeInstr::PUSH):
		case(midCode::MidCodeInstr::CALL):
			entrances.insert(index);
			while ((midCodes[index])->instr == midCode::MidCodeInstr::PUSH) {
				index++;
			}
			entrances.insert(index + 1);
			break;
		// branch 
		case(midCode::MidCodeInstr::BEQ):
		case(midCode::MidCodeInstr::BNE):
		case(midCode::MidCodeInstr::BGE):
		case(midCode::MidCodeInstr::BGT):
		case(midCode::MidCodeInstr::BLE):
		case(midCode::MidCodeInstr::BLT):
		case(midCode::MidCodeInstr::JUMP):
			entrances.insert(label_midCode_map[(midCodes[index])->label]);
			entrances.insert(index + 1);
			break;
		case(midCode::MidCodeInstr::RET):
			entrances.insert(index + 1);
			break;
		default:
			break;
		}
	}

	entrances.insert(midCodes.size());
	entrances.erase(0);

	int start = 0;
	for (int end : entrances) {
		if (midCodes[start]->instr == midCode::MidCodeInstr::LABEL) {
			label_block_map[midCodes[start]->label] = this->blocks.size();
		}
		blocks.push_back(new BasicBlock(
			midCodes.begin() + start, midCodes.begin() + end
		));
		start = end;
	}

	for (int index = 0; index < blocks.size(); index++) {
		midCode* exitMidCode = blocks[index]->midCodeList.back();
		switch (exitMidCode->instr) {
		case(midCode::MidCodeInstr::RET):
			blocks[index]->addNextBlock(endBlock);
			break;
		case(midCode::MidCodeInstr::JUMP):
			blocks[index]->addNextBlock(blocks[label_block_map[exitMidCode->label]]);
			break;
		case(midCode::MidCodeInstr::BEQ):
		case(midCode::MidCodeInstr::BNE):
		case(midCode::MidCodeInstr::BGE):
		case(midCode::MidCodeInstr::BGT):
		case(midCode::MidCodeInstr::BLE):
		case(midCode::MidCodeInstr::BLT):
			assert(label_block_map.find(exitMidCode->label) != label_block_map.end());
			blocks[index]->addNextBlock(blocks[label_block_map[exitMidCode->label]]);
		default:
			if (index != blocks.size() - 1) {
				blocks[index]->addNextBlock(blocks[index + 1]);
			}
			break;
		}
	}
}

