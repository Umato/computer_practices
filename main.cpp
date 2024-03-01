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

    NFA* nfa2 = NFA_init(states_count, alphabet_dim, initial_state, final_states_count, final_states);
    NFA_transition_add(nfa2, 0, 0, 0);
    NFA_transition_add(nfa2, 0, 0, 1);
    NFA_transition_add(nfa2, 0, 1, 0);
    NFA_transition_add(nfa2, 0, 2, 1);
    NFA_transition_add(nfa2, 1, 3, 1);
    NFA_transition_add(nfa2, 2, 3, 0);
    NFA_print(nfa2);
    NFA_to_DOT(nfa2);
    // accepts nums which start with 10

    big_int* num;
    num = big_int_get("11000100001001010111"); //not ok
    printf("\n\nAns: %d",NFA_accept(nfa2, num));
    big_int_free(num);
    num = big_int_get("10000100001001010111"); //ok
    printf("\nAns: %d",NFA_accept(nfa2, num));
    big_int_free(num);

    NFA_free(nfa2);


    big_int_free(4, zero, one, two, e, mod, ex);
    return 0;
}





