//
// Coded by Zelendzu & Umato on 31.05.2024.
//


#ifndef COMPUTER_PRACTICES_LINEAR_TERM_H
#define COMPUTER_PRACTICES_LINEAR_TERM_H

#include "basic_functions.h"

/**
 * @brief Structure representing a term in a linear expression.
 */
typedef struct term {
    int coefficient;      /**< Coefficient of the term. */
    char variable[100];   /**< Variable name of the term. */
} term;

/**
 * @brief Structure representing a linear expression.
 */
typedef struct linear_expression {
    term *terms;          /**< Pointer to an array of terms in the linear expression. */
    int terms_count;      /**< Number of terms in the linear expression. */
    int constant;         /**< Constant term in the linear expression. */
} linear_expression;

/**
 * @brief Initializes a new linear expression.
 * @return Pointer to the initialized linear expression.
 */
linear_expression* init_linear_expression();

/**
 * @brief Frees the memory allocated for the linear expression.
 * @param expr Pointer to the linear expression to be freed.
 */
void free_linear_expression(linear_expression* expr);

/**
 * @brief Adds a term to the linear expression.
 * @param expr Pointer to the linear expression.
 * @param coefficient Coefficient of the term to be added.
 * @param variable Variable name of the term to be added.
 */
void add_term(linear_expression* expr, int coefficient, const char* variable);

/**
 * @brief Parses a string input into a linear expression.
 * @param input String representation of the linear expression.
 * @return Pointer to the parsed linear expression.
 */
linear_expression* parse_linear_expression(const char* input);

/**
 * @brief Prints the linear expression to the standard output.
 * @param expr Pointer to the linear expression to be printed.
 */
void print_linear_expression(linear_expression* expr);


#endif //COMPUTER_PRACTICES_LINEAR_TERM_H
