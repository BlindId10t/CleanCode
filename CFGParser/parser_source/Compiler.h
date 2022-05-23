#include <boost/xpressive/xpressive.hpp>

// использовать эту библиотеку и работу с регулярными выражениями
// для парсинга посоветовали в одной из статей на хабре
//информацию брал отсюда https://theboostcpplibraries.com/boost.regex


using namespace boost::xpressive;
using namespace boost::xpressive::regex_constants;



class GrammarCompiler
{
public:

	GrammarCompiler();
	~GrammarCompiler() {};

		
		//////// КЛЮЧЕВЫЕ СЛОВА
		sregex keywords, correct_name;

		//////// КОММЕНТАРИИ
		sregex lc, mlc, comments, spc;

		//////// ПЕРЕМЕННЫЕ
		sregex namescope, idname, idvar;

		//////// ТИПЫ
		sregex int_sign, int_size, t_int, t_float, t_char, t_bool, t_basic, t_stdtype, t_usertype, t_type;

		//////// УКАЗАТЕЛИ
		sregex t_ref, t_ptr, t_pointer;

		//////// ЧАСТИ ВЕКТОРОВ
		sregex arr_part;

		//////// СИМВОЛЫ
			//// ЦЕЛОЧИСЛЕННЫЕ
			sregex int16, int2, int8, int10, int_lit;
			//// СИМВОЛЬНЫЕ
			sregex chr, chars, char_lit;
			//// ВЕЩЕСТВЕННЫЕ
			sregex float_lit;
			//// ЛОГИЧЕСКИЕ
			sregex bool_lit;

		sregex literal;

		//////// ВЫРАЖЕНИЯ
			//// основные
			sregex primary_expr, primary_expr_pars;
			//// постфиксные
			sregex postfix_oper, postfix_dot_arrow, postfix_sqpars, postfix_func, postfix_expr, postfix_expr_full;
			//// унарные
			sregex new_expr, del_expr, unary_oper, unary_expr, unary_getaddr, unary_expr_full;
			//// многомерные
			sregex multipl_expr, multipl_expr_full;
			//// добавление
			sregex add_expr, add_expr_full;
			//// сдвиг
			sregex shift_expr, shift_expr_full;
			//// отношения
			sregex relat_oper, relat_expr, relat_expr_full;
			//// равенство
			sregex equ_expr, equ_expr_full;
			//// побитовое И
			sregex bw_and_expr, bw_and_expr_full;
			//// побитовое исключающее И
			sregex bw_eor_expr, bw_eor_expr_full;
			//// побитовое исключающее ИЛИ
			sregex bw_ior_expr, bw_ior_expr_full;
			//// И
			sregex log_and_expr, log_and_expr_full;
			//// ИЛИ
			sregex log_or_expr, log_or_expr_full;
			//// условие
			sregex cond_expr, cond_expr_full;
			//// назначающие, равенства и проч.
			sregex assign_oper, assign_expr, assign_expr_full;
			//// постоянные
			sregex const_expr = cond_expr;

		sregex expression, expression_full;

		//////// Объявления
			//// Инициализация
			sregex init_clause, equal_init, init_list, init_list_full, braced_init_list, init_expr_list, initializer;
			//// Объявления
			sregex decl_ptr, decl_id, decl_arr_part, declarator, decl_init, decl_init_full, decl_list, decl_list_full;
			//// Определения функций
			sregex ret_val, param_list, param_list_full;

		sregex decl_qualifier, decl_type, declaration, single_decl, func_def;

		//////// Утверждения
			//// С пометкой (switch, default)
			sregex lbl_label, lbl_case, lbl_def, lbl_stat;
			//// выражения
			sregex expr_stat;
			//// составные
			sregex comp_stat;
			//// выбор (if, if-else, switch)
			sregex condition;
			sregex if_stat, ifelse_stat, switch_stat, select_stat;
			//// циклы (while, do-while, for)
			sregex while_stat, dowhile_stat, for_init_stat, for_stat, iter_stat;
			//// прыжки (break, continue, return)
			sregex jump_return, jump_continue, jump_break, jump_goto, jump_stat;
			//// объявления
			sregex decl_stat;

		sregex statement, stat_space;

		sregex func_def_spc, program_code;

		// Для парсинга
		sregex gl_expr, gl_initializer, gl_declaration, gl_statement;
};
