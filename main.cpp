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

    /*int states_count = 4;
    int alphabet_dim = 1;
    int initial_state = 0;
    int final_states_count = 1;
    int final_states_nums[] = {0};
    int* final_states = (int*)malloc(sizeof(int) * final_states_count);
    final_states = &final_states_nums[0];*/
    

    int nums[] = { 0, 54, 53 };
    int nums2[] = { 54, 53 };
    int* bigint_list_nums = &nums[0];
    int* bigint_list_nums2 = &nums2[0];
    big_int_list* bigint_list = big_int_list_init(3, bigint_list_nums);
    big_int_list* bigint_list2 = big_int_list_init(2, bigint_list_nums2);

    NFA* nfa_sum = NFA_get_sum3();
    NFA_transition_add(nfa_sum, 0, 1, 1 << nfa_sum->alphabet_dim);
    NFA* nfa_sum2 = NFA_project(nfa_sum, 1); 
    

    printf("result: %d", NFA_accept(nfa_sum, bigint_list));
    printf("\n2 result: %d", NFA_accept(nfa_sum2, bigint_list2));

    
    

    big_int_free(4, zero, one, two, e, mod, ex);
    return 0;
}





