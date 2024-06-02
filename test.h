//
// Created by Umato on 30.10.2023.
//

#ifndef COMPUTER_PRACTICE_TEST_H
#define COMPUTER_PRACTICE_TEST_H

#include <iostream>
#include "Algorithm.h"
#include "big_integer.h"
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

using ll = long long;

using std::cout;
using std::cin;
using std::string;
using std::endl;
using std::ifstream;

#pragma region Test_Common_Functions

void test_euclid();

void test_mod_pow();

void test_euclid_extended();

void test_linear_diophantine();

void test_mul_inverse();

void test_chinese_reminder();

void test_is_prime();

#pragma endregion

#pragma region Test_Big_Int_Functions

void test_big_int_add();

void test_big_int_mul();

void test_big_int_mul_karatsuba();

void test_big_int_shift_right();

void test_big_int_shift_left();

void test_big_int_div();

void test_big_int_mod();

void test_big_int_slice();

void test_big_int_euclid_binary();

void test_big_int_mod_pow();

void test_big_int_lr_mod_pow();

void test_big_int_miller_rabin();

void test_all_big_int_functions();

#pragma endregion

#endif //COMPUTER_PRACTICE_TEST_H
