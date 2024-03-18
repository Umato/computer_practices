//
// Created by Umato on 30.10.2023.
//

#ifndef COMPUTER_PRACTICE_BIG_INTEGER_H
#define COMPUTER_PRACTICE_BIG_INTEGER_H

#include <iostream>
#include <cassert>
#include <cstdarg>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>

const int prime[100] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
                        73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157,
                        163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241,
                        251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347,
                        349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439,
                        443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541};

using ull = unsigned long long;
using ll = long long;

using std::cout;
using std::string;
using std::max;
using std::swap;
using std::endl;

/**
 * @brief Defines a big integer data structure.
 *
 * This structure represents a large integer value, consisting of a length field, a pointer to an array of unsigned chars
 * representing the digits, and a sign indicating the number's sign.
 */
using big_int = struct big_int {
    size_t length;
    unsigned char *number;
    bool sign;
};

/**
 * @brief Defines a structure for list of big integers.
 *
 * This structure represents a list of big_int pointers, consisted of a double pointer to an array of 
 * big integers and their total amount.
 */
typedef struct big_int_list
{
    big_int** big_ints;
    size_t count;
} bigint_text;

/**
 * @brief Represents an RSA key structure.
 *
 * This structure holds the modulus and exponent used in RSA encryption.
 */
using rsa_key = struct {
    big_int *mod;
    big_int *exp;
};

#pragma region Memory Management
/**
 * @brief Creates new list of big_int pointers and fill it with given numbers
 *
 * @param count Amount of given numbers.
 * @param nums Given numbers.
 * @return Pointer to the newly created big_int_list object.
 */
big_int_list *big_int_list_init(size_t count, const int* nums);

/**
 * @brief Frees memory allocated for list of big_int pointers.
 *
 * @param bigint_list List of big_int pointers to be freed.
 */
void big_int_list_free(big_int_list* bigint_list);

/**
 * @brief Reads an integer in binary format and creates a big_int object from it.
 *
 * @param x Integer input.
 * @return Pointer to the newly created big_int number or NULL if allocation fails.
 */
big_int *big_int_get(const int x);

/**
 * @brief Reads a binary string and creates a big_int object from it.
 *
 * @param x Binary string input.
 * @return Pointer to the newly created big_int number or NULL if allocation fails.
 */
big_int *big_int_get(const char *x);

/**
 * @brief Reads a decimal string and creates a big_int object from it.
 *
 * @param x Decimal string input.
 * @return Pointer to the newly created big_int number or NULL if allocation fails.
 */
big_int *big_int_get_dec(const char *x);

/**
 * @brief Frees memory allocated for a big_int number.
 *
 * @param num Pointer to be freed from memory.
 */
void big_int_free(big_int *original);

/**
 * @brief Frees memory allocated for multiple big_int numbers.
 *
 * @param count The number of big_int pointers to be freed.
 * @param ... Variable argument list containing 'count' number of big_int pointers.
 */
void big_int_free(size_t count, ...);

/**
 * @brief Makes a copy of a big_int number.
 *
 * @param original The number to be copied.
 * @return A copy of the input number, or NULL if memory allocation fails.
 */
big_int *big_int_copy(const big_int *original);

/**
 * @brief Sets a new length (in number of bytes) to a big_int object and reallocates memory accordingly.
 *
 * @param num Pointer to the big_int object.
 * @param new_length The new length (in number of bytes).
 */
void big_int_set_length(big_int *num, const size_t new_length);

/**
 * @brief Reads a big_int numerical value from a file.
 *
 * @param filename The name (including path if not in the same directory) of the file to read from.
 * @return A pointer to the newly created big_int read from the file.
 */
big_int *read_big_int_from_file(const std::string &filename);

#pragma endregion

#pragma region Numeric Conversion

/**
 * @brief Creates a new big_int that is a slice (subset) of the input big_int from the start to end indices.
 *
 * @param n The input big_int from which to slice.
 * @param start The index at which the slice should start (inclusive).
 * @param end The index at which the slice should end (exclusive).
 * @return Pointer to the newly created big_int number that represents the slice, or NULL if allocation fails.
 */
big_int *big_int_slice(const big_int *n, size_t start, size_t end);

#pragma endregion

#pragma region Output

/**
 * @brief Prints a big_int object in binary format.
 *
 * @param num Big_int object to be printed.
 */
void big_int_print(const big_int *num);

/**
 * @brief Prints a big_int object in decimal format.
 *
 * @param num Pointer to the big_int object to be printed.
 */
void big_int_print_decimal(const big_int *num);

#pragma endregion

#pragma region Manipulation

/**
 * @brief Removes all leading zeroes from a big_int number.
 *
 * @param num The big_int number to be altered.
 */
void big_int_remove_zeroes(big_int *num);

/**
 * @brief Converts a big_int to a string representation.
 *
 * @param num A pointer to the big_int number structure.
 * @return A string representation of the big_int number, or an empty string if the input is null.
 */
const char* big_int_to_string(const big_int *num);

#pragma endregion

#pragma region Computation

/**
 * @brief Computes the length in bits of the big_int number.
 *
 * @param num Pointer to the big_int object.
 * @return The number of bits required to express the big_int, or 0 if the input big_int is NULL.
 */
size_t big_int_length(const big_int *num);

/**
 * @brief Adds two big_int numbers.
 *
 * @param n1 The first big_int number.
 * @param n2 The second big_int number.
 * @return Sum of n1 and n2, or NULL if memory allocation fails.
 */
big_int *big_int_add(const big_int *n1, const big_int *n2);

/**
 * @brief Subtracts n2 from n1.
 *
 * @param n1 The big_int number to be subtracted from.
 * @param n2 The big_int number to be subtracted.
 * @return Difference of n1 and n2, or NULL if memory allocation fails.
 */
big_int *big_int_sub(const big_int *n1, const big_int *n2);

/**
 * @brief Multiplies two big_int numbers.
 *
 * @param n1 The first big_int number.
 * @param n2 The second big_int number.
 * @return Product of n1 and n2, or NULL if memory allocation fails.
 */
big_int *big_int_mul(const big_int *n1, const big_int *n2);

/**
 * @brief Performs the division of two big integers (n1 and n2) by repeated subtraction and computes the quotient and remainder.
 *
 * @param n1 Pointer to the first big_int object (dividend).
 * @param n2 Pointer to the second big_int object (divisor).
 * @param quotient Pointer to a pointer storing the result of quotient after division.
 * @param remainder Pointer to a pointer storing the result of remainder after division.
 */
void big_int_div_by_sub(const big_int *n1, const big_int *n2, big_int **quotient, big_int **remainder);

/**
 * @brief Multiplies two big_int numbers using Karatsuba algorithm for efficient multiplication.
 *
 * @param n1 The first big_int number.
 * @param n2 The second big_int number.
 * @return Product of n1 and n2 computed via Karatsuba algorithm, or NULL if memory allocation fails.
 */
big_int *big_int_mul_karatsuba(const big_int *n1, const big_int *n2);

/**
 * @brief Performs the modulo operation on two big integers (n1 and n2), returning the remainder after division of n1 by n2.
 *
 * @param num1 Pointer to the first big_int object (dividend).
 * @param num2 Pointer to the second big_int object (divisor).
 * @return Remainder of the division of n1 by n2, or NULL if memory allocation fails.
 */
big_int *big_int_mod(const big_int *num1, const big_int *num2);

/**
 * \brief Performs division of two big integers (n1 and n2) and returns only quotient.
 *
 * \param n1 Pointer to the first big_int object (dividend).
 * \param n2 Pointer to the second big_int object (divisor).
 * \return Quotient of the division of n1 by n2, or NULL if memory allocation fails.
 */
big_int *big_int_div_quotient(const big_int *n1, const big_int *n2);

/**
 * \brief Performs the division of two big integers (N and D) and computes the quotient and remainder.
 *
 * \param num1 Pointer to the first big_int object (dividend).
 * \param num2 Pointer to the second big_int object (divisor).
 * \param quotient Pointer to a pointer storing the result of quotient after division.
 * \param remainder Pointer to a pointer storing the result of remainder after division.
 */
void big_int_div(const big_int *num1, const big_int *num2, big_int **quotient, big_int **remainder);

/**
 * \brief Performs a binary right shift on a big_int number.
 *
 * \param num The number to be shifted.
 * \param n The number of positions to shift.
 * \return Result of right shifting num by n positions, or NULL if memory allocation fails.
 */
big_int *big_int_shift_right(const big_int *num, ll n);

/**
 * \brief Performs a binary left shift on a big_int number.
 *
 * \param num The number to be shifted.
 * \param n The number of positions to shift.
 * \return Result of left shifting num by n positions, or NULL if memory allocation fails.
 */
big_int *big_int_shift_left(const big_int *num, ll n);

/**
 * \brief Get the absolute value of a big_int number.
 *
 * \param n The big_int number from which the absolute value is to be calculated.
 * \return Absolute value of the big_int number.
 */
big_int *big_int_abs(const big_int *n);

/**
 * \brief Finds the Greatest Common Divisor of two big_int numbers using binary Euclidean algorithm.
 *
 * \param x The first big_int number.
 * \param y The second big_int number.
 * \return Greatest Common Divisor of x and y, or NULL if memory allocation fails.
 */
big_int *big_int_euclid_binary(const big_int *x, const big_int *y);

/**
 * \brief Raise a number to the power modulo another number.
 *
 * \param base The base number.
 * \param exponent The exponent.
 * \param mod The modulating number.
 * \return The calculated power modulo, or NULL if memory allocation fails.
 */
big_int *big_int_mod_pow(const big_int *base, const big_int *exponent, const big_int *mod);

/**
 * \brief Raise a number to the power modulo another number.
 *
 * \param base The base number.
 * \param exponent The exponent.
 * \param mod The modulating number.
 * \return The calculated power modulo, or NULL if memory allocation fails.
 */
big_int *big_int_lr_mod_pow(const char base, const big_int *exponent, const big_int *mod);

/**
 * \brief Raise a number to the power modulo another number.
 *
 * \param base The base number.
 * \param exponent The exponent.
 * \param mod The modulating number.
 * \return The calculated power modulo, or NULL if memory allocation fails.
 */
big_int *big_int_lr_mod_pow(const big_int *base, const big_int *exponent, const big_int *mod);

#pragma endregion

#pragma region Comparisons

/**
 * \brief Checks if n1 is greater than or equal to n2.
 *
 * \param n1 The first big_int number.
 * \param n2 The second big_int number.
 * \return true if n1 is greater than or equal to n2, false otherwise.
 */
bool big_int_geq(const big_int *n1, const big_int *n2);

/**
 * \brief Checks if the absolute value of n1 is greater than or equal to the absolute value of n2.
 *
 * \param n1 The first big_int number.
 * \param n2 The second big_int number.
 * \return true if |n1| is greater than or equal to |n2|, false otherwise.
 */
bool big_int_abs_geq(const big_int *n1, const big_int *n2);

/**
 * \brief Checks if n1 is equal to n2.
 *
 * \param n1 The first big_int number.
 * \param n2 The second big_int number.
 * \return true if n1 is equal to n2, false otherwise.
 */
bool big_int_eq(const big_int *n1, const big_int *n2);

#pragma endregion

#pragma region Void_Functions

/**
 * \brief Add the second big_int (n2) to the first one (n1). The result is stored in n1.
 *
 * \param n1 Pointer to the first big_int object. It also stores the result of the addition.
 * \param n2 Pointer to the second big_int object to be added.
 */
void big_int_add_void(big_int **n1, const big_int *n2);

/**
 * \brief Subtracts the second big_int (n2) from the first one (n1). The result is stored in n1.
 *
 * \param n1 Pointer to the first big_int object. It also stores the result of the subtraction.
 * \param n2 Pointer to the second big_int object to be subtracted.
 */
void big_int_sub_void(big_int **n1, const big_int *n2);

/**
 * \brief Shifts a big_int right by a certain number of bits.
 *
 * \param num Pointer to the big_int object to be shifted.
 * \param n The number of bits to shift right.
 */
void big_int_shift_right_void(big_int **num, ll n);

/**
 * \brief Shifts a big_int left by a certain number of bits.
 *
 * \param n1 Pointer to the big_int object to be shifted.
 * \param n The number of bits to shift left.
 */
void big_int_shift_left_void(big_int **n1, ll n);

/**
 * \brief Multiplies the first big_int (n1) with the second one (n2). The result is stored in n1.
 *
 * \param n1 Pointer to the first big_int object. It also stores the result of multiplication.
 * \param n2 Pointer to the second big_int object to be multiplied.
 */
void big_int_mul_void(big_int **n1, const big_int *n2);

/**
 * \brief Performs in-place addition of the second big_int (n2) to the first one (n1).
 *
 * \param n1 Pointer to the first big_int object which will also store the result of the addition.
 * \param n2 Pointer to the second big_int object to be added.
 */
void big_int_add_void(big_int *n1, const big_int *n2);

/**
 * \brief Performs in-place subtraction of the second big_int (n2) from the first one (n1).
 *
 * \param n1 Pointer to the first big_int object which will also store the result of the subtraction.
 * \param n2 Pointer to the second big_int object to be subtracted.
 */
void big_int_sub_void(big_int *n1, const big_int *n2);

/**
 * \brief Performs in-place left binary shift on a big_int object.
 *
 * \param num Pointer to the big_int object to be shifted.
 * \param n Number of bits to shift left.
 */
void big_int_shift_left_void(big_int *num, ll n);

/**
 * \brief Performs in-place right binary shift on a big_int object.
 *
 * \param num Pointer to the big_int object to be shifted.
 * \param n Number of bits to shift right.
 */
void big_int_shift_right_void(big_int *num, ll n);

/**
 * \brief Performs in-place multiplication of the first big_int (n1) with the second one (n2).
 *
 * \param n1 Pointer to the first big_int object which will also store the result of the multiplication.
 * \param n2 Pointer to the second big_int object to be multiplied.
 */
void big_int_mul_void(big_int *n1, const big_int *n2);

/**
 * \brief Shifts the given big_int object to the left by one place.
 *
 * \param n The big_int object to be shifted.
 */
void big_int_shift_left_once(big_int *n);

/**
 * \brief Shifts the given big_int object to the right by one place.
 *
 * \param n The big_int object to be shifted.
 */
void big_int_shift_right_once(big_int *n);

#pragma endregion

#pragma region Slavik

big_int *big_int_karatsuba_mult2(big_int *n1, big_int *n2);

big_int *big_int_slice2(big_int *n1, long l1, long l2);

void big_int_bin_shft_l(big_int *n);

void big_int_bin_shft_l2(big_int *n, unsigned int cnt);

void big_int_bin_shft_r(big_int *n);

void big_int_bin_shft_r2(big_int *n, int cnt);

big_int *big_int_lr_mod_pow2(big_int *x, big_int *n, big_int *m);

int big_int_leq(big_int *n1, big_int *n2);

void big_int_div2_for_pow(big_int *n1, big_int *n2, big_int *rmdr);

big_int *big_int_add_sl(big_int *n1, big_int *n2);

void big_int_add2(big_int *n1, big_int *n2);

big_int *big_int_sub_sl(big_int *n1, big_int *n2);

void big_int_sub2(big_int *n1, big_int *n2);

#pragma endregion

#pragma region Big_Int_Generate_Functions

/**
 * \brief Create a big_int object with a random number of specified length.
 *
 * \param bytes_num The number of bytes for the new big_int.
 * \return A pointer to the newly created big_int object or NULL if memory allocation fails.
 */
big_int *big_int_rnd(size_t bytes_num);

/**
 * \brief Generate a prime number of specified length.
 *
 * \param bytes_num The number of bytes for the new prime big_int.
 * \return A pointer to the newly created prime big_int or NULL if memory allocation fails.
 */
big_int *generate_big_int_prime(size_t bytes_num);

/**
 * \brief Generate a big_int number within a range.
 *
 * \param n1 Pointer to the first bound (inclusive).
 * \param n2 Pointer to the second bound (exclusive).
 * \return A pointer to the newly created big_int object within the range n1 - n2 or NULL if memory allocation fails.
 */
big_int *big_int_generate_range(const big_int *n1, const big_int *n2);

/**
 * \brief Create a big_int object with a random odd number of specified length.
 *
 * \param bytes_num The number of bytes for the new big_int.
 * \return A pointer to the newly created big_int object or NULL if memory allocation fails.
 */
big_int *big_int_rnd_odd(size_t bytes_num);

/**
 * \brief Generate a big_int prime of specified length starting from an odd number consisting of 1's and incrementing by 2 until a prime number is found.
 *
 * \param bytes_num The number of bytes for the new prime big_int.
 * \return A pointer to the newly created prime big_int.
 */
big_int *generate_big_int_prime2(size_t bytes_num);

/**
 * \brief Generate a big_int prime of specified length starting from a random odd number and generating a new random odd number if the current one is not a prime.
 *
 * \param bytes_num The number of bytes for the new prime big_int.
 * \return A pointer to the newly created prime big_int.
 */
big_int *generate_big_int_prime3(size_t bytes_num);

/**
 * \brief Generate a big_int prime of specified length starting from a random odd number, checking that it is not divisible by the first 100 prime numbers and is a probable prime using the Miller-Rabin test.
 * If the current number is not a prime, it adds 2 to the current number and retests.
 *
 * \param bytes_num The number of bytes for the new prime big_int.
 * \return A pointer to the newly created prime big_int.
 */
big_int *generate_big_int_prime4(size_t bytes_num);

/**
 * \brief Generate a big_int prime of specified length starting from a random odd number, checking that it is not divisible by the first 100 prime numbers and is a probable prime using the Miller-Rabin test.
 * If the current number is not a prime, it adds 2 to the current number and retests.
 *
 * \param bytes_num The number of bytes for the new prime big_int.
 * \return A pointer to the newly created prime big_int.
 */
big_int *generate_big_int_prime5(size_t bytes_num);

/**
 * \brief Generate a specified count of big_int primes of specified size using the strategy from the generate_big_int_prime5 function and save them into a text file.
 *
 * \param size Number of bytes for each new prime big_int.
 * \param count Number of prime numbers to generate.
 * \return None.
 * \remark This function saves generated primes into the text file located at ./primes/{size}.txt
 */
void generate_and_save_primes(const size_t size, const size_t count);

#pragma endregion

#pragma region RSA_Functions

/**
 * \brief Perform the Miller-Rabin primality test.
 *
 * \param num The big_int number to test.
 * \param iterations The number of iterations for the test.
 * \return true if the number is probably a prime, false otherwise.
 */
bool miller_rabin_test_big_int(const big_int *num, size_t iterations);

/**
 * \brief Perform the extended Euclidean algorithm on two big_int numbers.
 * The extended Euclidean algorithm computes, in addition to the greatest common divisor of integers a and b,
 * the coefficients of Bezout's identity.
 *
 * \param m Pointer to the first big_int object.
 * \param n Pointer to the second big_int object.
 * \param x Pointer to a pointer to store the result of the x coefficient of Bezout's identity (ax + by = gcd(a, b)).
 * \param y Pointer to a pointer to store the result of the y coefficient of Bezout's identity (ax + by = gcd(a, b)).
 * \return The greatest common divisor of m and n as a new big_int object, or NULL if memory allocation fails.
 */
big_int *big_int_euclid_extended(const big_int *m, const big_int *n, big_int **x, big_int **y);

/**
 * \brief Find the modular multiplicative inverse of a big_int number under a given modulus.
 *
 * \param a The big_int number for which to find the modular inverse.
 * \param modulus The modulus under which the inverse is to be found.
 * \return The modular multiplicative inverse of a under modulus, or NULL if the inverse doesn't exist or memory allocation fails.
 */
big_int *big_int_mod_inverse(const big_int *num, const big_int *modulus);

/**
 * \brief Decrypt a ciphertext using RSA decryption with the provided private key.
 *
 * \param ciphertext The data to be decrypted as a big_int object.
 * \param private_key The private key to be used for decryption.
 * \return Decrypted data as a new big_int object, or NULL if memory allocation fails.
 */
void generate_rsa_keys(const size_t bytes_num, rsa_key **public_key, rsa_key **private_key);

/**
 * \brief Encrypt a plaintext using RSA encryption with the provided public key.
 *
 * \param plaintext The data to be encrypted as a big_int object.
 * \param public_key The public key to be used for encryption.
 * \return Encrypted data as a new big_int object.
 */
big_int *rsa_encrypt(const big_int *plaintext, const rsa_key *public_key);

/**
 * \brief Decrypt a ciphertext using RSA decryption with the provided private key.
 *
 * \param ciphertext The data to be decrypted as a big_int object.
 * \param private_key The private key to be used for decryption.
 * \return Decrypted data as a new big_int object.
 */
big_int *rsa_decrypt(const big_int *ciphertext, const rsa_key *private_key);

/**
 * \brief Finds the modular multiplicative inverse of a big_int number under a given modulus.
 *
 * \param a The big_int number for which to find the modular inverse.
 * \param modulus The modulus under which the inverse is to be found.
 * \return The modular multiplicative inverse of a under modulus, or NULL if inverse doesn't exist or memory allocation fails.
 */
big_int *big_int_mod_inverse1(const big_int *num, const big_int *modulus);
big_int* big_int_from_char(const char* text);
void big_int_print_text(big_int* text);
void parser_rsa();
void console_app();
void hlp_f();
void handle_encode_command(char *cmd);
void handle_decode_command(char* cmd);
void RSA_key_free(rsa_key* key);
void handle_help_command();
void handle_generate_command(const char *name, size_t key_length);
void handle_add_key_command(const char *name, const char *binary_key);
void handle_addkey_command(char *cmd);
big_int* get_public_key(char* name);
void save_encrypted_message(char* name, big_int* encrypted_msg);
void save_rsa_keys(const char *name, const rsa_key *public_key, const rsa_key *private_key);
void handle_show_all_public_keys_command();
void handle_show_public_key_command(const char *name);

#pragma endregion

#pragma region GLOBAL

/**
 * \brief The big_int object representing the number 1.
 */
extern const big_int *const one;

/**
 * \brief The big_int object representing the number 0.
 */
extern const big_int *const zero;

/**
 * \brief The big_int object representing the number 2.
 */
extern const big_int *const two;

/**
 * \brief The big_int object representing F_4(65537).
 */
extern const big_int *const e;

extern const big_int *const mod;

extern const big_int *const ex;

#pragma endregion

#endif //COMPUTER_PRACTICE_BIG_INTEGER_H
