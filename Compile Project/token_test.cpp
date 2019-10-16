#include"tokenize.h"
#include<iostream>
#include<fstream>

using namespace std;

int token_test() {
	Tokenizer tokenizer("testfile.txt");
	ofstream outFile("output.txt");

	while (true) {
		Token this_token = tokenizer.next_token();
		switch (this_token.getType())
		{
		case IDENFR:
			outFile << "IDENFR" << " " << (this_token.getStr()) << endl;
			break;
		case INTCON:
			outFile << "INTCON" << " " << this_token.getNum() << endl;
			break;
		case CHARCON:
			outFile << "CHARCON" << " " << char(this_token.getNum()) << endl;
			break;
		case STRCON:
			outFile << "STRCON" << " " << (this_token.getStr()) << endl;
			break;
		case CONSTTK:
			outFile << "CONSTTK const" << endl;
			break;
		case INTTK:
			outFile << "INTTK int" << endl;
			break;
		case CHARTK:
			outFile << "CHARTK char" << endl;
			break;
		case VOIDTK:
			outFile << "VOIDTK void" << endl;
			break;
		case MAINTK:
			outFile << "MAINTK main" << endl;
			break;
		case IFTK:
			outFile << "IFTK if" << endl;
			break;
		case ELSETK:
			outFile << "ELSETK else" << endl;
			break;
		case DOTK:
			outFile << "DOTK do" << endl;
			break;
		case WHILETK:
			outFile << "WHILETK while" << endl;
			break;
		case FORTK:
			outFile << "FORTK for" << endl;
			break;
		case SCANFTK:
			outFile << "SCANFTK scanf" << endl;
			break;
		case PRINTFTK:
			outFile << "PRINTFTK printf" << endl;
			break;
		case RETURNTK:
			outFile << "RETURNTK return" << endl;
			break;
		case PLUS:
			outFile << "PLUS +" << endl;
			break;
		case MINU:
			outFile << "MINU -" << endl;
			break;
		case MULT:
			outFile << "MULT *" << endl;
			break;
		case DIV:
			outFile << "DIV /" << endl;
			break;
		case LSS:
			outFile << "LSS <" << endl;
			break;
		case LEQ:
			outFile << "LEQ <=" << endl;
			break;
		case GRE:
			outFile << "GRE >" << endl;
			break;
		case GEQ:
			outFile << "GEQ >=" << endl;
			break;
		case EQL:
			outFile << "EQL ==" << endl;
			break;
		case NEQ:
			outFile << "NEQ !=" << endl;
			break;
		case ASSIGN:
			outFile << "ASSIGN =" << endl;
			break;
		case SEMICN:
			outFile << "SEMICN ;" << endl;
			break;
		case COMMA:
			outFile << "COMMA ," << endl;
			break;
		case LPARENT:
			outFile << "LPARENT (" << endl;
			break;
		case RPARENT:
			outFile << "RPARENT )" << endl;
			break;
		case LBRACK:
			outFile << "LBRACK [" << endl;
			break;
		case RBRACK:
			outFile << "RBRACK ]" << endl;
			break;
		case LBRACE:
			outFile << "LBRACE {" << endl;
			break;
		case RBRACE:
			outFile << "RBRACE }" << endl;
			break;
		default:
			break;
		}
		if (this_token.getType() == ENDFILE) {
			outFile.close();
			return 0;
		}
	}
	return 0;
}
