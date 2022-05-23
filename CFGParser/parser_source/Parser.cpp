#include "Parser.h"

//примеры работы с boost подсмотрел из https://github.com/openresty/sregex и https://zhyk.org/forum/showthread.php?t=789417

string Parser::makeIndent(int tabs) {
	string indent = "";
	for (int i = 0; i < tabs; i++)
		indent += "\t";

	return indent;
}

string Parser::getCode() {
	return this->code_out;
}

string Parser::getPairs() {
	return cfgpairs.getAllPairs(this->code_out);
}

void Parser::FuncDef(const string& fdef_in) {
	smatch m;
	regex_match(fdef_in, m, gc->func_def);
	this->code_out += m[4].str();

	cfgpairs.addStartIndex();
    comp(m[3].str(), 1);
	cfgpairs.addExitIndex();
	
	this->code_out += "\n\n";
}

void Parser::comp(const string& stat_in, int tabs) {
	string statements = "";
	string indent = makeIndent(tabs-1);	//правильный отступ

	this->code_out += indent + "{\n";
	
	smatch m;
	regex_match(stat_in, m, gc->comp_stat);
	statements = m[1].str();

	//запарси выражения между скобками
	sregex_iterator end;
	for (sregex_iterator iter(cbegin(statements), cend(statements), gc->stat_space); iter != end; iter++) {
		Stat((*iter)[1].str(), tabs);
		this->code_out += "\n";
	}

	this->code_out += indent + "}";
}

void Parser::decl(const string& stat_in, int tabs)
{
	string indent = makeIndent(tabs-1);
	string index = cfgpairs.getFlowIndex();

	this->code_out += indent + "/* " + index + " */ " + stat_in;
}

void Parser::dowhile(const string& stat_in, int tabs)
{
	string indent = makeIndent(tabs-1);
	string index = cfgpairs.getFlowIndex();

	smatch m;
	regex_match(stat_in, m, gc->dowhile_stat);	

	this->code_out += indent + "/* " + index + " */ " + "do\n";
	
	//добавить петлю в стэк
	cfgpairs.pushLoopToStack();

	//добавить пару и создать новый индекс для do-block
	cfgpairs.newPair();
	
	//запарси блок
	Stat(m[1].str(), tabs+1);

	this->code_out += "\n" + indent + "/* " + cfgpairs.getIndex() + " */ " + "while (" + m[2].str() + ");";

	//сделай пары для continues
	cfgpairs.makeLoopContinuePairs(cfgpairs.getIndex());

	if (!cfgpairs.getJumpFlag()) {
		//сохрани индекс последнего выражения
		string dw_last_stat = cfgpairs.getIndex();

		//создайте пару для последнего выражения цикла и начала цикла
		cfgpairs.addPair(dw_last_stat, index);

		//сделай пары для breaks
		cfgpairs.makeLoopBreakPairs(cfgpairs.newIndex());

		//создать пару для последнего выражения while и первого после while
		cfgpairs.addPair(dw_last_stat, cfgpairs.getIndex());			
	}
	else {
		//сделай пары для breaks
		cfgpairs.makeLoopBreakPairs(cfgpairs.newIndex());
		//переустанови флаг для последнего утверждения цикла (петли)
		cfgpairs.resetJumpFlag();
	}

	this->code_out += "\n" + indent + "/* " + cfgpairs.getIndex() + " */";	//добавить пустой блок после do-while

	//достань цикл из стэка
	cfgpairs.popLoopFromStack();		
}

void Parser::expr(const string& stat_in, int tabs)
{
	string indent = makeIndent(tabs-1);
	string index = cfgpairs.getFlowIndex();

	this->code_out += indent + "/* " + index + " */ " + stat_in;
}

void Parser::forFunc(const string& stat_in, int tabs)
{
	string indent = makeIndent(tabs-1);
	string index = cfgpairs.getFlowIndex();
	string forstat = "";

	smatch m;
	regex_match(stat_in, m, gc->for_stat);
	forstat = m[4].str();
	

	this->code_out += indent + "/* " + index + " */ " + m[5].str() + "\n";		//добавь for (...) в итоговый код

	//добавь цикл в стэк
	cfgpairs.pushLoopToStack();

	//добавь новую пару и создай новый индекс для блока for
	cfgpairs.newPair();

	//запарсь выражение в for
	Stat(forstat, tabs+1);

	//сделать пары с continues
	cfgpairs.makeLoopContinuePairs(index);

	//создать пару для последнего утверждения цикла и начала цикла
	if (!cfgpairs.getJumpFlag()) cfgpairs.addPair(cfgpairs.getIndex(), index);
	else cfgpairs.resetJumpFlag();
	
	//сделай пары для break
	cfgpairs.makeLoopBreakPairs(cfgpairs.newIndex());

	//создай пару для начала цикла и утверждения после цикла
	cfgpairs.addPair(index, cfgpairs.getIndex());

	//достать цикл из стэка
	cfgpairs.popLoopFromStack();

	this->code_out += "\n" + indent + "/* " + cfgpairs.getIndex() + " */";	//добавь пустой блок после for
}


void Parser::ifFunc(const string& stat_in, int tabs)
{
	string indent = makeIndent(tabs-1);
	string index = cfgpairs.getFlowIndex();

	smatch m;
	regex_match(stat_in, m, gc->ifelse_stat);

	this->code_out += indent + "/* " + index + " */ " + m[4].str() + "\n";		//добавь if (...) в код

    //добавь пару if
	cfgpairs.newPair();
	Stat(m[2].str(), tabs+1);

	//сохрани индекс и флаг прыжка для последнего утверждения if
	string ifstat_index = cfgpairs.getIndex();
	bool ifstat_jmp = cfgpairs.getJumpFlag();
	cfgpairs.resetJumpFlag();

	//если есть else
	if (m[3].str() != "") {	

		//добавь else
		cfgpairs.addPair(index, cfgpairs.newIndex());
		this->code_out += "\n" + indent + "/* " + cfgpairs.getIndex() + " */ " + "else\n";
		
		//запарси else
		Stat(m[3].str(), tabs+1);

		//сохрани индекс и флаг прыжка для последнего утверждения else
		string elsestat_index = cfgpairs.getIndex();
		bool elsestat_jmp = cfgpairs.getJumpFlag();
		cfgpairs.resetJumpFlag();

		//новый индекс для утверждения после if-else
		cfgpairs.newIndex();

        //добавь пару для последнего утверждения if и утверждением после блока if-else
		if (!ifstat_jmp) cfgpairs.addPair(ifstat_index, cfgpairs.getIndex());
		//add pair for last else-statement and statement after if-else block
        //добавь пару для последнего выражения else и утверждения после блока if-else
		if (!elsestat_jmp) cfgpairs.addPair(elsestat_index, cfgpairs.getIndex());
	} 	
	else {
        //новый индекс для выражения после блока if-then
		cfgpairs.newIndex();

        //добавь пару для последнего выражения if и выражения после блока if-then
		if (!ifstat_jmp) cfgpairs.addPair(ifstat_index, cfgpairs.getIndex());

        //добавь пару для начала if-then и утверждением после блока if-then
		cfgpairs.addPair(index, cfgpairs.getIndex());
	}

	this->code_out += "\n" + indent + "/* " + cfgpairs.getIndex() + " */";	//add empty block after if-else
}


void Parser::jump(const string& stat_in, int tabs)
{
	string indent = makeIndent(tabs-1);
	string index = cfgpairs.getFlowIndex();

	smatch m;
	if (regex_match(stat_in, m, gc->jump_return)) {
		cfgpairs.addReturn();
	}
	else if (regex_match(stat_in, gc->jump_continue)) {
		cfgpairs.addContinue();
	}
	else if (regex_match(stat_in, gc->jump_break)) {
		cfgpairs.addBreak();
	}
	else if (regex_match(stat_in, m, gc->jump_goto)) {
		cfgpairs.addGoto(m[1].str());	// m[1] - имя пометки
	}

	this->code_out += indent + "/* " + index + " */ " + stat_in;
}


void Parser::lbl(const string& stat_in, int tabs)
{
	string indent = makeIndent(tabs-1);
	string index = cfgpairs.getIndex();	
	string newindex = cfgpairs.newIndex();

    //создай пару с предыдущим выражением, если это не прыжок
	if (!cfgpairs.getJumpFlag()) cfgpairs.addPair(index, newindex);
	else cfgpairs.resetJumpFlag();

	smatch m;
	if (regex_match(stat_in, m, gc->lbl_case)) {		//если case
		this->code_out += indent + "/* " + newindex + " */ " + "case " + m[1].str() + ":\n";

        //свяжи со switch
		cfgpairs.addCase();

        //запарси выражения после case
		Stat(m[2].str(), tabs);
	}
	else if (regex_match(stat_in, m, gc->lbl_def)) {		//если default
		this->code_out += indent + "/* " + newindex + " */ " + "default:\n";

        //свяжи со switch
		cfgpairs.addDefault();

        //запарси выражения после default
		Stat(m[1].str(), tabs);
	}
	else if (regex_match(stat_in, m, gc->lbl_label)) {
        //добавь имя пометки и ее новый индекс в список
		cfgpairs.addLabel(m[1].str());
		
		this->code_out += indent + "/* " + newindex + " */ " + m[1].str() + ":\n";
				
		//запарсь остальное
		Stat(m[2].str(), tabs);
	}
}

void Parser::Stat(const string& stat_in, int tabs)
{
	if (stat_in != "") {
		if (regex_match(stat_in, gc->decl_stat)) {
            decl(stat_in, tabs);
		}
		else if (regex_match(stat_in, gc->expr_stat)) {
            expr(stat_in, tabs);
		}
		else if (regex_match(stat_in, gc->comp_stat)) {
            comp(stat_in, tabs);
		}
		else if (regex_match(stat_in, gc->while_stat)) {
            whileFunc(stat_in, tabs);
		}
		else if (regex_match(stat_in, gc->dowhile_stat)) {
            dowhile(stat_in, tabs);
		}
		else if (regex_match(stat_in, gc->for_stat)) {
            forFunc(stat_in, tabs);
		}
		else if (regex_match(stat_in, gc->ifelse_stat)) {
            ifFunc(stat_in, tabs);
		}
		else if (regex_match(stat_in, gc->switch_stat)) {
			SwitchStat(stat_in, tabs);
		}
		else if (regex_match(stat_in, gc->lbl_stat)) {
            lbl(stat_in, tabs);
		}
		else if (regex_match(stat_in, gc->jump_stat)) {
            jump(stat_in, tabs);
		}
	}
}


void Parser::SwitchStat(const string& stat_in, int tabs)
{
	string indent = makeIndent(tabs-1);
	string index = cfgpairs.getFlowIndex();

	smatch m;
	regex_match(stat_in, m, gc->switch_stat);

	this->code_out += indent + "/* " + index + " */ " + m[3].str() + "\n";		//добавь switch (...) в итоговый код

	//добавь switch в стэк
	cfgpairs.pushSwitchToStack();

	//запарси выражения в switch
	Stat(m[2].str(), tabs+1);

	//свяжи последнее утверждение switch с первым после switch
	if (!cfgpairs.getJumpFlag()) {
		cfgpairs.newPair();
	}
	else {
		cfgpairs.newIndex();
		cfgpairs.resetJumpFlag();
	}

	//make pairs for switch "breaks" and statement after switch
    //сделай пары для break в witch и выражением после switch
	cfgpairs.makeSwitchBreakPairs();

	// свяжи начало switch и выражение после switch
	if (!cfgpairs.isDefaultPresent()) {
		cfgpairs.addPair(index, cfgpairs.getIndex());
	}

	//достань switch из стэка
	cfgpairs.popSwitchFromStack();

    //добавь пустой индекс в выражение после switch
	this->code_out += "\n" + indent + "/* " + cfgpairs.getIndex() + " */";
}

void Parser::whileFunc(const string& stat_in, int tabs)
{
	string indent = makeIndent(tabs-1);
	string index = cfgpairs.getFlowIndex();

	smatch m;
	regex_match(stat_in, m, gc->while_stat);

	this->code_out += indent + "/* " + index + " */ " + m[3].str() + "\n";		//добавь while (...) в итоговый код

    //добавь цикл в стэк
	cfgpairs.pushLoopToStack();

    //добавь новую пару для начала и while
	cfgpairs.newPair();

	//запарси выражения в while
	Stat(m[2].str(), tabs+1);

	//создай пару с continues
	cfgpairs.makeLoopContinuePairs(index);

    //добавь пару для последнего утверждения петли и начала петли
	if (!cfgpairs.getJumpFlag()) cfgpairs.addPair(cfgpairs.getIndex(), index);
	else cfgpairs.resetJumpFlag();

	//добавь пару для breaks
	cfgpairs.makeLoopBreakPairs(cfgpairs.newIndex());

    //добавь пару для начала цикла и утверждением после цикла
	cfgpairs.addPair(index, cfgpairs.getIndex());

	//достань цикл из стэка
	cfgpairs.popLoopFromStack();

	this->code_out += "\n" + indent + "/* " + cfgpairs.getIndex() + " */";	//добавь пустой блок после while
}

void Parser::ParseCode()
{
	string functions = "";
	string statements = "";

	smatch m;
	if (code_in != "" && code_out == "") {
		if (regex_match(code_in, m, gc->program_code)) {			// увидел функцию
			functions = m[1].str();

			sregex_iterator end;
			for (sregex_iterator iter(cbegin(functions), cend(functions), gc->func_def_spc); iter != end; iter++) {
				FuncDef((*iter)[1].str());
				break; // запарси только первое
			}
		}
		else if (regex_match(code_in, m, gc->gl_statement)) {		// увидел выражение
			statements = m[1].str();
			//запарси все выражения
			sregex_iterator end;
			for (sregex_iterator iter(cbegin(statements), cend(statements), gc->stat_space); iter != end; iter++) {
				Stat((*iter)[1].str(), 1);	//отступ == 1 для коррекции индексов
				this->code_out += "\n";
			}
		}
	}
}