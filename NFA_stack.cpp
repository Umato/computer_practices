//
// Coded by Zelendzu & Umato on 31.05.2024.
//



#include "NFA_stack.h"

nfa_stack* create_nfa_stack() {
    nfa_stack* s = (nfa_stack*)malloc(sizeof(nfa_stack));
    if (!s) return nullptr;
    s->top = nullptr;
    return s;
}

void push(nfa_stack* s, NFA* nfa) {
    nfa_node* new_node = (nfa_node*)malloc(sizeof(nfa_node));
    if (!new_node) return;
    new_node->nfa = nfa;
    new_node->next = s->top;
    s->top = new_node;
}

NFA* pop(nfa_stack* s)
{
    if (s->top == nullptr) return nullptr;
    nfa_node* temp = s->top;
    NFA* nfa = temp->nfa;
    s->top = temp->next;
    free(temp);
    return nfa;
}

bool is_stack_empty(nfa_stack* s) {
    return s->top == nullptr;
}

void free_stack(nfa_stack* s)
{
    while (!is_stack_empty(s)){
        NFA_free(pop(s));
    }
    free(s);
}