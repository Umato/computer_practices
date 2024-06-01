//
// Coded by Zelendzu & Umato on 31.05.2024.
//


#ifndef COMPUTER_PRACTICES_NFA_CONSOLE_APP_H
#define COMPUTER_PRACTICES_NFA_CONSOLE_APP_H

#include "basic_functions.h"

#include "NFA_variables.h"
#include "linear_term.h"
#include "NFA_stack.h"
#include "NFA.h"

void NFA_console_app();
void print_help();
void print_hElp();
void NFA_list();
char* NFA_RPN(const char* formula);
NFA* NFA_from_predicate(const char* predicate);
void NFA_def(const char* command);
void NFA_eval_command(const char* command);
void NFA_for_command(const char* command);
void NFA_eval2_command(const char* command);
void NFA_visualize_command(const char* command);
void handle_minimization(const char* automaton_name);
void handle_conversion_to_dfa(const char* automaton_name);
bool handle_operation(nfa_stack* stack, char* operation, NFA_variables* global_structure);
void handle_remove_epsilon(const char* automaton_name);
void handle_cls();
void sync_nfa_structure(NFA** main_nfa, NFA** sub_nfa, NFA_variables* main_vars, NFA_variables* sub_vars);
NFA* union_terms(int terms_count, linear_expression** terms, NFA_variables** unioned_vars);
int merge_nfa_and_structure(NFA** added_nfa, NFA_variables* all_vars, NFA_variables* local_vars);
void remove_spaces(char* str);

int precedence(char op);
char* explicit_concatenation(const char* regex);
// a|b <-> 'a' or 'b', a* <-> '', 'a', 'aa', ...; a+ <-> 'a', 'aa', 'aaa', ...;
// a? <-> '', 'a'; . <-> char != \n
char* regex_to_rpn(const char* regex);
void handle_regex(const char* input);
NFA* NFA_from_regex(const char* regex);
void handle_operation_regex(nfa_stack* stack, char op);


#endif //COMPUTER_PRACTICES_NFA_CONSOLE_APP_H
