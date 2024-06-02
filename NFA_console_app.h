//
// Coded by Zelendzu & Umato on 31.05.2024.
//

/**
 * @file NFA_console_app.h
 * @brief Sub-library for NFA.h that adds a console application for user convenience.
 */

#ifndef COMPUTER_PRACTICES_NFA_CONSOLE_APP_H
#define COMPUTER_PRACTICES_NFA_CONSOLE_APP_H

#include "basic_functions.h"
#include "NFA_variables.h"
#include "linear_term.h"
#include "NFA_stack.h"
#include "NFA.h"

/**
 * @brief Starts the NFA console application.
 */
void NFA_console_app();

/**
 * @brief Prints the help command information.
 */
void print_help_command();
/**
 * @brief Prints the hElp command information (variation of help).
 */
void print_hElp_command();
/**
 * @brief Clears the console.
 */
void cls_command();
/**
 * @brief Lists available NFAs.
 */
void nfa_list_command();
/**
 * @brief Defines a new NFA from a logical predicate and saves it.
 * @param command Command string containing the definition.
 */
void def_command(const char* command);
/**
 * @brief Defines a new NFA from a regular expression and saves it.
 * @param input Command string containing the regular expression.
 */
void regex_command(const char* input);
/**
 * @brief Evaluates an automaton with given numbers.
 * @param command Command string containing the automaton name and numbers.
 */
void eval_command(const char* command);
/**
 * @brief Evaluates an automaton with given binary numbers.
 * @param command Command string containing the automaton name and binary numbers.
 */
void eval2_command(const char* command);
/**
 * @brief Tests an automaton with a range of decimal numbers.
 * @param command Command string containing the automaton name, start, end, and optional step.
 */
void for_command(const char* command);
/**
 * @brief Visualizes an NFA as a graphical representation.
 * @param command Command string containing the automaton name.
 */
void visualize_command(const char* command);
/**
 * @brief Minimizes an NFA/DFA.
 * @param automaton_name Name of the automaton to minimize.
 */
void minimization_command(const char* automaton_name);
/**
 * @brief Converts an NFA to a DFA.
 * @param automaton_name Name of the automaton to convert.
 */
void convert_to_dfa_command(const char* automaton_name);
/**
 * @brief Removes epsilon transitions from an NFA.
 * @param automaton_name Name of the automaton to remove epsilon transitions from.
 */
void remove_epsilon_command(const char* automaton_name);

/**
 * @brief Removes spaces from a string under specific conditions.
 * Removes spaces outside of parentheses and excludes spaces within logical quantifiers (E, A).
 * @param str The string to process.
 */
void remove_spaces(char* str);
/**
 * @brief Gets the priority of an NFA operation.
 * @param op The operation character.
 * @return The priority of the operation.
 */
int nfa_get_priority(char op);
/**
 * @brief Handles an NFA operation from the stack.
 * Applies the specified operation to the top elements of the stack.
 * Supports operations: intersection (&), union (|), complement (~), right quotient (/), left quotient (\), exist (E), and forall (A).
 * @param stack The NFA stack.
 * @param operation The operation string.
 * @param global_structure The global NFA variables structure.
 * @return true if the operation was handled successfully, false otherwise.
 */
bool handle_operation(NFA_stack* stack, char* operation, NFA_variables* global_structure);
/**
 * @brief Converts a formula to Reverse Polish Notation (RPN) for NFA.
 * Converts an infix entry formula to a postfix (RPN) one to simplify the creation of an NFA
 * @param formula The formula string.
 * @return The RPN string.
 */
char* NFA_RPN(const char* formula);
/**
 * @brief Creates an NFA from a logical predicate.
 * Parses the predicate and constructs the corresponding NFA.
 * @param predicate The predicate string.
 * @return Pointer to the created NFA.
 */
NFA* NFA_from_predicate(const char* predicate);
/**
 * @brief Synchronizes the structures of two NFAs.
 * Swaps common variables and extends coordinates to match the structure of both NFAs.
 * @param main_nfa Pointer to the main NFA.
 * @param sub_nfa Pointer to the sub NFA.
 * @param main_vars Pointer to the main NFA variables.
 * @param sub_vars Pointer to the sub NFA variables.
 */
void sync_nfa_structure(NFA** main_nfa, NFA** sub_nfa, NFA_variables* main_vars, NFA_variables* sub_vars);
/**
 * @brief Unions terms in one NFA to have as many "y" as number of terms in the given list.
 * Example: term1 = "y1 = 2a + b", term2 = "y2 = b + a + 3c". Resulting NFA accepts "y1, y2, a, b, c" and checks if they satisfy both terms.
 * @param terms_count Number of terms.
 * @param terms Array of pointers to linear expressions.
 * @param unioned_vars Pointer to the resulting unioned variables.
 * @return Pointer to the resulting NFA.
 */
NFA* union_terms(int terms_count, linear_expression** terms, NFA_variables** unioned_vars);
/**
 * @brief Merges an NFA and its structure with global structures.
 * Completes all_vars with new variables, reorders local_vars based on all_vars, and extends/reorders coordinates in added_nfa.
 * @param added_nfa Pointer to the NFA to be added.
 * @param all_vars Pointer to all variables.
 * @param local_vars Pointer to the local variables.
 * @return The size of the extension (number of coordinates added).
 */
int merge_nfa_and_structure(NFA** added_nfa, NFA_variables* all_vars, NFA_variables* local_vars);

/**
 * @brief Creates an NFA from a regular expression.
 * Parses the regular expression and constructs the corresponding NFA.
 *
 * Regular expression syntax:
 * - a|b <-> 'a' or 'b'
 * - a* <-> '', 'a', 'aa', ...
 * - a+ <-> 'a', 'aa', 'aaa', ...
 * - a? <-> '', 'a'
 * - . <-> char != '\n'
 *
 * @param regex The regular expression string.
 * @return Pointer to the created NFA.
 */
NFA* NFA_from_regex(const char* regex);
/**
 * @brief Handles an operation in a regular expression for NFA.
 * Applies the specified operation (concatenation, union, Kleene star, etc.) to the top elements of the stack.
 * @param stack The NFA stack.
 * @param op The operation character.
 */
void handle_operation_regex(NFA_stack* stack, char op);
/**
 * @brief Gets the precedence of a regular expression operator.
 * @param op The operator character.
 * @return The precedence of the operator.
 */
int precedence(char op);

/**
 * @brief Converts a regular expression to Reverse Polish Notation (RPN).
 * Converts infix notation regular expression to postfix (RPN) notation for easier evaluation.
 * @param regex The regular expression string.
 * @return The RPN string.
 */
char* regex_to_rpn(const char* regex);
/**
 * @brief Makes concatenation explicit in a regular expression.
 * Inserts explicit concatenation operators (&) between adjacent symbols in the regular expression.
 * This is done to simplify the transfer to the NFA
 * @param regex The regular expression string.
 * @return The modified regular expression with explicit concatenation.
 */
char* explicit_concatenation(const char* regex);

#endif //COMPUTER_PRACTICES_NFA_CONSOLE_APP_H
