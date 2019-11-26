#ifndef ERROR_HANDLER__H
#define ERROR_HANDLER__H
#include "error.h"

#include<list>
#include<iostream>
#include<fstream>

using namespace std;


class Error_handler {
public:
	Error_handler();
	void raise_error(Error _error);		// 循环引用会导致找不到该类
	void raise_error(int _line_num, ErrorType _error_type);
	void output(string _out_file);
private:
	ofstream outFile;
	list<Error> errorList;
};

#endif // !ERROR_HANDLER__H