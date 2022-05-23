#pragma once

#include <string>
#include <queue>
#include <list>
#include <stack>

using std::string;
using StringPair = std::pair<string, string>;
using PairsList = std::list<StringPair>; 


// структура для сохранения индексов разрыва и продолжений цикла
struct LoopLabels {
	std::stack<string> _continues;
	std::stack<string> _breaks;
};

// структура для хранения индексов switch и "break"
struct SwitchLabels {
	std::string _switchIndex;
	bool _defaultFlag;
	std::stack<string> _breaks;
};


// вспомогательный класс для индексов и пар эл-в (...->...)
class CFGpairs {
private:
	unsigned int curInd;					// основной индекс для выражений
	string curIndStr;						// строка текущего индекса
	PairsList pairs;						// список с индексными парами
	std::stack<LoopLabels*> loopstack;		// стек индексов перехода к циклу для вложенных циклов
	std::stack<SwitchLabels*> switchstack;	// стэк индексов switch и break
	PairsList labels;						// список пометочных выражений и их индексов
	PairsList gotos;						// список goto а также ожидающих goto
	bool jumpFlag;							// предыдущее утверждение было прыжком (break, continue, goto или return flag)

public:

	CFGpairs() : curInd(1), curIndStr("1"), jumpFlag(false) {};
	~CFGpairs() {};

	//Основные функции
	inline string getIndex() const { return curIndStr; }					// получить текущий номер индекса в виде строки
	inline string newIndex() { return curIndStr=std::to_string(++curInd); }	// получить увеличенный на 1 индекс в виде строки
	inline bool getJumpFlag() const { return this->jumpFlag; }				// получить флаг прыжка
	inline void resetJumpFlag() { this->jumpFlag = false; }					// флаг прыжка = 0
	string getFlowIndex();								// получить текущий индекс (с коррекцией на возможный дальнейший скачок)
	void addStartIndex();								// добавить стартовую пару в список пар
	void addExitIndex();								// добавить существующую пару в список пар
	void addPair(const string& f, const string& s);					// добавить пару ...->... в список
	void newPair();										// добавьте пару текущего и увеличенного на 1 индексов в список
	
	//// ФУНКЦИИ ПРЫЖКОВ
	void addReturn();									// установить флаг и добавить пару к EXIT
	void addContinue();									// установить флаг и добавить текущий индекс в continues последнего цикла в стеке
	void addBreak();									// установить флаг и добавить текущий индекс к break последнего цикла / switch'а в стеке

	//// GOTO и пометочные функции
	void addLabel(const string& lblname);						// добавить название пометки в список и связать его с будущими goto
	void addGoto(const string& lblname);						// связать индекс goto и существующий индексу пометки или связать с будущими goto
	void linkPendingGoto(const string& lblname);				// связать все будущие индексы goto с текущим индексом пометки
	
	//// функции ЦИКЛОВ
	void pushLoopToStack();								// добавить новый цикл в стэк
	void popLoopFromStack();							// удалить верхний цикл из стэка
	void makeLoopContinuePairs(const string& pair_ind);		// связать все индексы continue с верхним циклом в стэке
	void makeLoopBreakPairs(const string& pair_ind);			// связать все индексы break с верхним циклом в стэке

	//// функции SWITCH
	void pushSwitchToStack();							// добавить switch в стэк
	void popSwitchFromStack();							// убрать верхний switch из стэка
	void addCase();										// связать индекс case с switch
	void addDefault();									// связать индекс default с switch и установить флаг
	bool isDefaultPresent();							// получить флаг из default в текущий switch (в стэке)
	void makeSwitchBreakPairs();						// связать все break в switch'aх с индексом после свича


	string getAllPairs(string code_out) const;							// вернуть все пары в качестве строки

};