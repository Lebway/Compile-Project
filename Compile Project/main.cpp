#include"test.h"
#include"parser.h"
#include"tokenize.h"
#include"error_handler.h"
#include"error.h"
#include<iostream>
#include<string>

using namespace std;

int main()
{
	string input_file = "testfile.txt";
	string output_file = "output.txt";
	string error_file = "error.txt";

	Error_handler* error_handler = new Error_handler();
	Tokenizer tokenizer(input_file, error_handler);
	Parser parser(tokenizer.getTokenlist(), error_handler);
	parser.printWholeTree(output_file);
	error_handler->output(error_file);

	return 0;
}