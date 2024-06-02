//
// Coded by Zelendzu & Umato on 31.05.2024.
//

#ifndef COMPUTER_PRACTICES_NFA_H
#define COMPUTER_PRACTICES_NFA_H

#include "basic_functions.h"
#include "linear_term.h"

/**
 * @brief Structure representing a state in the NFA.
 */
typedef struct NFA_state {
    int id;                  /**< Unique identifier for the state. */
    bool is_final;           /**< Indicates whether the state is a final (accepting) state. */
    list** transitions;      /**< Array of lists, each representing transitions for a given symbol. */
} NFA_state;

/**
 * @brief Structure representing a non-deterministic finite automaton (NFA).
 */
typedef struct NFA {
    int states_count;        /**< Number of states in the NFA. */
    int alphabet_dim;        /**< Dimension of the alphabet (number of possible input symbols). */
    NFA_state** states;      /**< Array of pointers to the states in the NFA. */
    NFA_state* initial_state;/**< Pointer to the initial state of the NFA. */
} NFA;

#pragma region Others
/**
 * @brief Initialize list
 */
list* list_init();
/**
 * @brief Free memory, allocated for tr_list
 */
void list_free(list* tr_list);
/**
 * @brief Adds val to list
 */
void add_to_list(list* l, int val);
/**
 * @brief Prints binary representation of given num
 */
void print_bin(unsigned number, unsigned int bits);
/**
 * @brief Returns random number from start to end
 */
int get_random_num(int start, int end);
/**
 * @brief Returns given string, where only '0' and '1' remaining
 */
char* format_string_to_bin(const char* string);
/**
 * @brief Converts integer num to binary string
 */
char* int_to_string(int num);
/**
 * @brief Extracts automaton name - string that goes before "("
 */
char* extract_name(const char* token);
/**
 * @brief Lowercases given str
 */
void to_lower_case(char* str);
#pragma endregion


#pragma region NFA Main
/**
 * @brief Initialize NFA_state
 */
NFA_state* NFA_state_init(int id, bool is_final, int alphabet_dim);
/**
 * @brief Free memory, allocated for NFA.states
 */
void NFA_states_free(NFA* nfa);
/**
 * @brief Initialize NFA
 */
NFA* NFA_init(int states_count, int alphabet_dim, int initial_state, int final_states_count, int* final_states);
/**
 * @brief Clones given NFA, returns pointer to new NFA
 */
NFA* NFA_clone(NFA* nfa);
/**
 * @brief Free memory, allocated for given nfa
 */
void NFA_free(NFA* nfa);
/**
 * @brief Adds transition from start_state to end_state by letter in given nfa
 */
void NFA_transition_add(NFA* nfa, int start_state, int end_state, int letter);
/**
 * @brief Removes transition from start_state to end_state by letter in given nfa
 */
void NFA_transition_remove(NFA* nfa, int start_state, int end_state, int letter);
/**
 * @brief Add new state to given nfa
 */
bool NFA_state_add(NFA* nfa, bool is_final);
/**
 * @brief Removes NFA state by id (decrease state IDs following the deleted state by 1)
 */
bool NFA_state_remove(NFA* nfa, int id);
/**
 * @brief Remove states from NFA
 * @param removed_states: has to be in format [0, 0, 1, 0, 1, ...] where 1 stands for state to be removed
 */
bool NFA_state_list_remove(NFA* nfa, int* removed_states, int list_size);
/**
 * @brief Adds transitions from start_state to all end_states by given letter
 */
void NFA_transitions_list_add(NFA* nfa, int start_state, list* end_states, int letter);
/**
 * @brief Check if any final state is reachable by given num
 * @param nfa: one-dimensional NFA
 * @param num: binary representation of number
 */
bool NFA_accept(NFA* nfa, char* num);
/**
 * @brief Check if any final state is reachable by given numbers
 * @param nfa: multi-dimensional NFA
 * @param nums: list of binary strings
 * @param nums_count: count of given numbers
 */
bool NFA_accept(NFA* nfa, char** nums, int nums_count);
/**
 * @brief Check if any final state is reachable by given num
 * @param nfa: one-dimensional NFA
 * @param num: big_integer number
 */
bool NFA_accept(NFA* nfa, big_int* num);
/**
 * @brief Check if any final state is reachable by given nums
 * @param nfa: multi-dimensional NFA
 * @param bigint_list: list of big_integers
 */
bool NFA_accept(NFA* nfa, big_int_list* bigint_list);
#pragma endregion


#pragma region Input/Output
/**
 * @brief Prints main info and transitions of given NFA
 */
void NFA_print(NFA* nfa);
/**
 * @brief Saves and shows given NFA to DOT file
 */
void NFA_to_DOT(NFA* nfa);
/**
 * @brief Creates and shows .png file from given .dot file
 */
void generate_png_from_dot(char* dot_filename);
/**
 * @brief Saves given NFA to a new file
 */
void NFA_to_file(NFA* automaton, const char* filename);
/**
 * @brief Returns loaded NFA from given path
 */
NFA* NFA_from_file(const char* filename);
/**
 * @brief Returns loaded NFA from "NFAs" folder by its name. If name is "divN", returns NFA that accepts numbers divided by N
 */
NFA* load_NFA_from_file(const char* filename);
#pragma endregion


#pragma region Operations
/**
 * @brief Intersect nfa1 and nfa2 and returns pointer to a resulting NFA
 */
NFA* NFA_intersect(NFA* nfa1, NFA* nfa2);

/**
 * @brief Intersect nfa1 and nfa2 and saves resulting NFA to nfa1
 */
void NFA_intersect_rec(NFA** nfa1, NFA* nfa2);

/**
 * @brief Union nfa1 and nfa2 and returns pointer to a resulting NFA
 */
NFA* NFA_union(NFA* nfa1, NFA* nfa2);

/**
 * @brief Union nfa1 and nfa2 and saves resulting NFA to nfa1
 */
void NFA_union_rec(NFA** nfa1, NFA* nfa2);

/**
 * @brief Complets DFA and returns pointer to a resulting NFA
 */
NFA* DFA_complement(NFA* nfa);

/**
 * @brief Complets of DFA and saves resulting NFA to nfa
 */
void DFA_complement_rec(NFA** nfa);

/**
 * @brief Creates NFA which accepts words from L2\\L1 {suffixes: w2 + suffix = w1, w1 in L1, w2 in L2}.
 * Note that nfa2 should be EXTENDED to have the same alphabet_dim as nfa1 (each time differently).
 */
NFA* NFA_leftquo(NFA* nfa1, NFA* nfa2);

/**
 * @brief Assigns left quotient of nfa1 and nfa2 to nfa1
 */
void NFA_leftquo_rec(NFA** nfa1, NFA* nfa2);

/**
 * @brief Assigns union of nfa1 and new NFA to nfa1. 
 * New NFA accepts words from L2\\L1 {suffixes: w2 + suffix = w1, w1 in L1, w2 in L2}.
 * Note that nfa2 should be EXTENDED to have the same alphabet_dim as nfa1 (each time differently).
 */
void NFA_leftquo_unioned(NFA** nfa1, NFA* nfa2);

/**
 * @brief Creates NFA which accepts words from L1/L2 {prefixes: prefix + w2 = w1, w1 in L1, w2 in L2}.
 * Note that nfa2 should be EXTENDED to have the same alphabet_dim as nfa1 (each time differently).
 */
NFA* NFA_rightquo(NFA* nfa1, NFA* nfa2);

/**
 * @brief Assigns right quotient of nfa1 and nfa2 to nfa1
 */
void NFA_rightquo_rec(NFA** nfa1, NFA* nfa2);

/**
 * @brief Assigns union of nfa1 and new NFA to nfa1.
 * New NFA accepts words from L1/L2 {prefixes: prefix + w2 = w1, w1 in L1, w2 in L2}.
 * Note that nfa2 should be EXTENDED to have the same alphabet_dim as nfa1 (each time differently).
 */
void NFA_rightquo_unioned(NFA** nfa1, NFA* nfa2);

/**
 * @brief Return a pointer to new NFA with deleted n-th coordinate in transition letters
 *
 * @param n Number of a coordinate, starting from left with 0 (e.g. "0010" minus 1-st coordinate = "010")
 */
NFA* NFA_project(NFA* nfa, unsigned char n);

/**
 * @brief Return a pointer to new NFA with deleted n-th coordinate in transition letters with quotient option
 *
 * @param n: Number of a coordinate, starting from left with 0 (e.g. "0010" minus 1-st coordinate = "010")
 * @param with_quotient: Whether to include zero-quotient (non-significant zeros) in the result
 */
NFA* NFA_project(NFA* nfa, unsigned char n, bool with_quotient);

/**
 * @brief Delete n-th coordinate in transition letters
 *
 * @param n Number of a coordinate, starting from left with 0 (e.g. "0010" minus 1-st coordinate = "010")
 */
void NFA_project_rec(NFA** nfa, unsigned char n);

/**
 * @brief Delete n-th coordinate in transition letters with quotient option
 *
 * @param n Number of a coordinate, starting from left with 0 (e.g. "0010" minus 1-st coordinate = "010")
 * @param with_quotient Whether to include zero-quotient (non-significant zeros) in the result
 */
void NFA_project_rec(NFA** nfa, unsigned char n, bool with_quotient);

/**
 * @brief Return a pointer to new NFA with added n-th coordinate in transition letters
 *
 * @param n Number of a coordinate, starting from left with 0 (e.g. "0010" plus 2-nd coordinate = "00010"/"00110")
 */
NFA* NFA_extend(NFA* nfa, unsigned char n);

/**
 * @brief Add n-th coordinate in transition letters
 *
 * @param n Number of a coordinate, starting from left with 0 (e.g. "0010" plus 2-nd coordinate = "00010"/"00110")
 */
void NFA_extend_rec(NFA** nfa, unsigned char n);

/**
 * @brief Return a pointer to new NFA with swapped n1-th and n2-th coordinates in transition letters
 *
 * @param n1 Number of the first coordinate, starting from left with 0 (e.g. swap 0,1 in letter "0111" will result in "1011")
 * @param n2 Number of the second coordinate
 */
NFA* NFA_swap(NFA* nfa, int n1, int n2);

/**
 * @brief Swap n1-th and n2-th coordinates in transition letters
 *
 * @param n1 Number of the first coordinate, starting from left with 0 (e.g. swap 0,1 in letter "0111" will result in "1011")
 * @param n2 Number of the second coordinate
 */
void NFA_swap_rec(NFA** nfa, int n1, int n2);

/**
 * Return a pointer to new DFA which is converted from given NFA
 */
NFA* NFA_to_DFA(NFA* nfa_original);

/**
 * Converts NFA to DFA
 */
void NFA_to_DFA_rec(NFA** nfa);

/**
 * @brief Return a pointer to a new minimized DFA
 */
NFA* DFA_minimize(NFA* nfa_original);

/**
 * @brief Minimize given DFA
 */
void DFA_minimize_rec(NFA** nfa);

/**
 * @brief Concatenate nfa1 and nfa2 and returns pointer to a resulting NFA
 */
NFA* NFA_concatenate(NFA* nfa1, NFA* nfa2);

/**
 * @brief Concatenate nfa1 and nfa2 and saves resulting NFA to nfa1
 */
void NFA_concatenate_rec(NFA** nfa1, NFA* nfa2);

/**
 * @brief Return a pointer to a new NFA with Kleene star applied to it
 */
NFA* NFA_kleene_star(NFA* nfa);

/**
 * @brief Apply Kleene star to the given NFA and saves resulting NFA to the same pointer
 */
void NFA_kleene_star_rec(NFA** nfa);

/**
 * @brief Return a pointer to a new NFA with plus operation applied to it
 */
NFA* NFA_plus(NFA* nfa);

/**
 * @brief Apply plus operation to the given NFA and saves resulting NFA to the same pointer
 */
void NFA_plus_rec(NFA** nfa);

/**
 * @brief Return a pointer to a new NFA that includes the empty string
 */
NFA* NFA_with_empty_string(NFA* nfa_original);

/**
 * @brief Modify the given NFA to include the empty string
 */
void NFA_with_empty_string_rec(NFA** nfa);

#pragma endregion


#pragma region Support Functions
/**
 * @brief Clones given NFA and completes it
 */
NFA* DFA_make_complete(NFA* nfa);
/**
 * @brief Completes given NFA
 */
void DFA_make_complete_rec(NFA** nfa);
/**
 * @brief Function for DFA_minimization.
 * Divides states into groups, where states in one grup are equivalent to each other.
 */
list** divide_into_groups(NFA* nfa, list* group, int** state_group, int* groups_count);
/**
 * @brief Returns stack of states, that are reachable from given start_state
 */
stack* NFA_get_reachable_states(NFA* nfa, int start_state, char letter);
/**
 * @brief Returns new NFA, where unreachable states are removed
 */
NFA* NFA_remove_unreachable_states(NFA* nfa);
/**
 * @brief Removes unreachable states
 */
void NFA_remove_unreachable_states_rec(NFA* nfa);
/**
 * @brief Checks if given nfa is empty (has no transitions)
 */
bool NFA_is_empty(NFA* nfa);
/**
 * @brief Returns final states count
 */
int NFA_get_final_states_count(NFA* nfa);
/**
 * @brief Returns list of final states of given nfa, assigns final_states_count to states_count
 */
int* NFA_get_final_states(NFA* nfa, int* states_count);
/**
 * @brief Checks if given NFA is DFA
 */
bool NFA_is_DFA(NFA* nfa);
/**
 * @brief Copy transitions from first state with id=from_state to second state with id=to_state
 */
void copy_transitions(NFA* nfa, int from_state, int to_state);
/**
 * @brief Finds epsilon closure
 */
void find_epsilon_closure(NFA* nfa, int state_id, bool* epsilon_closure);
/**
 * @brief Removes epsilon transitions
 */
void NFA_remove_epsilon_transitions(NFA* nfa);
#pragma endregion


#pragma region Examples
/**
 * @brief Creates new NFA that accepts only empty string
 */
NFA* NFA_get_empty_string();
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
 * @brief Creates NFA that accepts only zero-strings 
 */
NFA* NFA_get_only_zeroes();
/**
 * @brief Creates multi-dimensional NFA that accepts only zero-strings
 */
NFA* NFA_get_only_zeroes(int dim);
/**
 * @brief Creates NFA that accepts everything
 */
NFA* NFA_get_trivial();
/**
 * @brief Creates new NFA which is cloned from given one, where n1 and n2 coordinates are equal
 */
NFA* NFA_get_equal_coordinates(NFA* nfa, int n1, int n2);
/**
 * @brief Creates new NFA which is cloned from given one, where n1 and n2 coordinates are equal, and assigns it to given nfa
 */
void NFA_get_equal_coordinates_rec(NFA** nfa, int n1, int n2);
/**
 * @brief Creates NFA that accepts only given num
 */
NFA* NFA_get_num(int num);
/**
 * @brief Creates NFA that accepts y,x such that y = 2^m * x
 */
NFA* NFA_get_2_power_m_y(int power);
/**
 * @brief Creates NFA that accepts numbers divisible by 2^m
 */
NFA* NFA_get_div_2_power_m(int power);
/**
 * @brief Creates NFA that accepts such y,x that y = ax
 */
NFA* NFA_get_a_y(int num);
/**
 * @brief Creates NFA that accepts numbers divisible by a
 */
NFA* NFA_get_div_a(int a);
/**
 * @brief Creates NFA that accepts linear term (such y,x1,x2,... that y = a[0] + a[1]x1 + a[2]x2 + ...)
 */
NFA* NFA_get_linear_term(int* a, int count);
/**
 * @brief Creates NFA that accepts linear term where NFA accepts "y"
 */
NFA* NFA_with_term(NFA* nfa, NFA* term, bool need_quotient = 0);
/**
 * @brief Creates NFA from linear expression that accepts linear term
 */
NFA* NFA_from_linear_expression(linear_expression* expr);
/**
 * @brief Creates NFA that accepts only give number without non-significant zeros
 */
NFA* NFA_get_num_without_zeroes(int num);
/**
 * @brief Creates NFA that accepts 0 or 1
 */
NFA* NFA_get_zero_or_one();
#pragma endregion


#endif //COMPUTER_PRACTICES_NFA_H
