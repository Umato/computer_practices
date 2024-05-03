//
// Coded by Zelendzu & Umato on 29.02.2024.
//

#ifndef COMPUTER_PRACTICES_NFA_H
#define COMPUTER_PRACTICES_NFA_H

#include <fstream>
#include <cstdlib>
#include <cstdio>

#include "Algorithm.h"
#include "big_integer.h"
#include "graph.h"
#include <bit>
#include <conio.h>
#include <vector>

#include "direct.h"
#include "windows.h"

typedef struct linear_term {
    int* coefficients;
    int constant;
    int count;
} linear_term;

typedef struct NFA_state {
    int id;
    bool is_final;
    list** transitions;
} NFA_state;

typedef struct NFA {
    int states_count;
    int alphabet_dim;
    NFA_state** states;
    NFA_state* initial_state;
} NFA;

typedef struct nfa_node {
    NFA* nfa;
    struct nfa_node* next;
} nfa_node;

typedef struct nfa_stack {
    nfa_node* top;
} nfa_stack;

list* list_init();
void list_free(list* tr_list);
void add_to_list(list* l, int val);
void print_bin(unsigned number, unsigned int bits);
int get_random_num(int start, int end);
char* format_string_to_bin(const char* string);
char* int_to_string(int num);


NFA_state* NFA_state_init(int id, bool is_final, int alphabet_dim);
void NFA_states_free(NFA* nfa);
NFA* NFA_init(int states_count, int alphabet_dim, int initial_state, int final_states_count, int* final_states);
NFA* NFA_clone(NFA* nfa);
void NFA_free(NFA* nfa);
void NFA_transition_add(NFA* nfa, int start_state, int end_state, int letter);
void NFA_transition_remove(NFA* nfa, int start_state, int end_state, int letter);
bool NFA_state_add(NFA* nfa, bool is_final);
bool NFA_state_remove(NFA* nfa, int id);
/**
 * @brief Remove states from NFA
 *
 * @param removed_states: has to be in format [0, 0, 1, 0, 1, ...] where 1 stands for state to be removed
 */
bool NFA_state_list_remove(NFA* nfa, int* removed_states, int list_size);
void NFA_transitions_list_add(NFA* nfa, int start_state, list* end_states, int letter);
bool NFA_accept(NFA* nfa, char* num);
bool NFA_accept(NFA* nfa, char** nums, int nums_count);
bool NFA_accept(NFA* nfa, big_int* num);
bool NFA_accept(NFA* nfa, big_int_list* bigint_list);


void NFA_print(NFA* nfa);
void NFA_to_DOT(NFA* nfa);
void generate_png_from_dot(char* dot_filename);
void NFA_to_file(NFA* automaton, const char* filename);
NFA* NFA_from_file(const char* filename);


NFA* NFA_intersect(NFA* nfa1, NFA* nfa2);
void NFA_intersect_rec(NFA** nfa1, NFA* nfa2);
NFA* NFA_union(NFA* nfa1, NFA* nfa2);
void NFA_union_rec(NFA** nfa1, NFA* nfa2);
NFA* DFA_complement(NFA* nfa);
void DFA_complement_rec(NFA** nfa);
/**
 * @brief Creates NFA which accepts words from L2\\L1 {suffixes: w2 + suffix = w1, w1 in L1, w2 in L2}.
 * Note that nfa2 should be EXTENDED to have the same alphabet_dim as nfa1 (each time differently).
 */
NFA* NFA_leftquo(NFA* nfa1, NFA* nfa2);
/**
 * @brief Assigns union of nfa1 and new NFA to nfa1. New NFA accepts words from L2\\L1 {suffixes: w2 + suffix = w1, w1 in L1, w2 in L2}.
 * Note that nfa2 should be EXTENDED to have the same alphabet_dim as nfa1 (each time differently).
 */
void NFA_leftquo_unioned(NFA** nfa1, NFA* nfa2);
/**
 * @brief Creates NFA which accepts words from L1/L2 {prefixes: prefix + w2 = w1, w1 in L1, w2 in L2}.
 * Note that nfa2 should be EXTENDED to have the same alphabet_dim as nfa1 (each time differently).
 */
NFA* NFA_rightquo(NFA* nfa1, NFA* nfa2);
/**
 * @brief Assigns union of nfa1 and new NFA to nfa1. 
 * New NFA accepts words from L1/L2 {prefixes: prefix + w2 = w1, w1 in L1, w2 in L2}.
 * Note that nfa2 should be EXTENDED to have the same alphabet_dim as nfa1 (each time differently).
 */
void NFA_rightquo_unioned(NFA** nfa1, NFA* nfa2);
/**
 * @brief Return a pointer to new NFA with deleted n-th coordinate in transition letters
 *
 * @param n: Number of a coordinate, starting from left with 0 (e.g. "0010" minus 1-st coordinate = "010")
 */
NFA* NFA_project(NFA* nfa, unsigned char n);
NFA* NFA_project(NFA* nfa, unsigned char n, bool with_quotient);
/**
 * @brief Delete n-th coordinate in transition letters
 *
 * @param n: Number of a coordinate, starting from left with 0 (e.g. "0010" minus 1-st coordinate = "010")
 */
void NFA_project_rec(NFA** nfa, unsigned char n);
void NFA_project_rec(NFA** nfa, unsigned char n, bool with_quotient);
/**
 * @brief Return a pointer to new NFA with added n-th coordinate in transition letters
 *
 * @param n: Number of a coordinate, starting from left with 0 (e.g. "0010" plus 2-nd coordinate = "00010"/"00110")
 */
NFA* NFA_extend(NFA* nfa, unsigned char n);
/**
 * @brief Add n-th coordinate in transition letters
 *
 * @param n: Number of a coordinate, starting from left with 0 (e.g. "0010" plus 2-nd coordinate = "00010"/"00110")
 */
void NFA_extend_rec(NFA** nfa, unsigned char n);
/**
 * @brief Return a pointer to new NFA with swapped n1-th and n2-th coordinates in transition letters
 *
 * @param n: Number of a coordinate, starting from left with 0 (e.g. swap 0,1 in letter "0111" will result in "1011")
 */
NFA* NFA_swap(NFA* nfa, int n1, int n2);
/**
 * @brief Swap n1-th and n2-th coordinates in transition letters
 *
 * @param n: Number of a coordinate, starting from left with 0 (e.g. swap 0,1 in letter "0111" will result in "1011")
 */
void NFA_swap_rec(NFA** nfa, int n1, int n2);
/**
 * @brief IMPORTANT! Removes unreachable states from initial automaton
 * Return a pointer to new DFA which is converted from given NFA
 */
NFA* NFA_to_DFA(NFA* nfa_original);
/**
 * @brief IMPORTANT! Removes unreachable states from initial automaton
 * Converts NFA to DFA
 */
void NFA_to_DFA_rec(NFA** nfa);
/**
 * @brief IMPORTANT! Removes unreachable states from initial automaton
 * Return a pointer to a new minimized DFA 
 */
NFA* DFA_minimize(NFA* nfa_original);
/**
 * @brief IMPORTANT! Removes unreachable states from initial automaton
 * Minimize given DFA
 */
void DFA_minimize_rec(NFA** nfa);


void DFA_make_complete(NFA* nfa);
list** divide_into_groups(NFA* nfa, list* group, int** state_group, int* groups_count);
stack* NFA_get_reachable_states(NFA* nfa, int start_state, char letter);
NFA* NFA_remove_unreachable_states(NFA* nfa);
void NFA_remove_unreachable_states_rec(NFA* nfa);
bool NFA_is_empty(NFA* nfa);
int NFA_get_final_states_count(NFA* nfa);
int* NFA_get_final_states(NFA* nfa, int* states_count);
bool NFA_is_DFA(NFA* nfa);
void copy_transitions(NFA* nfa, int from_state, int to_state);
void find_epsilon_closure(NFA* nfa, int state_id, bool* epsilon_closure);
void NFA_remove_epsilon_transitions(NFA* nfa);


/**
 * @brief Creates new NFA that checks: 2|n
 */
NFA* NFA_get_div_2_custom();
/**
 * @brief Creates new NFA that checks: 3|n
 */
NFA* NFA_get_div_3_custom();
/**
 * @brief Creates new NFA that checks: 2^power|n
 *
 * @param power: power of two
 */
NFA* NFA_get_div_power_of_2_custom(int power);
/**
 * @brief Creates new NFA that checks: x+y=z
 */
NFA* NFA_get_sum();
/**
 * @brief Creates new NFA that checks: x=y
 */
NFA* NFA_get_equal();
/**
 * @brief Creates random NFA with alphabet_dim = 1 and max_states_count = 6
 */
NFA* NFA_get_random();
/**
 * @brief Creates custom NFA
 */
NFA* NFA_get_automaton_1();
/**
 * @brief Creates NFA that accepts only zero-strings 
 */
NFA* NFA_get_only_zeroes();
NFA* NFA_get_trivial();
NFA* NFA_get_equal_coordinates(NFA* nfa, int n1, int n2);
void NFA_get_equal_coordinates_rec(NFA** nfa, int n1, int n2);
NFA* NFA_get_equal_num(int num);
NFA* NFA_get_div_2();
NFA* NFA_get_div_3();
NFA* NFA_get_2_power_m_y(int power);
NFA* NFA_get_div_2_power_m(int power);
NFA* NFA_get_a_y(int num);
NFA* NFA_get_div_a(int a);
NFA* NFA_get_linear_term(int* a, int count);


void NFA_console_app();
void print_help();
void NFA_list();
int nfa_get_priority(char op);
char* NFA_RPN(const char* formula);
NFA* load_NFA_from_file(const char* filename);
NFA* predicate_to_NFA(const char* predicate);


nfa_stack* create_nfa_stack();
void push(nfa_stack* s, NFA* nfa);
NFA* pop(nfa_stack* s);
bool is_stack_empty(nfa_stack* s);
void free_stack(nfa_stack* s);
NFA* NFA_from_predicate(const char* predicate);

void NFA_def(const char* command);
void NFA_eval_command(const char* command);
void NFA_for_command(const char* command);
void NFA_eval2_command(const char* command);
void DFA_complement_rec(NFA** nfa);
void NFA_visualize_command(const char* command);
void handle_minimization(const char* automaton_name);
void handle_conversion_to_dfa(const char* automaton_name);
char* extract_name(const char* token);

void handle_operation(nfa_stack* stack, char op);
void to_lower_case(char *str);
void handle_remove_epsilon(const char* automaton_name);
void handle_cls();
void handle_command(const std::string& command);

linear_term* parse_linear_term(const char* input);
void print_linear_term(const linear_term* term);

NFA* NFA_get_only_zeroes(int dim);

void NFA_leftquo_rec(NFA** nfa1, NFA* nfa2);
void NFA_rightquo_rec(NFA** nfa1, NFA* nfa2);

NFA* NFA_with_term(NFA* nfa, NFA* term);

#endif //COMPUTER_PRACTICES_NFA_H
