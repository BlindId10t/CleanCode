#pragma once

#include <string>
#include "Compiler.h"
#include "CFGPairs.h"

using std::string;


class Parser
{
private:
	GrammarCompiler* gc;			//указатель на объект с грамматикой компилятора
	CFGpairs cfgpairs;				//вспомогательный класс для индексов и пар эл-в (...->...)
	string code_in;					//введенный код в виде строки
	string code_out;				//итоговый код с соответсвующими индексами
	
	string makeIndent(int tabs);	//создание отступов

	// ФУНКЦИИ ПАРСИНГА
	void comp(const string& stat_in, int tabs);			//парсинг составного выражения
	void decl(const string& stat_in, int tabs);			//парсинг объявляющего выражения
	void dowhile(const string& stat_in, int tabs);		//парсинг do-while
	void expr(const string& stat_in, int tabs);			//парсинг выражений (+, - и проч.)
	void forFunc(const string& stat_in, int tabs);			//парсинг for
	void ifFunc(const string& stat_in, int tabs);				//парсинг if
	void jump(const string& stat_in, int tabs);			//парсинг jump
	void lbl(const string& stat_in, int tabs);			//парсинг пометки
	void Stat(const string&  stat_in, int tabs);				//парсинг самого выражения
	void SwitchStat(const string& stat_in, int tabs);			//парсинг switch
	void whileFunc(const string& stat_in, int tabs);			//парсинг while
	void FuncDef(const string& fdef_in);						//парсинг функции

public:

	Parser(GrammarCompiler* gc, string& cppcode_in) : gc(gc), code_in(cppcode_in) {};
	~Parser() {};

	string getCode();		//получить итоговый код с индексами
	string getPairs();		//получить пары эл-в (...->...)

	void ParseCode();		//парсинг полученного кода
};
