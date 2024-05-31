//
// Created by Umato on 31.05.2024.
//

#include "linear_term.h"


void add_term(linear_expression* expr, int coefficient, const char* variable)
{
    for (int i = 0; i < expr->terms_count; i++) {
        if (strcmp(expr->terms[i].variable, variable) == 0) {
            expr->terms[i].coefficient += coefficient;
            return;
        }
    }
    expr->terms = (term*)realloc(expr->terms, (expr->terms_count + 1) * sizeof(term));
    expr->terms[expr->terms_count].coefficient = coefficient;
    strcpy(expr->terms[expr->terms_count].variable, variable);
    expr->terms_count++;
}

linear_expression* init_linear_expression()
{
    linear_expression* expr = (linear_expression*)malloc(sizeof(linear_expression));
    expr->terms = nullptr;
    expr->terms_count = 0;
    expr->constant = 0;
    return expr;
}

void free_linear_expression(linear_expression* expr)
{
    free(expr->terms);
    free(expr);
}

linear_expression* parse_linear_expression(const char* input)
{
    linear_expression* expr = init_linear_expression();

    const char* p = input;
    while (*p)
    {
        int coefficient = 0;
        char variable[100] = {0};
        char* var_start = variable;

        while (*p == ' ' || *p == '+') p++;

        if (isdigit(*p)){
            while (isdigit(*p)){
                coefficient = coefficient * 10 + (*p - '0');
                p++;
            }
        } else {
            coefficient = 1;
        }

        if (*p == '*') p++;

        while (isalpha(*p) || *p == '_' || isdigit(*p))
        {
            *var_start++ = *p++;
        }
        *var_start = '\0';

        if (strlen(variable) > 0) {
            add_term(expr, coefficient, variable);
        } else {
            expr->constant += coefficient;
        }
    }

    return expr;
}

void print_linear_expression(linear_expression* expr)
{
    for (int i = 0; i < expr->terms_count; i++)
    {
        printf("%d%s ", expr->terms[i].coefficient, expr->terms[i].variable);
        if (i < expr->terms_count - 1) cout << "+ ";
    }
    if (expr->constant != 0)
    {
        cout << "+ " << expr->constant;
    }
    cout << "\n";
}

