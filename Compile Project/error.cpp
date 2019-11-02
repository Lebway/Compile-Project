#include "error.h"
#include <string>
using namespace std;

Error::Error() {
	lineNum = -1;
	errorType = I_DONT_KNOW_WHAT_ERROR;
}

Error::Error(int _lineNum, ErrorType _errorType) {
	lineNum = _lineNum;
	errorType = _errorType;
}

string Error::toStr() {
	string errorcode = "";
	switch (errorType) {
	case(ILLEGAL_SYMBOL):
		errorcode = "a"; break;
	case(REDEFINITION):
		errorcode = "b"; break;
	case(UNDEFINEDNAME):
		errorcode = "c"; break;
	case(PARAMETER_NUMBER_DONT_MATCH):
		errorcode = "d"; break;
	case(PARAMETER_TYPE_DONT_MATCH):
		errorcode = "e"; break;
	case(ILLEGAL_IF_CONDITION):
		errorcode = "f"; break;
	case(RETURN_IN_NON_RETURN_FUNC):
		errorcode = "g"; break;
	case(NON_RETURN_IN_RETURN_FUNC):
		errorcode = "h"; break;
	case(ILLEGAL_ARRAY_INDEX):
		errorcode = "i"; break;
	case(ASSIGN_TO_CONST):
		errorcode = "j"; break;
	case(MISSING_SEMICN):
		errorcode = "k"; break;
	case(MISSING_RPARENT_RIGHT_SMALL):
		errorcode = "l"; break;
	case(MISSING_RBRACK_RIGHT_MIDDLE):
		errorcode = "m"; break;
	case(MISSING_WHILE_IN_DOWHILE):
		errorcode = "n"; break;
	case(ONLY_INT_CHAR_ASSIGN_CONST):
		errorcode = "o"; break;
	default:
		break;
	}
	return to_string(lineNum) + " " + errorcode;
}