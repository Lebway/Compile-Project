#include "error_handler.h"
#include "error.h"
using namespace std;

Error_handler::Error_handler()
{
	// TODO: init
}

void Error_handler::output(string _out_file) {
	outFile.open(_out_file);
	list<Error>::iterator iter;
	for (iter = this->errorList.begin(); iter != errorList.end(); iter++) {
		outFile << (*iter).toStr() << endl;
	}
}

void Error_handler::raise_error(Error _error) {
	errorList.push_back(_error);
}

void Error_handler::raise_error(int _line_num, ErrorType _error_type) {
	errorList.push_back(Error(_line_num, _error_type));
}