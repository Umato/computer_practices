//
// Coded by Zelendzu & Umato on 31.05.2024.
//


#ifndef COMPUTER_PRACTICES_NFA_VARIABLES_H
#define COMPUTER_PRACTICES_NFA_VARIABLES_H

#include "basic_functions.h"
#include "linear_term.h"

typedef struct NFA_variables
{
    int count;
    char** variables;
} NFA_variables ;

/**
 * @brief Creates new NFA_variables objects with count = 0 and variables = nullptr
 */
NFA_variables* NFA_variables_init();
/**
 * @brief Adds new_var to NFA_vars if it's NOT already in the list
 */
void NFA_variables_add(NFA_variables* vars, const char* new_var);
/**
 * @brief Returns pointer to new NFA_vars, cloned from given vars
 */
NFA_variables* NFA_variables_clone(NFA_variables* vars);
/**
 * @brief Creates NFA_vars from given linear_expression (with the same vars names, without const)
 */
NFA_variables* NFA_vars_from_linear_expr(linear_expression* expr);
/**
 * @brief Deletes var from given NFA_var by index
 */
void NFA_variables_delete(NFA_variables* vars, int index);
/**
 * @brief Inserts new_var into NFA_vars[index]. DO NOT CHECK if new_var is already in list, so copies may appear
 */
void NFA_variables_insert(NFA_variables* vars, int index, const char* new_var);
/**
 * @brief Swaps variables by their indices
 */
void NFA_variables_swap(NFA_variables* vars, int index1, int index2);
/**
 * @brief Returns index of given var. If it's not in list - returns -1.
 */
int NFA_variables_index(NFA_variables* vars, const char* var);
/**
 * @brief Returns True if var is in given NFA_vars and False if not
 */
bool NFA_variables_in(NFA_variables* vars, const char* var);
/**
 * @brief Free memory, allocated for given NFA_vars
 */
void NFA_variables_free(NFA_variables* vars);


#endif //COMPUTER_PRACTICES_NFA_VARIABLES_H
