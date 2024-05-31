//
// Created by Umato on 31.05.2024.
//

#ifndef COMPUTER_PRACTICES_NFA_VARIABLES_H

#include "NFA.h"

typedef struct NFA_variables
{
    int count;
    char** variables;
} NFA_variables ;

NFA_variables* NFA_variables_init();
void NFA_variables_add(NFA_variables* vars, const char* new_var);
NFA_variables* NFA_variables_clone(NFA_variables* vars);
NFA_variables* NFA_vars_from_linear_expr(linear_expression* expr);
void NFA_variables_delete(NFA_variables* vars, int index);
void NFA_variables_insert(NFA_variables* vars, int index, const char* new_var);
void NFA_variables_swap(NFA_variables* vars, int index1, int index2);
int NFA_variables_index(NFA_variables* vars, const char* var);
bool NFA_variables_in(NFA_variables* vars, const char* var);
void NFA_variables_free(NFA_variables* vars);

#define COMPUTER_PRACTICES_NFA_VARIABLES_H

#endif //COMPUTER_PRACTICES_NFA_VARIABLES_H
