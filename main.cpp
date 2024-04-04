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

    #pragma region Start Parametres
//    int states_count = 4;
//    int states_count = 2;
//    int alphabet_dim = 1;
//    int initial_state = 0;
//    int final_states_count = 1;
//    int final_states_nums[] = {1};
//    int* final_states = (int*)malloc(sizeof(int) * final_states_count);
//    final_states = &final_states_nums[0];
    #pragma endregion

    #pragma region Project/Extend Check
    //int nums[] = { 0, 54, 53 };
    //int nums2[] = { 54, 53 };
    //int* bigint_list_nums = &nums[0];
    //int* bigint_list_nums2 = &nums2[0];
    //big_int_list* bigint_list = big_int_list_init(3, bigint_list_nums);
    //big_int_list* bigint_list2 = big_int_list_init(2, bigint_list_nums2);



    //NFA* nfa_sum = NFA_get_sum3();
    //NFA_transition_add(nfa_sum, 0, 1, 1 << nfa_sum->alphabet_dim); // just to check E-tr. 
    //NFA* nfa_sum2 = NFA_project(nfa_sum, 0);
    //NFA* nfa_sum3 = NFA_extend(nfa_sum, 2);
    //NFA_print(nfa_sum);
    //printf("\n\n");
    //NFA_print(nfa_sum2);
    //printf("\n\n");
    //NFA_print(nfa_sum3);

    //printf("result: %d", NFA_accept(nfa_sum, bigint_list));
    //printf("\n2 result: %d", NFA_accept(nfa_sum2, bigint_list2));
    #pragma endregion
 
    #pragma region NFA_equal Check

//    int nums[] ={ 2, 3, 5 };
//    int* bigint_list_nums = &nums[0];
//    big_int_list* bigint_list = big_int_list_init(3, bigint_list_nums);
//
//    NFA* nfa_eq = NFA_get_sum3();
//
//    NFA* nfa = NFA_project(nfa_eq, 2);
//
//    NFA_to_DOT(nfa);
//
//    printf("result: %d", NFA_accept(nfa, bigint_list));
    #pragma endregion


    big_int_free(4, zero, one, two, e, mod, ex);
    return 0;
}





