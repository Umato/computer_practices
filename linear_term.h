//
// Coded by Zelendzu & Umato on 31.05.2024.
//


#ifndef COMPUTER_PRACTICES_LINEAR_TERM_H
#define COMPUTER_PRACTICES_LINEAR_TERM_H

#include "basic_functions.h"

typedef struct term {
    int coefficient;
    char variable[100];
} term;

typedef struct linear_expression {
    term *terms;
    int terms_count;
    int constant;
} linear_expression;

void add_term(linear_expression* expr, int coefficient, const char* variable);
linear_expression* init_linear_expression();
void free_linear_expression(linear_expression* expr);
linear_expression* parse_linear_expression(const char* input);
void print_linear_expression(linear_expression* expr);


#endif //COMPUTER_PRACTICES_LINEAR_TERM_H
