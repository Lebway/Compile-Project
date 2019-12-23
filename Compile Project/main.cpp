#include"test.h"
#include"parser.h"
#include"tokenize.h"
#include"error_handler.h"
#include"error.h"
#include"symbolTable.h"
#include "mips.h"
#include "mipsWithReg.h"
#include "optimize.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
	string input_file = "testfile.txt";
	string output_file = "output.txt";
	string error_file = "error.txt";
	ofstream midCode_output("midCode.txt");
	ofstream mips_output("mips.txt");

	Error_handler* error_handler = new Error_handler();
	Tokenizer tokenizer(input_file, error_handler);
	SymbolTable* symbolTable = new SymbolTable();
	Parser parser(tokenizer.getTokenlist(), error_handler, symbolTable);
	error_handler->output(error_file);
	if (error_handler->getNum()) return 0;
	// symbolTable->midCode_optimize();
	Optimizer optimizer(symbolTable);
	optimizer.optimize();
	// Mips mips(symbolTable);
	MipsWithReg mips(symbolTable);
	// parser.printWholeTree(output_file);
	symbolTable->midCode_output(midCode_output);
	mips.mipsGen();
	mips.output(mips_output);
	

	return 0;
}

/*
Check List:
	+ temp寄存器不写回策略
	+ 找可用的临时寄存器的时候，先进行松弛
	+ 循环改写可能导致const fold出错！因为temp 用了两边
	+ peephole adjace assign中58行可以不需要
	+ inline目前有size限制，可以解除
	+ 块的划分中，函数调用CALL可以不用划分
	+ 目前全局变量不分配全局寄存器，可以为全局变量分配全局寄存器
*/