//
// Coded by Zelendzu & Umato on 31.05.2024.
//


#ifndef COMPUTER_PRACTICES_NFA_STACK_H
#define COMPUTER_PRACTICES_NFA_STACK_H

#include "basic_functions.h"
#include "NFA.h"

typedef struct nfa_stack {
    nfa_node* top;
} nfa_stack;

nfa_stack* create_nfa_stack();
void push(nfa_stack* s, NFA* nfa);
NFA* pop(nfa_stack* s);
bool is_stack_empty(nfa_stack* s);
void free_stack(nfa_stack* s);



#endif //COMPUTER_PRACTICES_NFA_STACK_H
