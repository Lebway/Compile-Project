#include"test.h"
#include"parser.h"
#include"tokenize.h"
#include"error_handler.h"
#include"error.h"
#include"symbolTable.h"
#include"mips.h"
#include<iostream>
#include<string>

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
	Mips mips(symbolTable);
	parser.printWholeTree(output_file);
	symbolTable->midCode_output(midCode_output);
	mips.mipsGen();
	mips.output(mips_output);
	error_handler->output(error_file);

	return 0;
}