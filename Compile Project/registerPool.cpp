#include "registerPool.h"
#include <algorithm>


registerPool::registerPool(list<objCode>* _mips_code) {
	this->mipscode = _mips_code;

	paraRegPool.insert(Reg::a0);
	
	// paraRegPool.insert(Reg::a1);
	// paraRegPool.insert(Reg::a1);
	// paraRegPool.insert(Reg::a3);

	globRegPool.insert(Reg::s0);
	globRegPool.insert(Reg::s1);
	globRegPool.insert(Reg::s2);
	globRegPool.insert(Reg::s3);
	globRegPool.insert(Reg::s4);
	globRegPool.insert(Reg::s5);
	globRegPool.insert(Reg::s6);
	globRegPool.insert(Reg::s7);

	tempRegPool.insert(Reg::t0);
	tempRegPool.insert(Reg::t1);
	tempRegPool.insert(Reg::t2);
	tempRegPool.insert(Reg::t3);
	tempRegPool.insert(Reg::t4);
	tempRegPool.insert(Reg::t5);
	tempRegPool.insert(Reg::t6);
	tempRegPool.insert(Reg::t7);
	tempRegPool.insert(Reg::t8);
	tempRegPool.insert(Reg::t9);
	tempRegPool.insert(Reg::k0);

	globRegPool.insert(Reg::k1);
	globRegPool.insert(Reg::a1);
	globRegPool.insert(Reg::a1);
	globRegPool.insert(Reg::a3);

	// merge all the reg to the tempRegPool
	// tempRegPool.insert(globRegPool.begin(), globRegPool.end());

	allRegPool.insert(tempRegPool.begin(), tempRegPool.end());
	allRegPool.insert(globRegPool.begin(), globRegPool.end());

	for (Reg reg : tempRegPool)
		recordMap[reg] = new regRecord(reg);

	for (Reg reg : globRegPool)
		recordMap[reg] = new regRecord(reg);

	for (Reg reg : paraRegPool)
		recordMap[reg] = new regRecord(reg);
}

// id 2 reg
Reg registerPool::findReg(identifier* _id) {
	for (auto record_iter : recordMap) {
		if (record_iter.second->id == _id) {
			return record_iter.first;
		}
	}
	return Reg::ILLEGAL_REGISTER;
}

// clean the '#' var which used 2 twice

void registerPool::relaxDIVtemp() {
	for(Reg reg	: tempLRU) {
		if (recordMap[reg]->alloced) 
			if (recordMap[reg]->id->name[0] == '#' && recordMap[reg]->read >= 1) {
				// assert(recordMap[reg]->read == 1);
				recordMap[reg]->clear();
				cout << "[DEBUG] we relax 1 reg" << endl;
				return;
			}
	}
}


// the reg we return is a empty one
Reg registerPool::allocTempReg() {


	for (Reg reg : tempRegPool) {
		if (!recordMap[reg]->alloced) {
			tempLRU.remove(reg);
			tempLRU.push_back(reg);
			return reg;
		}
	}
	
	// relax
	// relaxDIVtemp();

	for (Reg reg : tempRegPool) {
		if (!recordMap[reg]->alloced) {
			tempLRU.remove(reg);
			tempLRU.push_back(reg);
			return reg;
		}
	}

	cout << "[DEBUG] Can not find a empty reg!" << endl;
	// cannot find a empty reg
	Reg lastUsedReg = tempLRU.front();
	tempLRU.pop_front();
	recordMap[lastUsedReg]->release(this->mipscode);
	tempLRU.push_back(lastUsedReg);
	return lastUsedReg;
}

void registerPool::useTempReg(Reg _reg) {
	this->tempLRU.remove(_reg);
	this->tempLRU.push_back(_reg);
}

bool registerPool::isTempId(identifier* _id) {
	return true; // TODO: now the identifier is always temp 
}

Reg registerPool::request(identifier* _id, func* _func, BasicBlock* _block, bool needValue) {
	if (_id->kind == CONST_IDENTIFIER && _id->getValue() == 0) {
		return Reg::zero;
	}	
	Reg reg = this->findReg(_id);
	if (reg != Reg::ILLEGAL_REGISTER) {
		return reg;
	}

	if (isTempId(_id)) {
		reg = this->allocTempReg();
		assert(!recordMap[reg]->alloced);
		recordMap[reg]->occupy(_id, _func);
		if (needValue) {
			if (_id->kind == CONST_IDENTIFIER)
				this->mipscode->push_back(objCode(objCode::Instr::li, reg, Reg::zero, Reg::zero, _id->getValue(), ""));
			else if (_id->location == GLOBAL_LOCATION)
				this->mipscode->push_back(objCode(objCode::Instr::lw, reg, Reg::zero, Reg::zero, 0, _id->name));
			else
				this->mipscode->push_back(objCode(objCode::Instr::lw, reg, Reg::fp, Reg::zero, _id->offset, ""));
		}
	} else {
		// TODO: global var
	}
	assert(reg != Reg::ILLEGAL_REGISTER);
	return reg;
}

void registerPool::readReg(Reg _reg) {
	assert(_reg != Reg::ILLEGAL_REGISTER);
	if (recordMap.find(_reg) == recordMap.end()) return;
	recordMap[_reg]->read++;

	if (tempRegPool.count(_reg) > 0)
		this->useTempReg(_reg);
}

void registerPool::writeReg(Reg _reg) {
	assert(_reg != Reg::ILLEGAL_REGISTER);
	if (recordMap.find(_reg) == recordMap.end()) return;
	recordMap[_reg]->write++;

	if (tempRegPool.count(_reg) > 0) 
		this->useTempReg(_reg);
}

// clear temp before the end of the block
void registerPool::clearTemp(BasicBlock* _block) {
	for (Reg reg : tempRegPool) {
		if (recordMap[reg]->alloced) {
			identifier* id = recordMap[reg]->id;
			if (_block->out.count(id) > 0 || id->location == GLOBAL_LOCATION) {
				recordMap[reg]->release(this->mipscode);
			}
		}
	}
	// clear
	for (Reg reg : tempRegPool)
		recordMap[reg]->clear();
}
/*
void registerPool::saveTemp(BasicBlock* _block) {
	for (Reg reg : tempRegPool) {
		identifier* id = recordMap[reg]->id;
		if (_block->out.count(id) >= 0) {
			assert(id->name[0] != '#');
			saveReg(reg);
		}
	}
}
*/
/*
void registerPool::saveReg(Reg reg) {
	identifier* id = recordMap[reg]->id;
	if (id->name[0] == '#' && recordMap[reg]->read > 0) {
		;
	} else if (id->location == GLOBAL_LOCATION) {
		this->mipscode->push_back(objCode(objCode::Instr::sw, reg, Reg::zero, Reg::zero, NOVALUE, id->name));
	} else {
		this->mipscode->push_back(objCode(objCode::Instr::sw, reg, Reg::fp, Reg::zero, id->offset, ""));
	}
}
*/
void registerPool::setDirty(Reg reg) {
	this->recordMap[reg]->dirty = true;
}

identifier* registerPool::findIdentifier(Reg reg) {
	if (this->recordMap[reg]->alloced) {
		return this->recordMap[reg]->id;
	}
	return NULL;
}

int cmp(const pair<identifier*, int>& pair1, const pair<identifier*, int>& pair2) {
	if (pair1.second > pair2.second) return true;
	else return false;
}

void registerPool::countReference(vector<BasicBlock*>* _blocks, func* _func) {
	this->blocks = _blocks;
	this->curFunction = _func;
	setBlockAcrossVar();
	for (auto midCode_iter = curFunction->midCodeList.begin(); midCode_iter != curFunction->midCodeList.end(); midCode_iter++) {
		checkRefer((*midCode_iter)->t0, midCode_iter);
		checkRefer((*midCode_iter)->t1, midCode_iter);
		checkRefer((*midCode_iter)->t2, midCode_iter);
	}

	vector<pair<identifier*, int> > vec(referCounter.begin(), referCounter.end());
	sort(vec.begin(), vec.end(), cmp);
	int index = 0;
	cout << "====== global register ======" << endl;
	for (Reg reg : globRegPool) {
		if (index == vec.size()) break;
		identifier* tmpId = vec[index].first;
		// we don't give reg to the global variable
		if (tmpId->location != GLOBAL_LOCATION && tmpId->kind != CONST_IDENTIFIER) {
			accrossVarMap[tmpId] = reg;
			cout << "count: " << vec[index].second << " id: " << tmpId->name << endl;
			recordMap[reg]->occupy(tmpId, curFunction);
			if (tmpId->kind == CONST_IDENTIFIER)
				assert(0);
			else if (tmpId->location == GLOBAL_LOCATION)
				this->mipscode->push_back(objCode(objCode::Instr::lw, reg, Reg::zero, Reg::zero, 0, tmpId->name));
			else
				this->mipscode->push_back(objCode(objCode::Instr::lw, reg, Reg::fp, Reg::zero, tmpId->offset, ""));
		}
		index++;
	}
	// cout << "====== end ======" << endl;
	set<Reg> newGlobRegPool;
	int releaseNum = 0;
	for (Reg reg : globRegPool) {
		if (!recordMap[reg]->alloced) {
			releaseNum++;
			tempRegPool.insert(reg);
		} else {
			newGlobRegPool.insert(reg);
		}
	}
	cout << "====== Release " << releaseNum << " Reg " << "======" << endl;
	this->globRegPool = newGlobRegPool;
}

void registerPool::setBlockAcrossVar() {
	for (BasicBlock* block : *this->blocks) {
		this->blockAcrossVar.insert(block->out.begin(), block->out.end());
	}
}

void registerPool::checkRefer(identifier* _id, list<midCode*>::iterator _curMidCode) {
	if (!_id) return;
	if (!blockAcrossVar.count(_id)) return;
	int count = 1;
	set<string> label;
	for (auto tmpIter = this->curFunction->midCodeList.begin(); tmpIter != _curMidCode; tmpIter++) {
		if ((*tmpIter)->instr == midCode::MidCodeInstr::LABEL)
			label.insert((*tmpIter)->label);
	}
	for (auto tmpIter = _curMidCode; tmpIter != this->curFunction->midCodeList.end(); tmpIter++) {
		switch ((*tmpIter)->instr) {
			case (midCode::MidCodeInstr::BGT):
			case (midCode::MidCodeInstr::BGE):
			case (midCode::MidCodeInstr::BLT):
			case (midCode::MidCodeInstr::BLE):
			case (midCode::MidCodeInstr::BEQ):
			case (midCode::MidCodeInstr::BNE):
			case (midCode::MidCodeInstr::JUMP):
				if (label.count((*tmpIter)->label)) {
					count = count * 5;
				}
				break;
			default:
				break;
		}
	}

	if (referCounter.find(_id) == referCounter.end()) {
		referCounter[_id] = count;
	} else {
		referCounter[_id] += count;
	}

}

void registerPool::writeBackGlobalVar() {
	for (Reg reg : allRegPool) {
		if (recordMap[reg]->alloced) {
			identifier* id = recordMap[reg]->id;
			if (id->location == GLOBAL_LOCATION) {
				recordMap[reg]->release(this->mipscode);
			}
		}
	}
}
