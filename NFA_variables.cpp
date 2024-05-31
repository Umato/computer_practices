//
// Created by Umato on 31.05.2024.
//

#include "NFA_variables.h"

NFA_variables* NFA_variables_init()
{
    NFA_variables* vars = (NFA_variables*)malloc(sizeof(NFA_variables));
    vars->count = 0;
    vars->variables = nullptr;

    return vars;
}

NFA_variables* NFA_variables_clone(NFA_variables* vars)
{
    NFA_variables* new_vars = NFA_variables_init();
    new_vars->count = vars->count;
    new_vars->variables = (char**)malloc(sizeof(char*) * new_vars->count);
    for (int i = 0; i < new_vars->count; i++)
    {
        char* var = vars->variables[i];
        new_vars->variables[i] = (char*)calloc(strlen(var) + 1, sizeof(char));
        strcpy(new_vars->variables[i], var);
        // может быть тут нужен '\0'
    }
    return new_vars;
}

NFA_variables* NFA_vars_from_linear_expr(linear_expression* expr)
{
    if (!expr) return nullptr;
    NFA_variables* vars = NFA_variables_init();
    for (int i = 0; i < expr->terms_count; i++)
    {
        NFA_variables_add(vars, expr->terms[i].variable);
    }

    return vars;
}

void NFA_variables_delete(NFA_variables* vars, int index)
{
    if (!vars || (index < 0) || index >= vars->count) return;

    free(vars->variables[index]);
    for (int i = index; i < vars->count - 1; i++)
    {
        vars->variables[i] = vars->variables[i + 1];
    }

    vars->count--;
    vars->variables = (char**)realloc(vars->variables, sizeof(char*) * (vars->count));
}

void NFA_variables_free(NFA_variables* vars)
{
    if (!vars) return;
    for (int i = 0; i < vars->count; i++)
    {
        free(vars->variables[i]);
    }
    free(vars->variables);
    free(vars);
}

bool NFA_variables_in(NFA_variables* vars, const char* var)
{
    return NFA_variables_index(vars, var) != -1;
}

int NFA_variables_index(NFA_variables* vars, const char* var)
{
    if (!vars) return -1;
    for (int i = 0; i < vars->count; i++)
    {
        char* current_var = vars->variables[i];
        if ((strlen(var) == strlen(current_var)) && (strncmp(current_var, var, strlen(var)) == 0)) return i;
    }

    return -1;
}

void NFA_variables_add(NFA_variables* vars, const char* new_var)
{
    if (!vars || NFA_variables_in(vars, new_var)) return;

    vars->count++;
    vars->variables = (char**)realloc(vars->variables, sizeof(char*) * (vars->count));
    vars->variables[vars->count - 1] = (char*)calloc(strlen(new_var) + 1, sizeof(char));
    strcpy(vars->variables[vars->count - 1], new_var);
    // может быть тут нужен '\0'
}

void NFA_variables_insert(NFA_variables* vars, int index, const char* new_var)
{
    if (!vars || index > vars->count) return;

    vars->count++;
    vars->variables = (char**)realloc(vars->variables, sizeof(char*) * (vars->count));


    for (int i = vars->count - 1; i > index; i--)
    {
        vars->variables[i] = vars->variables[i - 1];
    }

    vars->variables[index] = (char*)calloc(strlen(new_var) + 1, sizeof(char));
    strcpy(vars->variables[index], new_var);
    // может быть тут нужен '\0'
}

void NFA_variables_swap(NFA_variables* vars, int index1, int index2)
{
    if (!vars) return;

    char* temp_var = vars->variables[index1];
    vars->variables[index1] = vars->variables[index2];
    vars->variables[index2] = temp_var;
    temp_var = nullptr;
}