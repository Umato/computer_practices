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
//    NFA* nfa = NFA_project(nfa_eq, 2);
//    NFA_to_DOT(nfa);
//
//    printf("result: %d", NFA_accept(nfa, bigint_list));
#pragma endregion

#pragma region NFA_rightquo Check
    /*NFA* nfa_sum = NFA_get_sum3();
    int fs[] = { 0 };
    int* fsl = &(fs[0]);
    NFA* nfa_zeroes = NFA_init(1, 2, 0, 1, fsl);
    
    NFA_transition_add(nfa_zeroes, 0, 0, 0);
    NFA* nfa_zeroes1 = NFA_extend(nfa_zeroes, 2);
    NFA_to_DOT(nfa_zeroes1);
    NFA* nfa2 = NFA_rightquo(nfa_sum, nfa_zeroes1);
    NFA_to_DOT(nfa2);
    NFA* new_nfa = union_NFA(nfa_sum, nfa2);
    NFA_to_DOT(new_nfa);*/
#pragma endregion

#pragma region NFA_leftquo Check

    //// ������� ����������� ������ {0}+{1}* (�������� 000111, 00001, 11111, 1) (����� �� 0 �� +\\inf, ����� ������ �� 1 �� +\\inf)
    //int fs[] = { 1 };
    //int* fsl = &(fs[0]);
    //NFA* nfa01 = NFA_init(2, 1, 0, 1, fsl);
    //NFA_transition_add(nfa01, 0, 0, 0);
    //NFA_transition_add(nfa01, 0, 1, 1);
    //NFA_transition_add(nfa01, 1, 1, 1);

    //// ������� ����������� ������ {0}+1 (�������� 0000001, 01, 1) (����� �� 0 �� +\\inf, ����� ������ ���� 1)
    //int fs2[] = { 1 };
    //int* fsl2 = &(fs[0]);
    //NFA* nfa_zeroes1 = NFA_init(2, 1, 0, 1, fsl2);
    //NFA_transition_add(nfa_zeroes1, 0, 0, 0);
    //NFA_transition_add(nfa_zeroes1, 0, 1, 1);

    //// ����� ������� ����������� {1}* (�� ���� ������ �� 1 �� +\\inf)
    //NFA* new_nfa = NFA_leftquo(nfa01, nfa_zeroes1);
    //NFA_to_DOT(new_nfa);
    //// �� �������� ��� �� ����������� - ������ �������� nfa01, � ����� ���� �� �� �������� ������ ������������ � {0}*, � �� ��������� :(

#pragma endregion

#pragma region Both Quotients Check
    //int fs[] = { 0 };
    //int* fsl = &(fs[0]);

    //NFA* nfa2 = NFA_init(2, 1, 0, 1, fsl); // (01)* = 01/0101/010101/01010101 - ��������� L2 - ���� �������� ����� ������� ��� ������
    //NFA_transition_add(nfa2, 0, 1, 0);
    //NFA_transition_add(nfa2, 1, 0, 1);

    //NFA* nfa1 = NFA_init(1, 1, 0, 1, fsl); // {0,1}* - ��������� L1 - ��� ��� ������
    //NFA_transition_add(nfa1, 0, 0, 0);
    //NFA_transition_add(nfa1, 0, 0, 1);


    //// L2  (01)* = 01/0101/010101/01010101
    //// L1  {0,1}* - ��������� ��� ��� ������

    //NFA* left = NFA_leftquo(nfa2, nfa1); // ��� ����� �������� S, ��� ���� ����� �� L1 ����� ��� L1+S in L2 
    //// (spoiler: ��������� 1/01/101/0101/ 10101/ 010101/ 1010101 (01)* )


    //NFA* right = NFA_rightquo(nfa2, nfa1); // ��� ����� �������� P, ��� ���� ����� �� L1 ����� ��� P+L1 in Ls 
    //// (spoiler: ��������� 0/01/010/0101/01010 - �� ���� L2 U 0(01)* )


    //NFA_to_DOT(left);
    //NFA_to_DOT(right);
#pragma endregion


#pragma region Div2
    //// DIV2
    //NFA* nfa_eq = NFA_get_equal();
    //NFA* nfa_sum = NFA_get_sum3();
    //NFA* nfa_eq_1 = NFA_extend(nfa_eq, 2);
    //NFA* nfa_new = intersect_NFA(nfa_eq_1, nfa_sum);
    //NFA* nfa_new_result = NFA_project(nfa_new, 0);
    //NFA* nfa_new_result_2 = NFA_project(nfa_new_result, 0);
    //NFA_remove_unreachable_states(nfa_new_result_2);
    //NFA_to_DOT(nfa_new_result_2);
#pragma endregion

#pragma region Div3
//// DIV3
    NFA* nfa_equal_xz = NFA_get_equal(); // x = z
    NFA* nfa_equal_xz_1 = NFA_extend(nfa_equal_xz, nfa_equal_xz->alphabet_dim); //extend by y
    NFA* nfa_equal_xz_2 = NFA_extend(nfa_equal_xz_1, nfa_equal_xz_1->alphabet_dim); //extend by M

    NFA* nfa_sum = NFA_get_sum3(); // x + z = y
    NFA* nfa_sum_1 = NFA_extend(nfa_sum, nfa_sum->alphabet_dim); //extend by M

    NFA* nfa_inter = intersect_NFA(nfa_equal_xz_2, nfa_sum_1); //x = z && x + z = y

    NFA* nfa_res_sum = NFA_get_sum3(); //x + y = M
    NFA* nfa_res_sum_1 = NFA_extend(nfa_res_sum, 1); //extend by z

    NFA* result1 = intersect_NFA(nfa_inter, nfa_res_sum_1); // x = z && x + z = y && x + y = M
    NFA* existx = NFA_project(result1, 0);
    NFA* existy = NFA_project(existx, 0);
    NFA* existz = NFA_project(existy, 0);

    NFA_remove_unreachable_states(existz);
    

    for (int i = 0; i < 1000; i++)
    {
        big_int* n = big_int_get(i);
        if ((i % 3 == 0) != NFA_accept(existz, n))
        {
            printf("%d\n", i);
        }
    }
#pragma endregion
    
    


    big_int_free(4, zero, one, two, e, mod, ex);
    return 0;
}





