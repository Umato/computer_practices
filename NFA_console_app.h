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

void print_help_command();
void print_hElp_command();
void cls_command();
void nfa_list_command();
void def_command(const char* command);
void regex_command(const char* input);
void eval_command(const char* command);
void eval2_command(const char* command);
void for_command(const char* command);
void visualize_command(const char* command);
void minimization_command(const char* automaton_name);
void convert_to_dfa_command(const char* automaton_name);
void remove_epsilon_command(const char* automaton_name);

void remove_spaces(char* str);
int nfa_get_priority(char op);
bool handle_operation(nfa_stack* stack, char* operation, NFA_variables* global_structure);
char* NFA_RPN(const char* formula);
NFA* NFA_from_predicate(const char* predicate);
/**
 * @brief Sync structures of nfas (swap coordinates, add new coordinates to nfas). Edit both NFAs and vars
 * Common variables order is based on main_vars order
 */
void sync_nfa_structure(NFA** main_nfa, NFA** sub_nfa, NFA_variables* main_vars, NFA_variables* sub_vars);
/**
 * @brief Union terms in one nfa so it will have as many "y" as number of terms in given list
 * Example: term1 = "y1 = 2a + b", term2 = "y2 = b + a + 3c". Resulting NFA accepts "y1, y2, a, b, c" and checks if they satisfy both terms.
 */
NFA* union_terms(int terms_count, linear_expression** terms, NFA_variables** unioned_vars);
/**
 * @brief Complete all_vars with new variables, reorder local_vars basing on all_vars, extend and reorder coordinates in added_nfa
 * @return: extend_size (how many coordinates were added to added_nfa)
 */
int merge_nfa_and_structure(NFA** added_nfa, NFA_variables* all_vars, NFA_variables* local_vars);

NFA* NFA_from_regex(const char* regex);
void handle_operation_regex(nfa_stack* stack, char op);
int precedence(char op);
// a|b <-> 'a' or 'b', a* <-> '', 'a', 'aa', ...; a+ <-> 'a', 'aa', 'aaa', ...;
// a? <-> '', 'a'; . <-> char != \n
char* regex_to_rpn(const char* regex);
char* explicit_concatenation(const char* regex);


#endif //COMPUTER_PRACTICES_NFA_CONSOLE_APP_H
