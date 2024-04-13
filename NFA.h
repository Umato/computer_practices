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
#include <bitset>

#include "direct.h"
#include "windows.h"

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


void list_free(list* tr_list);
void add_to_list(list* l, int val);
void print_bin(unsigned number, unsigned int bits);
int get_random_num(int start, int end);


NFA_state* NFA_state_init(int id, bool is_final, int alphabet_dim);
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
bool NFA_accept(NFA* nfa, big_int* num);
bool NFA_accept(NFA* nfa, big_int_list* bigint_list);


void NFA_print(NFA* nfa);
void NFA_to_DOT(NFA* nfa);
void generate_png_from_dot(char* dot_filename);
void NFA_to_file(NFA* automaton, const char* filename);
NFA* NFA_from_file(const char* filename);


NFA* intersect_NFA(NFA* nfa1, NFA* nfa2);
NFA* union_NFA(NFA* nfa1, NFA* nfa2);
void DFA_complement(NFA* nfa);
/**
 * @brief Creates NFA which accepts words from L2\\L1 {suffixes: w2 + suffix = w1, w1 in L1, w2 in L2}
 */
NFA* NFA_leftquo(NFA* nfa1, NFA* nfa2);
/**
 * @brief Creates NFA which accepts words from L1/L2 {prefixes: prefix + w2 = w1, w1 in L1, w2 in L2}
 */
NFA* NFA_rightquo(NFA* nfa1, NFA* nfa2);
/**
 * @brief Delete n-th coordinate in transition letters
 *
 * @param n: Number of a coordinate, starting from left with 0 (e.g. "0010" minus 1-st coordinate = "010")
 */
NFA* NFA_project(NFA* nfa, unsigned char n);
/**
 * @brief Add n-th coordinate in transition letters
 *
 * @param n: Number of a coordinate, starting from left with 0(e.g. "0010" plus 2-nd coordinate = "00010"/"00110")
 */
NFA* NFA_extend(NFA* nfa, unsigned char n);
/**
 * @brief Swap n1-th and n2-th coordinates in transition letters
 *
 * @param n: Number of a coordinate, starting from left with 0 (e.g. swap 0,1 in letter "0111" will result in "1011")
 */
NFA* NFA_swap(NFA* nfa, int n1, int n2);
/**
 * @brief IMPORTANT! Removes unreachable states from initial automaton
 * Converts NFA to DFA
 */
NFA* NFA_to_DFA(NFA* nfa);
/**
 * @brief IMPORTANT! Removes unreachable states from initial automaton
 * Minimize given DFA
 */
NFA* DFA_minimize(NFA* nfa);


list** divide_into_groups(NFA* nfa, list* group, int** state_group, int* groups_count);
void NFA_remove_unreachable_states(NFA* nfa);
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
NFA* NFA_get_div_2();
/**
 * @brief Creates new NFA that checks: 2^power|n
 *
 * @param power: power of two
 */
NFA* NFA_get_div_power_of_2(int power);
/**
 * @brief Creates new NFA that checks: 3|n
 */
NFA* NFA_get_div_3();
/**
 * @brief Creates new NFA that checks: x+y=z
 */
NFA* NFA_get_sum3();
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


void NFA_console_app();
void print_help();
void NFA_list();
int nfa_get_priority(char op);
char* NFA_RPN(const char* formula);

#endif //COMPUTER_PRACTICES_NFA_H
