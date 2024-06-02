//
// Coded by Zelendzu & Umato on 31.05.2024.
//


#ifndef COMPUTER_PRACTICES_NFA_STACK_H
#define COMPUTER_PRACTICES_NFA_STACK_H

#include "basic_functions.h"
#include "NFA.h"

/**
 * @brief Structure representing a node in the NFA stack.
 */
typedef struct NFA_node {
    NFA* nfa;               /**< Pointer to the NFA structure stored in this node. */
    struct NFA_node* next;  /**< Pointer to the next node in the stack. */
} NFA_node;

/**
 * @brief Structure representing a stack of NFAs.
 *
 * This stack is implemented as a linked list, where each node
 * contains an NFA and a pointer to the next node in the stack.
 */
typedef struct NFA_stack {
    NFA_node* top; /**< Pointer to the top node of the stack. */
} NFA_stack;


/**
 * @brief Creates a new NFA stack.
 * @return Pointer to the created NFA stack.
 */
NFA_stack* create_nfa_stack();

/**
 * @brief Frees the memory allocated for the stack.
 * @param s Pointer to the NFA stack to be freed.
 */
void free_stack(NFA_stack* s);

/**
 * @brief Checks if the stack is empty.
 * @param s Pointer to the NFA stack.
 * @return true if the stack is empty, false otherwise.
 */
bool is_stack_empty(NFA_stack* s);

/**
 * @brief Pushes an NFA onto the stack.
 * @param s Pointer to the NFA stack.
 * @param nfa Pointer to the NFA to be pushed.
 */
void push(NFA_stack* s, NFA* nfa);

/**
 * @brief Pops an NFA from the stack.
 * @param s Pointer to the NFA stack.
 * @return Pointer to the popped NFA.
 */
NFA* pop(NFA_stack* s);



#endif //COMPUTER_PRACTICES_NFA_STACK_H
