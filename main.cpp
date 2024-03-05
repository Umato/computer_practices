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

    int states_count = 4;
    int alphabet_dim = 1;
    int initial_state = 0;
    int final_states_count = 1;
    int* final_states = (int*)malloc(sizeof(int) * final_states_count);
    final_states[0] = 2;

    NFA* nfa1 = NFA_init(states_count, alphabet_dim, initial_state, final_states_count, final_states);
    NFA_transition_add(nfa1, 0, 1, 0);
    NFA_transition_add(nfa1, 0, 0, 2);
    NFA_transition_add(nfa1, 0, 1, 2);
    NFA_transition_add(nfa1, 1, 1, 1);
    NFA_transition_add(nfa1, 1, 2, 0);
    NFA_transition_add(nfa1, 1, 3, 2);
    NFA_transition_add(nfa1, 3, 3, 1);
    NFA_transition_add(nfa1, 3, 2, 0);

    NFA_print(nfa1);

    NFA_remove_epsilon_transitions(nfa1);
    NFA_print(nfa1);


    big_int_free(4, zero, one, two, e, mod, ex);
    return 0;
}





