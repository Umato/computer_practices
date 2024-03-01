#include <iostream>
#include <random>
#include "big_integer.h"
#include "Algorithm.h"
#include "test.h"
#include "graph.h"
#include "NFA.h"

int main() {
    std::random_device rd;
    srand(rd());

    int final_states_count = 1;
    int states_count = 4;
    int alphabet_dim = 1;
    int initial_state = 0;
    int* final_states = (int*)malloc(sizeof(int) * final_states_count);
    final_states[0] = 3;

    NFA *nfa2 = NFA_init(states_count, alphabet_dim, initial_state, final_states_count, final_states);
    NFA_transition_add(nfa2, 0, 0, 0);
    NFA_transition_add(nfa2, 0, 0, 1);
    NFA_transition_add(nfa2, 0, 1, 0);
    NFA_transition_add(nfa2, 0, 2, 1);
    NFA_transition_add(nfa2, 1, 3, 1);
    NFA_transition_add(nfa2, 2, 3, 0);
    NFA_transition_add(nfa2, 2, 3, 2);
    NFA_print(nfa2);
    NFA_to_DOT(nfa2);
    NFA_free(nfa2);

//    NFA_print(nfa2);

    big_int_free(4, zero, one, two, e, mod, ex);
    return 0;
}





