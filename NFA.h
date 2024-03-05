//
// Created by Umato on 29.02.2024.
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

NFA_state* NFA_state_init(int id, bool is_final, int alphabet_dim);
NFA* NFA_init(int states_count, int alphabet_dim, int initial_state, int final_states_count, int* final_states);
void list_free(list* tr_list);
void NFA_free(NFA* automaton);
void NFA_transition_add(NFA* automaton, int start_state, int end_state, int letter);
void NFA_transition_remove(NFA* automaton, int start_state, int end_state, int letter);
void NFA_print(NFA* automaton);
bool NFA_accept(NFA* automaton, big_int* num);

void NFA_to_DOT(NFA* automaton);
NFA* NFA_from_file(const char* filename);
NFA* intersect_NFA(NFA* nfa1, NFA* nfa2);
NFA* union_NFA(NFA* nfa1, NFA* nfa2);
bool NFA_is_DFA(NFA* automaton);

void find_epsilon_closure(NFA* automaton, int state_id, bool* epsilon_closure);
void copy_transitions(NFA* automaton, int from_state, int to_state);
void NFA_remove_epsilon_transitions(NFA* automaton);


#endif //COMPUTER_PRACTICES_NFA_H
