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

NFA_state* NFA_state_init(int id, bool is_final, int alphabet_dim);
NFA* NFA_init(int states_count, int alphabet_dim, int initial_state, int final_states_count, int* final_states);
void NFA_free(NFA* automaton);
void NFA_transition_add(NFA* automaton, int start_state, int end_state, int letter);
void NFA_transition_remove(NFA* automaton, int start_state, int end_state, int letter);
void NFA_transitions_list_add(NFA* automaton, int start_state, list* end_states, int letter);
bool NFA_accept(NFA* automaton, big_int* num);
bool NFA_accept(NFA* automaton, big_int_list* bigint_list);

void NFA_print(NFA* automaton);
void NFA_to_DOT(NFA* automaton);
NFA* NFA_from_file(const char* filename);

NFA* intersect_NFA(NFA* nfa1, NFA* nfa2);
NFA* union_NFA(NFA* nfa1, NFA* nfa2);
void DFA_complement(NFA* automaton);

int* NFA_get_final_states(NFA* nfa, int* states_count);
/**
 * @brief Delete n-th coordinate in transition letters
 *
 * @param nfa: NFA.
 * @param n: Number of a coordinate, starting from left (e.g. 2 = "0010" minus 3-rd coordinate = "000" = 0)
 * @return Pointer to the newly created NFA.
 */
NFA* NFA_project(NFA* nfa, unsigned char n);
bool NFA_is_DFA(NFA* automaton);
void copy_transitions(NFA* automaton, int from_state, int to_state);
void find_epsilon_closure(NFA* automaton, int state_id, bool* epsilon_closure);
void NFA_remove_epsilon_transitions(NFA* automaton);

/**
 * @brief Creates new NFA that checks: 2|n
 */
NFA* NFA_get_div_2();

/**
 * @brief Creates new NFA that checks: 3|n
 */
NFA* NFA_get_div_3();

/**
 * @brief Creates new NFA that checks: x+y=z
 */
NFA* NFA_get_sum3();

#endif //COMPUTER_PRACTICES_NFA_H
