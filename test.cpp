//
// Created by Umato on 30.10.2023.
//

#include "test.h"

#pragma region Test_Common_Functions

void test_euclid() {
    ll a;
    ll b;
    cin >> a >> b;

    ll resultEuclid = euclid(a, b);
    cout << "GCD " << a << " and " << b << " with 'euclid' is " << resultEuclid << "\n";

    ll resultEuclidRec = euclid_recursive(a, b);
    cout << "GCD " << a << " and " << b << " with 'euclid_recursive' is " << resultEuclidRec << "\n";

    ll resultEuclidBinary = euclid_binary(a, b);
    cout << "GCD " << a << " and " << b << " with 'euclid_binary' is " << resultEuclidBinary << "\n";
}

void test_mod_pow() {
    long long x;
    long long y;
    long long m;
    cin >> x >> y >> m;

    long long resultModPow = mod_pow(x, y, m);
    cout << "Result of " << x << "^" << y << " mod " << m << " with mod_pow is " << resultModPow << "\n";

    long long resultModPowRec = mod_pow_recursive(x, y, m);
    cout << "Result of " << x << "^" << y << " mod " << m << " with mod_pow_recursive is " << resultModPowRec << "\n";

    long long resultLrModPow = lr_mod_pow(x, y, m);
    cout << "Result of " << x << "^" << y << " mod " << m << " with lr_mod_pow is " << resultLrModPow << "\n";
}

void test_euclid_extended() {
    ll a;
    ll b;
    cin >> a >> b;

    ll x;
    ll y;

    ll gcdEuclidExtended = euclid_extended_recursive(a, b, x, y);
    cout << "GCD " << a << " and " << b << " with recursive extended Euclid is " << gcdEuclidExtended;
    cout << ", Bezout coefficients: x = " << x << ", y = " << y << "\n";

    ll gcdEuclidExtendedd = euclid_extended_iterative(a, b, x, y);
    cout << "GCD " << a << " and " << b << " with iterative extended Euclid is " << gcdEuclidExtendedd;
    cout << ", Bezout coefficients: x = " << x << ", y = " << y << "\n";
}

void test_linear_diophantine() {
    ll n;
    cin >> n;

    auto *coefficients = new ll[n];
    auto *result = new ll[n];

    for (size_t i = 0; i < n; i++) {
        cin >> coefficients[i];
    }

    ll target;
    cin >> target;

    ll success = linear_diophantine(coefficients, n, result, target);
    if (success) {
        for (int j = 0; j < n; j++) {
            cout << "x" << j + 1 << " = " << result[j] << "\n";
        }
    } else {
        cout << "\\----\\\n";
    }

    delete[] coefficients;
    delete[] result;
}

void test_mul_inverse() {
    ll num;
    ll mod_val;
    cin >> num >> mod_val;

    ll result = mul_inverse(num, mod_val);

    if (result == 0) {
        cout << num << " ---- " << mod_val << "\n";
    } else {
        cout << "Inverse " << num << " mod " << mod_val << " = " << result << "\n";
    }
}

void test_chinese_reminder() {
    ll num_equations;
    cin >> num_equations;

    auto *remainders = (ll *) malloc(num_equations * sizeof(ll));
    auto *modules = (ll *) malloc(num_equations * sizeof(ll));

    for (int i = 0; i < num_equations; i++) {
        cin >> remainders[i];
    }
    for (int i = 0; i < num_equations; i++) {
        cin >> modules[i];
    }

    ll resultChineseReminderr = chinese_remainder_iterative(remainders, modules, num_equations);
    cout << "Solution with chinese_remainder_iterative: " << resultChineseReminderr << "\n";

    ll resultChineseReminder = chinese_reminder(remainders, modules, num_equations);
    cout << "Solution with chinese_reminder: " << resultChineseReminder << "\n";

    free(remainders);
    free(modules);
}

void test_is_prime() {
    ll num;
    cin >> num;

    cout << num << " is " << (is_prime(num) ? "prime\n" : "not prime\n");
}

#pragma endregion

#pragma region Test_Big_Int_Functions

void test_big_int_add() {
    std::ifstream in("add.txt");

    ll start = clock();

    std::string num1Str;
    std::string num2Str;
    std::string resStr;
    while (in >> num1Str && in >> num2Str && in >> resStr) {
        big_int *num1 = big_int_get(num1Str.c_str());
        big_int *num2 = big_int_get(num2Str.c_str());
        big_int *expectedResult = big_int_get(resStr.c_str());

        big_int *actualResult = big_int_add(num1, num2);

        if (!big_int_eq(expectedResult, actualResult)) std::cout << "Error: add\n";
        big_int_free(4, num1, num2, actualResult, expectedResult);
    }

    in.close();
    ll end = clock();
    double execution_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Add time: " << execution_time << endl;
}

void test_big_int_mul() {
    std::ifstream in("mul.txt");

    ll start = clock();

    std::string num1Str;
    std::string num2Str;
    std::string resStr;
    while (in >> num1Str && in >> num2Str && in >> resStr) {
        big_int *num1 = big_int_get(num1Str.c_str());
        big_int *num2 = big_int_get(num2Str.c_str());
        big_int *expectedResult = big_int_get(resStr.c_str());

        big_int *actualResult = big_int_mul(num1, num2);

        if (!big_int_eq(expectedResult, actualResult)) std::cout << "Error: mul\n";
        big_int_free(4, num1, num2, actualResult, expectedResult);
    }

    in.close();
    ll end = clock();
    double execution_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Mul time: " << execution_time << endl;
}

void test_big_int_mul_karatsuba() {
    std::ifstream in("mul.txt");

    ll start = clock();

    std::string num1Str;
    std::string num2Str;
    std::string resStr;
    while (in >> num1Str && in >> num2Str && in >> resStr) {
        big_int *num1 = big_int_get(num1Str.c_str());
        big_int *num2 = big_int_get(num2Str.c_str());
        big_int *expectedResult = big_int_get(resStr.c_str());

        big_int *actualResult = big_int_mul_karatsuba(num1, num2);

        if (!big_int_eq(expectedResult, actualResult)) std::cout << "Error: mul karatsuba\n";

        big_int_free(4, num1, num2, actualResult, expectedResult);
    }

    in.close();
    ll end = clock();
    double execution_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Mul karatsuba time: " << execution_time << endl;
}

void test_big_int_shift_left() {
    std::ifstream in("shift_left.txt");
    string str_number;
    string str_expected;
    size_t n;

    ll start = clock();

    while (in >> str_number >> n >> str_expected) {
        big_int *original = big_int_get(str_number.c_str());
        big_int *shifted = big_int_shift_left(original, n);
        big_int *expected = big_int_get(str_expected.c_str());

        if (!big_int_eq(shifted, expected)) cout << "Error: shift left\n";

        big_int_free(3, original, shifted, expected);

    }
    in.close();

    ll end = clock();
    double execution_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Shift left time: " << execution_time << endl;
}

void test_big_int_shift_right() {
    ifstream in("shift_right.txt");
    string str_number;
    string str_expected;
    size_t n;

    ll start = clock();

    while (in >> str_number >> n >> str_expected) {
        big_int *original = big_int_get(str_number.c_str());
        big_int *shifted = big_int_shift_right(original, n);
        big_int *expected = big_int_get(str_expected.c_str());

        if (!big_int_eq(shifted, expected)) cout << "Error: shift right\n";

        big_int_free(3, original, shifted, expected);

    }
    in.close();

    ll end = clock();
    double execution_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Shift right time: " << execution_time << endl;
}

void test_big_int_div() {
    ifstream infile("div.txt");
    string str_number1;
    string str_number2;
    string str_expected_quotient;
    string str_expected_remainder;

    ll start = clock();

    while (infile >> str_number1 >> str_number2 >> str_expected_quotient >> str_expected_remainder) {
        big_int *number1 = big_int_get(str_number1.c_str());
        big_int *number2 = big_int_get(str_number2.c_str());
        big_int *expected_quotient = big_int_get(str_expected_quotient.c_str());
        big_int *expected_remainder = big_int_get(str_expected_remainder.c_str());

        big_int *actual_quotient;
        big_int *actual_remainder;
        big_int_div(number1, number2, &actual_quotient, &actual_remainder);

        if (!big_int_eq(actual_quotient, expected_quotient) || !big_int_eq(actual_remainder, expected_remainder))
            cout << "Error: div\n";

        big_int_free(6, number1, number2, expected_quotient, expected_remainder, actual_quotient, actual_remainder);
    }

    infile.close();
    ll end = clock();
    double execution_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Div time: " << execution_time << endl;
}

void test_big_int_mod() {
    ifstream infile("div.txt");
    string str_number1;
    string str_number2;
    string str_expected_quotient;
    string str_expected_remainder;

    ll start = clock();

    while (infile >> str_number1 >> str_number2 >> str_expected_quotient >> str_expected_remainder) {
        big_int *number1 = big_int_get(str_number1.c_str());
        big_int *number2 = big_int_get(str_number2.c_str());
        big_int *expected_remainder = big_int_get(str_expected_remainder.c_str());

        big_int *actual_remainder = big_int_mod(number1, number2);

        //if (!big_int_eq(actual_remainder, expected_remainder)) cout << "Error: mod\n";

        big_int_free(4, number1, number2, expected_remainder, actual_remainder);
    }

    infile.close();
    ll end = clock();
    double execution_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Mod time: " << execution_time << endl;
}

void test_big_int_slice() {
    std::ifstream in("slice.txt");

    ll start = clock();

    std::string num1Str;
    size_t start1;
    size_t end1;
    std::string resStr;
    while (in >> num1Str && in >> start1 && in >> end1 && in >> resStr) {
        big_int *num1 = big_int_get(num1Str.c_str());
        big_int *expectedResult = big_int_get(resStr.c_str());

        big_int *actualResult = big_int_slice(num1, start1, end1);

        if (!big_int_eq(expectedResult, actualResult)) std::cout << "Error: my slice\n";

        big_int_free(3, num1, actualResult, expectedResult);
    }

    in.close();
    ll end = clock();
    double execution_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Slice time: " << execution_time << endl;
}

void test_big_int_euclid_binary() {
    ifstream infile("gcd.txt");

    string str_number1;
    string str_number2;
    string str_expected_gcd;

    ll start = clock();


    while (infile >> str_number1 >> str_number2 >> str_expected_gcd) {
        big_int *number1 = big_int_get(str_number1.c_str());
        big_int *number2 = big_int_get(str_number2.c_str());
        big_int *expected_gcd = big_int_get(str_expected_gcd.c_str());

        big_int *gcd = big_int_euclid_binary(number1, number2);


        if (!big_int_eq(gcd, expected_gcd)) cout << "Error: binary euclid\n";

        big_int_free(4, number1, number2, gcd, expected_gcd);
    }

    infile.close();
    ll end = clock();
    double execution_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Binary Euclid time: " << execution_time << endl;
}

void test_big_int_mod_pow() {
    ifstream infile("mod_pow.txt");
    string base;
    string exponent;
    string mod;
    string result;

    ll start = clock();

    while (infile >> base >> exponent >> mod >> result) {
        big_int *b = big_int_get(base.c_str());
        big_int *er = big_int_get(exponent.c_str());
        big_int *m = big_int_get(mod.c_str());
        big_int *r = big_int_get(result.c_str());

        big_int *res = big_int_mod_pow(b, er, m);

        if (!big_int_eq(res, r)) cout << "Error: mod_pow\n";

        big_int_free(5, b, e, m, r, res);
    }

    infile.close();
    ll end = clock();
    double execution_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "mod_pow time: " << execution_time << endl;
}

void test_big_int_lr_mod_pow() {
    ifstream infile("mod_pow.txt");
    string base;
    string exponent;
    string mod;
    string result;

    ll start = clock();

    while (infile >> base >> exponent >> mod >> result) {
        big_int *b = big_int_get(base.c_str());
        big_int *er = big_int_get(exponent.c_str());
        big_int *m = big_int_get(mod.c_str());
        big_int *r = big_int_get(result.c_str());

        big_int *res = big_int_lr_mod_pow(b, er, m);

        if (!big_int_eq(res, r)) cout << "Error: lr_mod_pow\n";

        big_int_free(5, b, e, m, r, res);
    }

    infile.close();
    ll end = clock();
    double execution_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "lr_mod_pow time: " << execution_time << endl;
}

void test_big_int_miller_rabin() {
    std::ifstream in("miller_rabin.txt");
    string str_number;
    int b;

    ll start = clock();

    while (in >> str_number >> b) {
        big_int *original = big_int_get_dec(str_number.c_str());
        int a = miller_rabin_test_big_int(original, 10);

        if (a != b) cout << "Error: miller rabin\n";

        big_int_free(original);
        cout << str_number.c_str() << " Test passed\n";
    }
    in.close();

    ll end = clock();
    double execution_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Miller rabin time: " << execution_time << endl;
}

void test_all_big_int_functions() {
    ll start = clock();

    //test_big_int_add();
    //test_big_int_add2();
    //test_big_int_mul();
    //test_big_int_mul_karatsuba();
    //test_big_int_shift_left();
    //test_big_int_shift_right();
    //test_big_int_euclid_binary();
    //test_big_int_mod_pow();
    //test_big_int_lr_mod_pow();
    ////test_big_int_lr_mod_pow2();
    //test_big_int_div();
    //test_big_int_mod();
    //test_big_int_mod2();
    //test_big_int_slice();
    //test_big_int_euclid_extended();
    test_big_int_miller_rabin();

    ll end = clock();
    double execution_time = double(end - start) / CLOCKS_PER_SEC;
    std::cout << "Total Time: " << execution_time << endl;
}

#pragma endregion