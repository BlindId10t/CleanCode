#include "CFGPairs.h"
#include "Parser.h"
#include <algorithm>
#include <iostream>
#include <fstream>

std::string CFGpairs::getFlowIndex() {
	// Если предыдущее было прыжком
	if (jumpFlag) {
		jumpFlag = false;
		return newIndex();
	}
	
	return curIndStr;
}

void CFGpairs::addStartIndex() {
	pairs.push_back({ "START", "1" }); 
}

void CFGpairs::addExitIndex() {
	if (!jumpFlag) pairs.push_back({ curIndStr, "EXIT" }); 
}

void CFGpairs::addPair(const std::string& first, const std::string& second) {
	if (first != second) pairs.push_back({ first, second }); 
}

void CFGpairs::newPair() {
	std::string oldindex = curIndStr;
	pairs.push_back({ oldindex, newIndex() }); 
}

void CFGpairs::addReturn() {
	jumpFlag = true;
	addPair(curIndStr, std::string("EXIT"));
}

void CFGpairs::addContinue() {
	jumpFlag = true;

	if (!loopstack.empty()) {
		(loopstack.top())->_continues.push(curIndStr);
	}
}

void CFGpairs::addBreak() {
	jumpFlag = true;
	
	// если break в области switch
	if (!switchstack.empty()) {
		(switchstack.top())->_breaks.push(curIndStr);
	}
	else { // иначе в области петли
		if (!loopstack.empty()) {
			(loopstack.top())->_breaks.push(curIndStr);
		}
	}
}

void CFGpairs::addLabel (const std::string& lblname) {
	// добавить в список пометок
	labels.push_back({ lblname, curIndStr });
	// связать с будущим goto
	linkPendingGoto(lblname);
}

void CFGpairs::addGoto (const std::string& lblname) {
	jumpFlag = true;
	
	//найти имя пометки в списке
	PairsList::iterator iter = std::find_if(labels.begin(), labels.end(), [lblname] (StringPair & m) { return  lblname == m.first; });

	//если нашел
	if (iter != labels.end()) {
		this->addPair(curIndStr, (*iter).second); //создать пару с индексом goto и индексом пометки
	}
	else { // добавить пометку будущего goto для последующего связывания с пометкой
		this->gotos.push_back({ lblname, curIndStr });
	}
}

void CFGpairs::linkPendingGoto(const std::string& lblname) {
	if (!gotos.empty()) {
        //для каждого имени пометки в списке будущих goto
		for (PairsList::iterator iter = gotos.begin(); iter != gotos.end(); ++iter) {
			if ((*iter).first == lblname) {
				addPair((*iter).second, curIndStr);		//добавить пару для индекса перехода и индекса пометки
				iter = gotos.erase(iter);				//удалить будущий переход из списка
				
				if (iter == gotos.end()) break;
			}
		}
	}		
}

void CFGpairs::pushLoopToStack() {
	loopstack.push(new LoopLabels());
}

void CFGpairs::popLoopFromStack() {
	if (!loopstack.empty()) {
		delete loopstack.top();
		loopstack.pop();
	}
}

void CFGpairs::makeLoopContinuePairs(const std::string& pair_ind) {
	LoopLabels* currentloop = loopstack.top();
	while ( !currentloop->_continues.empty() ) {
		this->addPair(currentloop->_continues.top(), pair_ind);
		currentloop->_continues.pop();
	}
}

void CFGpairs::makeLoopBreakPairs(const std::string& pair_ind) {
	LoopLabels* currentloop = loopstack.top();
	while ( !currentloop->_breaks.empty() ) {
		this->addPair(currentloop->_breaks.top(), pair_ind);
		currentloop->_breaks.pop();
	}
}

void CFGpairs::pushSwitchToStack() {
	switchstack.push( new SwitchLabels({ this->getIndex(), false }) );
	this->jumpFlag = true;
}

void CFGpairs::popSwitchFromStack() {
	if (!switchstack.empty()) {
		delete switchstack.top();
		switchstack.pop();
	}
}

void CFGpairs::addCase() {
	if (!switchstack.empty()) {
		this->addPair(switchstack.top()->_switchIndex, this->getIndex());
	}
}

void CFGpairs::addDefault() {
	if (!switchstack.empty()) {
		this->addPair(switchstack.top()->_switchIndex, this->getIndex());
		switchstack.top()->_defaultFlag = true;
	}
}

bool CFGpairs::isDefaultPresent() {
	return switchstack.empty() ? false : switchstack.top()->_defaultFlag;
}

void CFGpairs::makeSwitchBreakPairs() {
	SwitchLabels* currentswitch = switchstack.top();
	while ( !currentswitch->_breaks.empty() ) {
		this->addPair(currentswitch->_breaks.top(), this->getIndex());
		currentswitch->_breaks.pop();
	}
}

std::string CFGpairs::getAllPairs(string code_out) const {
	std::string result = "";
    std::string graph = "";

	for (PairsList::const_iterator iter = pairs.begin(); iter != pairs.end(); iter++) {
        result += (*iter).first + " -> " + (*iter).second + ";\n";
        graph += (*iter).first + "->" + (*iter).second + ";\n";
	}

    std::ofstream Fout;
    Fout.open("CFG.gv");
    Fout << "digraph G {\n";
    Fout << graph;
    Fout << "\n}";
    Fout.close();
    system("dot -v -Tpng CFG.gv -o CFG.png"); //пишет в консоли
    std::cout << "\n";

    std::cout << "\nCFG PAIRS:\n\n";
	return result;
}

/*
CleanPass()
for	each	block	i,	in	postorder
    if	i ends	in	a	branch	then
        if	both	targets	are	idenHcal	then
            rewrite	with	a	jump
    if	i ends	in	a	jump	to	j	then
        if	i		is	empty	then
        merge	i	with	j
        else	if	j	has	only	one	predecessor
            merge	i	with	j
        else	if	j	is	empty	&	j	has	a	branch	then
            rewrite	i’s	jump	with	j’s	branch

            Clean()
unHl	CFG	stops	changing
compute	postorder
CleanPass()
*/
