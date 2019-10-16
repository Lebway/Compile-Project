#include"test.h"
#include"parser.h"
#include"tokenize.h"
#include<iostream>

using namespace std;

int main()
{
	Tokenizer tokenizer("testfile.txt");
	Parser parser(tokenizer.getTokenlist());
	parser.printWholeTree("output.txt");
	return 0;
}