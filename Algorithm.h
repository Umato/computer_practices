//
// Created by Umato on 30.10.2023.
//

#ifndef COMPUTER_PRACTICE_ALGORITHM_H
#define COMPUTER_PRACTICE_ALGORITHM_H

#include <iostream>
#include "graph.h"

using stack = struct stack {
    struct node *top;
};

using ull = unsigned long long;
using ll = long long;

using std::swap;
using std::abs;

/**
 * @defgroup Euclid
 * @brief Functions related to the Euclidean algorithm for finding the greatest common divisor (GCD).
 */

/**
 * @ingroup Euclid
 * @brief Compute the greatest common divisor (GCD) of two numbers using the Euclidean algorithm.
 * @param a The first input number.
 * @param b The second input number.
 * @return The GCD of a and b.
 */
ll euclid(ll a, ll b);

/**
 * @ingroup Euclid
 * @brief Compute the GCD of two numbers using the Euclidean algorithm in a recursive manner.
 * @param a The first input number.
 * @param b The second input number.
 * @return The GCD of a and b.
 */
ll euclid_recursive(const ll a, const ll b);

/**
 * @ingroup Euclid
 * @brief Compute the GCD of two numbers using the binary Euclidean algorithm.
 * @param x The first input number.
 * @param y The second input number.
 * @return The GCD of x and y.
 */
ll euclid_binary(ll x, ll y);

/**
 * @ingroup Euclid
 * @brief Extended Euclidean algorithm to find a solution for Diophantine equations. Recursive version.
 * @param a Value a.
 * @param b Value b.
 * @param x First Bezout's coeff.
 * @param y Second Bezout's coeff.
 * @return GCD of a and b.
 */
ll euclid_extended_recursive(const ll a, const ll b, ll &x, ll &y);

/**
 * @ingroup Euclid
 * @brief Extended Euclidean algorithm to find a solution for Diophantine equation. Iterative version.
 * @param a Value a.
 * @param b Value b.
 * @param x First Bezout's coeff.
 * @param y Second Bezout's coeff.
 * @return GCD of a and b.
 */
ll euclid_extended_iterative(ll a, ll b, ll &x, ll &y);

/**
 * @ingroup Euclid
 * @brief Find the multiplicative inverse of a number modulo m.
 * @param x Value for which to find the multiplicative inverse modulo m.
 * @param m The modulus.
 * @return The multiplicative inverse of x modulo m.
 */
ll mul_inverse(ll x, ll m);

/**
 * @defgroup Modular_Exponentiation
 * @brief Functions related to modular exponentiation.
 */

/**
 * @ingroup Modular_Exponentiation
 * @brief Calculate the exponentiation of a number modulo another number.
 * @param x The base.
 * @param y The exponent.
 * @param m The modulus.
 * @return The result of (x^y) mod m.
 */
ll mod_pow(ll x, ll y, ll m);

/**
 * @ingroup Modular_Exponentiation
 * @brief Recursive algorithm for raised power calculation modulo m.
 * @param x The base.
 * @param y The exponent.
 * @param m The modulus.
 * @return The result of (x^y) mod m.
 */
ll mod_pow_recursive(const ll x, const ll y, const ll m);

/**
 * @ingroup Modular_Exponentiation
 * @brief Compute x raised to the power y modulo m in logarithmic time complexity.
 * @param x The base of the power.
 * @param y The exponent of the power.
 * @param m The modulus.
 * @return The result of (x^y) mod m, using fast exponentiation (also known as exponentiation by squaring).
 */
ll lr_mod_pow(const ll x, const ll y, const ll m);

/**
 * @defgroup Linear_Diophantine_Equation
 * @brief Functions for solving linear Diophantine equations.
 */

/**
 * @ingroup Linear_Diophantine_Equation
 * @brief Solve a Diophantine equation.
 * @param coefficients Coefficients in the equation.
 * @param n Number of equations.
 * @param result Array to store the solution.
 * @param target Target number.
 * @return True if a solution exists, False otherwise.
 */
ll linear_diophantine(const ll *coefficients, const ll n, ll *result, const ll target);

/**
 * @defgroup Chinese_Reminder
 * @brief Functions for solving systems of simultaneous congruences using the Chinese Remainder Theorem (CRT).
 */

/**
 * @ingroup Chinese_Reminder
 * @brief Solve a system of simultaneous congruences using the Chinese Remainder Theorem (CRT).
 * @param remainders Remainders of the equations.
 * @param modules Moduli of the equations.
 * @param num_equations Number of equations in the system.
 * @return A number x such that x modulo modules[i] equals to remainders[i] for every i from 0 to num_equations.
 */
ll chinese_reminder(const ll *remainders, const ll *modules, const ll num_equations);

/**
 * @ingroup Chinese_Reminder
 * @brief Solve a system of simultaneous congruences using the Chinese Remainder Theorem (CRT) iteratively.
 * @param remainders Remainders of the equations.
 * @param modules Moduli of the equations.
 * @param n Number of equations in the system.
 * @return A number x such that x modulo modules[i] equals to remainders[i] for every i from 0 to n.
 */
ll chinese_remainder_iterative(const ll *remainders, const ll *modules, const ll n);

/**
 * @defgroup Other_Functions
 * @brief Miscellaneous mathematical functions.
 */

/**
 * @ingroup Other_Functions
 * @brief Converts a string of numerical characters to a long long integer.
 * @param ch The character string to be converted.
 * @return Returns the long long integer equivalent of the provided character string or 0 if invalid.
 */
ll cti(const char *ch);

/**
 * @ingroup Other_Functions
 * @brief Checks if a given number is prime.
 * @param num The number to check for primality.
 * @return Returns true if the number is prime, false otherwise.
 */
bool is_prime(const ll num);

/**
 * @ingroup Other_Functions
 * @brief Calculate the least common multiple of two numbers.
 * @param x The first number.
 * @param y The second number.
 * @return Returns the least common multiple of x and y.
 */
ll lcm(ll x, ll y);

#pragma endregion

int performOperation(int operand1, int operand2, char operation);

int RPNCalculator(const char* expression);

short getPriority(char op);

void infixToPostfix(const char* infix, char* postfix);

stack *create_stack();
void push(stack *s, int element);
int pop(stack *s);
bool is_stack_empty(stack *s);
int stack_top(stack *top);
void free_stack(stack *s);

#endif //COMPUTER_PRACTICE_ALGORITHM_H
