//#include <Windows.h>

#include <iostream>
#include <fstream>
#include <string>
#include "parser_source/Parser.h"

using namespace std;

int main()
{
    system("dir");
	ifstream file_in("ccode.txt");
	string code = "";

	if (file_in.good()) {
		cout << "File opened\n\n";
		char c;
		while(file_in.get(c)) {
			code += c;
		}
	}

	string codeinp = code;
	GrammarCompiler gc;
	Parser p(&gc, codeinp);
	p.ParseCode();

    cout << "OUTPUT CODE:\n\n" << p.getCode();
    //system("pause");

    cout << p.getPairs();
    //system("pause");

	return 0;
}