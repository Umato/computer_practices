//
// Coded by Zelendzu & Umato on 31.05.2024.
//


#include "NFA_stack.h"

NFA_stack* create_nfa_stack() {
    NFA_stack* s = (NFA_stack*)malloc(sizeof(NFA_stack));
    if (!s) return nullptr;
    s->top = nullptr;
    return s;
}

void push(NFA_stack* s, NFA* nfa) {
    NFA_node* new_node = (NFA_node*)malloc(sizeof(NFA_node));
    if (!new_node) return;
    new_node->nfa = nfa;
    new_node->next = s->top;
    s->top = new_node;
}

NFA* pop(NFA_stack* s)
{
    if (s->top == nullptr) return nullptr;
    NFA_node* temp = s->top;
    NFA* nfa = temp->nfa;
    s->top = temp->next;
    free(temp);
    return nfa;
}

bool is_stack_empty(NFA_stack* s) {
    return s->top == nullptr;
}

void free_stack(NFA_stack* s)
{
    while (!is_stack_empty(s)){
        NFA_free(pop(s));
    }
    free(s);
}