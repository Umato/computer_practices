//
// Created by Umato on 30.10.2023.
//

#include <sstream>
#include "big_integer.h"

#pragma region Big_int_list
big_int_list* big_int_list_init(size_t count, const int* nums)
{
    big_int_list* bigint_list = (big_int_list*)malloc(sizeof(big_int_list));
    bigint_list->count = count;
    bigint_list->big_ints = (big_int**)malloc(sizeof(big_int*) * count);
    for (int i = 0; i < count; i++)
    {
        bigint_list->big_ints[i] = big_int_get(nums[i]);
    }
    return bigint_list;
}

big_int_list* big_int_list_init(size_t count, int bytes_count)
{
    big_int_list* bigint_list = (big_int_list*)malloc(sizeof(big_int_list));
    bigint_list->count = count;
    bigint_list->big_ints = (big_int**)malloc(sizeof(big_int*) * count);
    for (int i = 0; i < count; i++)
    {
        bigint_list->big_ints[i] = big_int_init_zeroes(bytes_count);
    }
    return bigint_list;
}

void big_int_list_add_num(big_int_list* list, big_int* num)
{
    list->count++;
    list->big_ints = (big_int**)realloc(list->big_ints, sizeof(big_int*) * list->count);
    list->big_ints[list->count - 1] = num;
}

void big_int_list_free(big_int_list* bigint_list)
{
    for (int i = 0; i < bigint_list->count; i++)
    {
        big_int_free(bigint_list->big_ints[i]);
    }
    free(bigint_list->big_ints);
    free(bigint_list);
}

// NEED TO BE TESTED
void big_int_list_add_bit(big_int_list* list, int big_int_index, int bit_index)
{
    if (!list || big_int_index >= list->count || bit_index >= (list->big_ints[big_int_index]->length << 3)) return;

    list->big_ints[big_int_index]->number[bit_index >> 3] |= 1 << (bit_index & 7);
}

// NEED TO BE TESTED
int big_int_list_get_bit(big_int_list* list, int big_int_index, int bit_index)
{
    if (!list || big_int_index >= list->count || bit_index >= (list->big_ints[big_int_index]->length << 3)) return -1;
    return (list->big_ints[big_int_index]->number[bit_index >> 3] & (1 << (bit_index & 7))) != 0;
}
#pragma endregion

#pragma region Support Functions For NFA
// NEED TO BE TESTED
void big_int_add_bit(big_int* num, int bit_index)
{
    if (!num || bit_index >= (num->length << 3)) return;

    num->number[bit_index >> 3] |= 1 << (bit_index & 7);
}

// NEED TO BE TESTED
int big_int_get_bit(big_int* num, int bit_index)
{
    if (!num || bit_index >= (num->length << 3)) return -1;
    return (num->number[bit_index >> 3] & (1 << (bit_index & 7))) != 0;
}

// NEED TO BE TESTED
big_int* big_int_init_zeroes(int bytes_count)
{
    big_int* num = (big_int*)malloc(sizeof(big_int));
    num->length = bytes_count;
    num->sign = false;
    num->number = (unsigned char*)calloc(bytes_count, sizeof(unsigned char));

    return num;
}
#pragma endregion

#pragma region Memory Management
big_int *big_int_get(const int x) {
    big_int *num = (big_int *) malloc(sizeof(big_int));

    if (!num) return nullptr;
    num->sign = false;

    if (x < 0) num->sign = true;

    int new_x = abs(x);

    num->length = sizeof(int);

    num->number = (unsigned char *) malloc(num->length * sizeof(unsigned char));

    if (!num->number) {
        free(num);
        return nullptr;
    }

    for (size_t i = 0; i < num->length; i++) {
        num->number[i] = (new_x >> (i << 3));
    }

    big_int_remove_zeroes(num);
    return num;
}

big_int *big_int_get(const char *x) {
    if (!x) return nullptr; // Не указывает ли x на nullptr;

    big_int *num = (big_int *) malloc(sizeof(big_int));
    if (!num) return nullptr; // Если не удалось выделить память для y

    size_t l = strlen(x);
    num->sign = false;
    const char *new_x = x;
    if (x[0] == '-') {
        num->sign = true;
        new_x++;
        l--;
    }

    num->length = (l + 7) >> 3;

    num->number = (unsigned char *) calloc(num->length, sizeof(unsigned char));
    if (!num->number) {  // Не удалось выделить память для y->number
        free(num);
        return nullptr;
    }

    for (size_t i = 0; i < l; i++) {
        if (new_x[l - 1 - i] == '1') num->number[i >> 3] |= (1 << (i & 7));
        else if (new_x[l - 1 - i] != '0') {  // x[] не 1 и не 0
            big_int_free(num);
            return nullptr;
        }
    }
    new_x = nullptr;
    big_int_remove_zeroes(num);
    return num;
}

big_int *big_int_get_dec(const char *x) {
    if (!x) return nullptr;

    big_int *res = big_int_get("0");
    if (!res) return nullptr;

    big_int *ten = big_int_get("1010");
    if (!ten) {
        big_int_free(res);
        return nullptr;
    }
    for (const char *new_x = x + (*x == '-'); *new_x; ++new_x) {
        if (!isdigit(*new_x)) {
            big_int_free(res);
            big_int_free(ten);
            return nullptr;
        }

        unsigned char val = (unsigned char) (*new_x - '0');
        big_int *digit = big_int_get("0");
        digit->number[0] = val;

        big_int_mul_void(&res, ten);
        big_int_add_void(&res, digit);

        big_int_free(digit);
    }
    res->sign = *x == '-';
    big_int_free(ten);
    return res;
}

void big_int_free(big_int *num) {
    if (!num) return;
    num->length = 0;
    if (num->number) {
        free(num->number);
        num->number = nullptr;
    }
    free(num);
}

void big_int_free(size_t count, ...) {
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; ++i) {
        big_int *num = va_arg(args, big_int*);
        if (num) {
            num->length = 0;
            num->sign = false;
            if (num->number) {
                free(num->number);
                //num->number = nullptr;
            }
            free(num);
        }
    }

    va_end(args);
}

big_int *big_int_copy(const big_int *original) {
    if (!original) return nullptr;

    big_int *copy = (big_int *) malloc(sizeof(big_int));
    if (!copy) return nullptr;

    *copy = *original;

    copy->number = (unsigned char *) malloc(copy->length * sizeof(unsigned char));
    if (!copy->number) {
        big_int_free(copy);
        return nullptr;
    }
    // Копирование данных из исходного массива чисел в копию
    memcpy(copy->number, original->number, copy->length * sizeof(unsigned char));

    return copy;
}

void big_int_set_length(big_int *num, const size_t new_length) {
    if (!num) return;

    unsigned char *resized_number = (unsigned char *) realloc(num->number, new_length * sizeof(unsigned char));

    if (!resized_number) {
        big_int_free(num);
        return;
    }

    if (new_length > num->length) {
        size_t fill_length = new_length - num->length;
        memset(resized_number + num->length, 0, fill_length);
    }

    num->number = resized_number;
    num->length = new_length;
}

big_int *read_big_int_from_file(const std::string &filename) {
    std::ifstream infile(filename);

    std::string line;

    getline(infile, line);

    big_int *num = big_int_get(line.c_str());

    infile.close();

    return num;
}

#pragma endregion

#pragma region Output

void big_int_print(const big_int *num) {
    if (!num) return;

    const size_t total_bits = num->length << 3;
    char bit;

    if (num->sign) putchar('-');

    bool flag = false;
    for (int i = ((total_bits) - 1); i >= 0; --i) {
        bit = (num->number[i >> 3] & (1 << (i & 7))) ? '1' : '0';
        if (flag || bit == '1') {
            flag = true;
            putchar(bit);
        }
    }
    if (!flag) putchar('0');
    putchar('\n');
    return;
}

void big_int_print_decimal(const big_int *num) {
    if (!num) return;

    if (num->length == 1 && num->number[0] == 0) {
        cout << "0\n";
        return;
    }

    char *dec = (char *) calloc(num->length * 3, sizeof(char));
    if (!dec) return;
    big_int *copy = big_int_abs(num);
    if (!copy) {
        free(dec);
        return;
    }

    size_t dec_length = 0;
    big_int *ten = big_int_get("1010");
    if (!ten) {
        big_int_free(copy);
        free(dec);
        return;
    }
    big_int *quotient;
    big_int *remainder;
    while (!big_int_eq(copy, zero)) {
        big_int_div(copy, ten, &quotient, &remainder);
        if (!quotient || !remainder) {
            big_int_free(copy);
            big_int_free(ten);
            free(dec);
            return;
        }

        dec[dec_length++] = remainder->number[0] + '0';

        big_int_free(copy);
        big_int_free(remainder);
        copy = quotient;
    }

    if (num->sign) putchar('-');

    for (int i = dec_length - 1; i >= 0; i--) {
        putchar(dec[i]);
    }

    putchar('\n');

    big_int_free(copy);
    big_int_free(ten);
    free(dec);
}

void big_int_print_text(big_int* text) {
    for (size_t i = 0; i < text->length; i++) {
        putchar(text->number[i]);
    }
    putchar('\n');
}

#pragma endregion

#pragma region Manipulation

void big_int_remove_zeroes(big_int *num) {
    if (!num) return;

    size_t zeroes = 0;
    for (int i = num->length - 1; i >= 0 && num->number[i] == 0; i--) zeroes++;

    if (!zeroes) return;

    if (zeroes == num->length) {
        num->length = 1;
        num->number[0] = 0;
        num->sign = false;
        return;
    }

    if (zeroes > 0) {
        size_t new_len = num->length - zeroes;
        unsigned char *new_num = (unsigned char *) realloc(num->number, new_len * sizeof(unsigned char));
        if (!new_num) {
            num->length = new_len;
            return;
        }
        num->number = new_num;
        num->length = new_len;
    }
}

char* big_int_to_string(const big_int *num) {
    if (!num || num->length == 0)
    {
        char* empty_string = (char*)malloc(sizeof(char));
        empty_string[0] = '\0';
        return empty_string;
    }

    const size_t total_bits = num->length << 3;

    size_t str_size = total_bits + (num->sign ? 2: 1);
    char *result = (char*) malloc(str_size * sizeof(char));

    size_t index = 0;
    if (num->sign) {
        result[index++] = '-';
    }

    bool leading_zeroes = true;
    for (int i = (int)(total_bits) - 1; i >= 0; --i) {
        char bit = (num->number[i / 8] & (1 << (i % 8))) ? '1' : '0';
        if (leading_zeroes && bit == '1') {
            leading_zeroes = false;
        }
        if (!leading_zeroes) {
            result[index++] = bit;
        }
    }
    if (leading_zeroes) {
        result[index++] = '0';
    }

    result[index] = '\0';

    return result;
}

#pragma endregion

#pragma region Computation

size_t big_int_length(const big_int *num) {
    if (!num) return 0;
    size_t len = ((num->length - 1) << 3);
    bool flag = false;
    for (size_t i = 256; i >= 1; i >>= 1) {
        if (num->number[num->length - 1] & i) flag = true;
        len += flag;
    }
    return len;
}

big_int *big_int_add(const big_int *n1, const big_int *n2) {
    if (n1 == nullptr || n2 == nullptr) return nullptr;

    if (n1->sign == n2->sign) {
        size_t new_length = (n1->length > n2->length ? n1->length : n2->length) + 1;
        big_int *result = (big_int *) malloc(sizeof(big_int));
        if (!result) return nullptr;
        result->length = new_length;

        result->number = (unsigned char *) calloc(result->length, sizeof(unsigned char));
        if (!result->number) {
            free(result);
            return nullptr;
        }

        result->sign = n1->sign;
        unsigned char carry = 0;

        for (size_t i = 0; i < new_length; i++) {
            unsigned int sum = carry;

            if (i < n1->length) sum += n1->number[i];
            if (i < n2->length) sum += n2->number[i];

            result->number[i] = sum;
            carry = (sum >> 8);
        }

        if (carry) result->number[new_length - 1] = carry;
        else big_int_remove_zeroes(result);

        return result;
    } else {
        ((big_int *) n2)->sign = !n2->sign;
        big_int *result = big_int_sub(n1, n2);
        ((big_int *) n2)->sign = !n2->sign;
        if (!result) return nullptr;

        return result;
    }
}

big_int *big_int_sub(const big_int *n1, const big_int *n2) {
    if (!n1 || !n2) return nullptr;

    if (n1->sign == n2->sign) {
        if (big_int_abs_geq(n1, n2)) {
            size_t new_length = n1->length;
            big_int *result = (big_int *) malloc(sizeof(big_int));
            if (!result) return nullptr;

            result->length = new_length;
            result->number = (unsigned char *) calloc(result->length, sizeof(unsigned char));
            if (!result->number) {
                free(result);
                return nullptr;
            }

            result->sign = n1->sign;
            unsigned char borrow = 0;
            for (size_t i = 0; i < new_length; i++) {
                int sub = n1->number[i] - borrow;
                if (i < n2->length) sub -= n2->number[i];

                if (sub >= 0) {
                    borrow = 0;
                    result->number[i] = sub;
                } else {
                    result->number[i] = 0x100 + sub; // 1 0000 0000
                    borrow = 1;
                }
            }
            big_int_remove_zeroes(result);
            return result;
        } else {
            big_int *result = big_int_sub(n2, n1);
            if (!result) return nullptr;
            result->sign = !result->sign;
            return result;
        }
    } else {
        ((big_int *) n2)->sign = !n2->sign;
        big_int *result = big_int_add(n1, n2);
        ((big_int *) n2)->sign = !n2->sign;
        return result;
    }
    return nullptr;
}

big_int *big_int_mul(const big_int *n1, const big_int *n2) {
    if (!n1 || !n2) return big_int_get(0);

    size_t new_length = n1->length + n2->length;

    big_int *result = (big_int *) malloc(sizeof(big_int));
    if (!result) return nullptr;
    result->length = new_length;

    result->number = (unsigned char *) calloc(result->length, sizeof(unsigned char));
    if (!result->number) {
        free(result);
        return big_int_get(0);
    }

    for (size_t i = 0; i < n1->length; i++) {
        for (size_t j = 0; j < n2->length; j++) {
            unsigned short mult = n1->number[i] * n2->number[j];
            unsigned short carry = (result->number[i + j] + mult) >> 8;
            result->number[i + j] += mult;
            int k = 1;
            while (carry) {
                unsigned short new_carry = (result->number[i + j + k] + carry) >> 8;
                result->number[i + j + k] += carry;
                carry = new_carry;
                k++;
            }
        }
    }
    result->sign = !(big_int_eq(zero, result)) ? n1->sign ^ n2->sign : false;
    big_int_remove_zeroes(result);
    return result;
}

void big_int_div_by_sub(const big_int *n1, const big_int *n2, big_int **quotient, big_int **remainder) {
    if (!n1 || !n2 || big_int_eq(n2, zero)) {
        *quotient = big_int_get("0");
        *remainder = big_int_get("0");
        return;
    }

    big_int *r = big_int_abs(n1);
    big_int *q = big_int_get("0");
    q->sign = false;
    if (!r || !q) {
        big_int_free(r);
        big_int_free(q);
        return;
    }

    big_int *b = big_int_abs(n2);
    if (!b) {
        big_int_free(r);
        big_int_free(q);
        return;
    }

    big_int *tmp_r = nullptr;
    big_int *tmp_q = nullptr;

    while (big_int_geq(r, b)) {
        big_int_sub_void(&r, b);
        big_int_add_void(&q, one);
        if (!r || !q) {
            big_int_free(r);
            big_int_free(q);
            big_int_free(b);
            return;
        }
    }

    if (n1->sign && !big_int_eq(r, big_int_get("0"))) {
        big_int_sub_void(&r, b);
        big_int_add_void(&q, one);
        if (!r || !q) {
            big_int_free(r);
            big_int_free(q);
            big_int_free(b);
            return;
        }
    }

    big_int_free(b);
    q->sign = !big_int_eq(q, zero) ? n1->sign ^ n2->sign : false;
    r->sign = false;
    *quotient = q;
    *remainder = r;
    return;
}

void big_int_div(const big_int *num1, const big_int *num2, big_int **quotient, big_int **remainder) {
    if (!num1 || !num2) return;
    if (big_int_eq(num2, zero)) return;

    big_int *q = big_int_get("0");
    big_int *r = big_int_get("0");
    if (!q || !r) {
        big_int_free(q);
        big_int_free(r);
        return;
    }

    big_int_set_length(q, num1->length);

    const size_t total_bits = num1->length << 3;

    big_int *a = big_int_abs(num1);
    big_int *b = big_int_abs(num2);
    if (!a || !b) {
        big_int_free(a);
        big_int_free(b);
        big_int_free(q);
        big_int_free(r);
        return;
    }

    for (int i = total_bits - 1; i >= 0; --i) {
        big_int_shift_left_void(&r, 1);
        if (!r) {
            big_int_free(a);
            big_int_free(b);
            big_int_free(q);
            return;
        }
        r->number[0] |= ((num1->number[i >> 3] >> (i & 7)) & 1);

        if (big_int_geq(r, b)) {
            big_int_sub_void(&r, b);
            if (!r) {
                big_int_free(a);
                big_int_free(b);
                big_int_free(q);
                return;
            }
            q->number[i >> 3] |= (1 << (i & 7));
        }
    }

    if (num1->sign && !big_int_eq(r, zero)) {
        big_int_sub_void(&r, b);
        big_int_add_void(&q, one);
        if (!r || !q) {
            big_int_free(a);
            big_int_free(b);
            big_int_free(q);
            big_int_free(r);
            return;
        }
    }
    q->sign = false;
    q->sign = !big_int_eq(q, zero) ? num1->sign ^ num2->sign : false;
    r->sign = false;
    big_int_free(a);
    big_int_free(b);
    big_int_remove_zeroes(q);
    big_int_remove_zeroes(r);
    *quotient = q;
    *remainder = r;

}

big_int *big_int_mul_karatsuba(const big_int *n1, const big_int *n2) {
    if (n1->length + n2->length <= 100) return big_int_mul(n1, n2);

    //x = (n1->length + n2->length) >> 2
    size_t x = max(n1->length, n2->length);
    x /= 2;

    big_int *a = big_int_slice(n1, x, n1->length);
    a->sign = false;
    big_int *b = big_int_slice(n1, 0, x);
    b->sign = false;
    big_int *c = big_int_slice(n2, x, n2->length);
    c->sign = false;
    big_int *d = big_int_slice(n2, 0, x);
    d->sign = false;

    big_int *ac = big_int_mul_karatsuba(a, c); /*(a * c)*/
    big_int *bd = big_int_mul_karatsuba(b, d); /*(b * d)*/

    big_int_add_void(a, b); // a = a + b
    big_int_add_void(c, d); // c = c + d


    big_int *res = big_int_mul_karatsuba(a, c); // res = (a + b) * (c + d)
    big_int *ac_bd = big_int_add(ac, bd); // (ac + bd)

    big_int_sub_void(res, ac_bd); // res = (a + b) * (c + d) - ac_bd (ac_bd = ac + bd)

    big_int_shift_left_void(ac, (x << 4)); // ac * x ^ 2
    big_int_shift_left_void(res, (x << 3)); // res * x

    big_int_add_void(res, ac); // res = res + ac
    big_int_add_void(res, bd); // res = res + bd

    res->sign = !(big_int_eq(res, zero)) ? (n1->sign != n2->sign) : false;
    big_int_free(7, a, b, c, d, ac, bd, ac_bd);
    return res;
}

big_int *big_int_mod(const big_int *num1, const big_int *num2) {

    if (big_int_eq(num2, zero)) return big_int_get(0);

    big_int *r = big_int_get("0");

    const size_t total_bits = num1->length << 3;
    big_int *b = big_int_abs(num2);

    for (ll i = total_bits - 1; i >= 0; --i) {
        big_int_shift_left_once(r);
        r->number[0] |= ((num1->number[i >> 3] >> (i & 7)) & 1);

        if (big_int_geq(r, b)) {
            big_int_sub_void(r, b);
        }
    }

    if (num1->sign && !big_int_eq(r, zero)) {
        big_int_sub_void(&r, b);
    }

    r->sign = false;
    big_int_free(b);
    big_int_remove_zeroes(r);
    return r;
}

big_int *big_int_div_quotient(const big_int *n1, const big_int *n2) {
    if (!n1 || !n2) return nullptr;

    big_int *quotient = nullptr;
    big_int *reminder = nullptr;
    big_int_div(n1, n2, &quotient, &reminder);
    big_int_free(reminder);

    return quotient;
}

big_int *big_int_shift_right(const big_int *num, ll n) {
    if (!num || n < 0) return nullptr;

    ll bytes_shift = n / 8;
    ll bits_shift = n % 8;

    if (bytes_shift >= num->length) return big_int_get("0");

    big_int *new_num = big_int_copy(num);
    new_num->length -= bytes_shift;
    memmove(new_num->number, new_num->number + bytes_shift, new_num->length);

    if (bits_shift == 0) {
        return new_num;
    }

    unsigned char borrow = 0;
    unsigned char current;

    for (ll i = new_num->length - 1; i >= 0; i--) {
        current = new_num->number[i];
        new_num->number[i] = (current >> bits_shift) | (borrow << (8 - bits_shift));
        borrow = current & ((1 << bits_shift) - 1);
    }

    big_int_remove_zeroes(new_num);
    return new_num;
}

big_int *big_int_shift_left(const big_int *num, ll n) {
    if (!num || n < 0) return big_int_get("0");

    ll bytes_shift = n / 8;
    ll bits_shift = n % 8;

    size_t new_length = num->length + bytes_shift + (bits_shift ? 1 : 0);

    big_int *new_num = big_int_copy(num);
    if (!new_num) return nullptr;

    unsigned char *temp = (unsigned char *) realloc(new_num->number, new_length * sizeof(unsigned char));
    if (!temp) {
        big_int_free(new_num);
        return big_int_get("0");
    }
    new_num->number = temp;

    if (bits_shift > 0) {
        memset(new_num->number + num->length + bytes_shift, 0, sizeof(unsigned char));
    }
    if (bytes_shift > 0) {
        memmove(new_num->number + bytes_shift, new_num->number, num->length);
        memset(new_num->number, 0, bytes_shift);
    }

    if (bits_shift > 0) {
        for (size_t i = new_length - 1; i > bytes_shift; i--) {
            new_num->number[i] = (new_num->number[i] << bits_shift) | (new_num->number[i - 1] >> (8 - bits_shift));
        }
        new_num->number[bytes_shift] <<= bits_shift;
    }
    new_num->length = new_length;

    big_int_remove_zeroes(new_num);
    return new_num;

}

big_int *big_int_abs(const big_int *n) {
    if (!n) return nullptr;

    big_int *copy = big_int_copy(n);
    if (!copy) return nullptr;

    copy->sign = false;

    return copy;
}

big_int *big_int_euclid_binary(const big_int *x, const big_int *y) {
    if (!x || !y) return big_int_get("0");

    big_int *a = big_int_abs(x);
    big_int *b = big_int_abs(y);
    if (!a || !b) {
        big_int_free(2, a, b);
        return big_int_get("0");
    }


    if (big_int_eq(a, zero)) {
        big_int_free(a);
        return b;
    }

    if (big_int_eq(b, zero)) {
        big_int_free(b);
        return a;
    }

    size_t power = 0;
    while (!(b->number[0] & 1) && !(a->number[0] & 1)) {
        big_int_shift_right_void(&a, 1);
        big_int_shift_right_void(&b, 1);
        if (!a || !b) {
            big_int_free(2, a, b);
            return big_int_get(0);
        }
        power++;
    }

    while (!(a->number[0] == 0 && a->length == 1)) {
        //while (!big_int_eq(zero, a)) {
        while (!(b->number[0] & 1)) {
            big_int_shift_right_void(&b, 1);
            if (!b) {
                big_int_free(a);
                return big_int_get(0);
            }
        }
        while (!(a->number[0] & 1)) {
            big_int_shift_right_void(&a, 1);
            if (!a) {
                big_int_free(b);
                return big_int_get(0);
            }
        }
        if (!big_int_geq(a, b)) {
            swap(a, b);
        }
        big_int_sub_void(&a, b);
        if (!a) {
            big_int_free(b);
            return big_int_get(0);
        }
    }

    big_int_shift_left_void(&b, power);
    big_int_free(a);
    if (!b) {
        return big_int_get(0);
    }


    return b;
}

big_int *big_int_mod_pow(const big_int *base, const big_int *exponent, const big_int *mod) {
    if (!base || !exponent || !mod) {
        return nullptr;
    }
    big_int *result = big_int_get("1");

    big_int *base_copy = big_int_copy(base);

    big_int *exp_copy = big_int_copy(exponent);

    big_int *rem = big_int_mod(base_copy, mod);

    big_int_free(base_copy);
    base_copy = rem;

    while (!big_int_eq(exp_copy, zero)) {
        big_int *tmp = nullptr;
        if (exp_copy->number[0] & 1) {
            tmp = big_int_mul_karatsuba(result, base_copy);
            big_int *tmpmod = big_int_mod(tmp, mod);
            big_int_free(result);
            result = tmpmod;
            big_int_free(tmp);
        }
        tmp = big_int_mul_karatsuba(base_copy, base_copy);
        big_int *tmpmod = big_int_mod(tmp, mod);
        big_int_free(base_copy);
        big_int_free(tmp);
        base_copy = tmpmod;
        big_int_shift_right_once(exp_copy);
    }

    big_int_free(base_copy);
    big_int_free(exp_copy);

    return result;
}

big_int *big_int_lr_mod_pow(const char base, const big_int *exponent, const big_int *mod) {
    if (!base || !exponent || !mod) return nullptr;

    big_int *result = big_int_get(base - '0');
    if (!result) return nullptr;

    const size_t total_bits = exponent->length << 3;
    bool c = false;
    for (int i = total_bits - 1; i >= 0; --i) {
        if (c) {
            big_int_mul_void(&result, result);
            big_int *rm = big_int_mod(result, mod);
            if (!rm) {
                big_int_free(result);
                return nullptr;
            }
            big_int_free(result);
            result = rm;

            if ((exponent->number[i >> 3] >> (i & 7)) & 1) {
                big_int *tmp = big_int_get("0");
                for (char counter = 0; counter < base - 1 - '0'; counter++) {
                    big_int_add_void(&tmp, result);
                }
                big_int_add_void(&result, tmp);
                rm = big_int_mod(result, mod);
                if (!rm) {
                    big_int_free(result);
                    return nullptr;
                }
                big_int_free(result);
                result = rm;
            }
            continue;;
        }
        if ((exponent->number[i >> 3] >> (i & 7)) & 1) c = true;
    }

    return result;
}

big_int *big_int_lr_mod_pow(const big_int *base, const big_int *exponent, const big_int *mod) {
    if (!base || !exponent || !mod) return nullptr;

    big_int *result = big_int_mod(base, mod);
    if (!result) return nullptr;

    const size_t total_bits = exponent->length << 3;
    bool c = false;
    for (ll i = total_bits - 1; i >= 0; --i) {
        if (c) {
            big_int_mul_void(&result, result);
            big_int *rm = big_int_mod(result, mod);
            if (!rm) {
                big_int_free(result);
                return nullptr;
            }
            big_int_free(result);
            result = rm;
            if ((exponent->number[i >> 3] >> (i & 7)) & 1) {
                big_int_mul_void(result, base);
                rm = big_int_mod(result, mod);
                if (!rm) {
                    big_int_free(result);
                    return nullptr;
                }
                big_int_free(result);
                result = rm;
            }
            continue;
        }
        if ((exponent->number[i >> 3] >> (i & 7)) & 1) c = true;
    }
    return result;
}

#pragma endregion

#pragma region Comparisons

bool big_int_geq(const big_int *n1, const big_int *n2) {
    if (n1 == nullptr || n2 == nullptr) return false;

    if (n1->sign != n2->sign) return !n1->sign;

    if (n1->length != n2->length) return ((n1->length > n2->length) ^ n1->sign);

    for (int i = (n1->length - 1); i >= 0; i--) {
        if (n1->number[i] != n2->number[i]) return ((n1->number[i] > n2->number[i]) ^ n1->sign);
    }

    return true;
}

bool big_int_abs_geq(const big_int *n1, const big_int *n2) {
    if (!n1 || !n2) return false;

    if (n1->length != n2->length) return n1->length > n2->length;

    for (ll i = (n1->length - 1); i >= 0; i--) {
        if (n1->number[i] != n2->number[i]) return n1->number[i] > n2->number[i];
    }
    return true;
}

bool big_int_eq(const big_int *n1, const big_int *n2) {
    if (!n1 || !n2 || n1->sign != n2->sign || n1->length != n2->length) return false;

    for (size_t i = 0; i < n1->length; i++) {
        if (n1->number[i] != n2->number[i]) return false;
    }
    return true;
}

#pragma endregion

#pragma region Numeric Conversion

big_int *big_int_slice(const big_int *n, size_t start, size_t end) {
    if (!n || end < start) return big_int_get(0);

    if (end > n->length) end = n->length;

    big_int *result = (big_int *) malloc(sizeof(big_int));
    if (!result) return big_int_get(0);

    result->length = end - start;
    result->number = (unsigned char *) calloc(result->length, sizeof(unsigned char));
    if (!result->number) {
        free(result);
        return big_int_get(0);;
    }

    result->sign = n->sign;

    memcpy(result->number, n->number + start, result->length);
    big_int_remove_zeroes(result);
    return result;
}

#pragma endregion

#pragma region Void_Functions

void big_int_add_void(big_int *n1, const big_int *n2) {
    if (!n1 || !n2) return;

    big_int *temp = big_int_add(n1, n2);
    if (!temp) return;
    free(n1->number);
    *n1 = *temp;
    free(temp);
}

void big_int_add_void(big_int **n1, const big_int *n2) {
    if (!n1 || !n2) return;

    big_int *temp = big_int_add(*n1, n2);
    if (!temp) return;

    big_int_free(*n1);
    *n1 = temp;
}

void big_int_sub_void(big_int *n1, const big_int *n2) {
    if (!n1 || !n2) return;

    big_int *temp = big_int_sub(n1, n2);
    if (!temp) return;
    free(n1->number);
    *n1 = *temp;
    free(temp);
}

void big_int_sub_void(big_int **n1, const big_int *n2) {
    if (!n1 || !n2) return;

    big_int *temp = big_int_sub(*n1, n2);
    if (!temp) return;

    big_int_free(*n1);
    *n1 = temp;
}

void big_int_shift_right_void(big_int *num, ll n) {
    if (!num || n < 0) return;

    big_int *temp = big_int_shift_right(num, n);
    if (!temp) return;
    free(num->number);
    *num = *temp;
    free(temp);
}

void big_int_shift_right_void(big_int **num, ll n) {
    if (!num || n < 0) return;

    big_int *temp = big_int_shift_right(*num, n);
    if (!temp) return;

    big_int_free(*num);
    *num = temp;
}

void big_int_shift_left_void(big_int *num, ll n) {
    if (!num || n < 0) return;

    big_int *temp = big_int_shift_left(num, n);
    if (!temp) return;
    free(num->number);
    *num = *temp;
    free(temp);
}

void big_int_shift_left_void(big_int **num, ll n) {
    if (!num || n < 0) return;

    big_int *temp = big_int_shift_left(*num, n);
    if (!temp) return;

    big_int_free(*num);
    *num = temp;
}

void big_int_mul_void(big_int *n1, const big_int *n2) {
    if (!n1 || !n2) return;

    big_int *temp = big_int_mul_karatsuba(n1, n2);
    if (!temp) return;
    free(n1->number);
    *n1 = *temp;
    free(temp);
}

void big_int_mul_void(big_int **n1, const big_int *n2) {
    if (!n1 || !n2) return;

    big_int *temp = big_int_mul_karatsuba(*n1, n2);
    if (!temp) return;

    big_int_free(*n1);
    *n1 = temp;
}

void big_int_shift_left_once(big_int *n) {
    if (!n) return;

    int t = (n->number[n->length - 1]) & 128;

    if (t) {
        n->length++;
        n->number = (unsigned char *) realloc(n->number, n->length);
        if (!n->number) return;
        memset(n->number + n->length - 1, 0, 1);
    }

    for (ll i = n->length - 1; i >= 0; i--) {
        n->number[i] <<= 1;
        if (i) n->number[i] += (n->number[i - 1] & 128) != 0;
    }
}

void big_int_shift_right_once(big_int *n) {
    for (size_t i = 0; i < n->length; i++) {
        n->number[i] >>= 1;
        if (i != n->length - 1) n->number[i] |= (n->number[i + 1] & 1) << 7;
    }
    big_int_remove_zeroes(n);
}

#pragma endregion

#pragma region Big_Int_Generate_Functions

big_int *big_int_rnd(size_t bytes_num) {

    big_int *x = (big_int *) malloc(sizeof(big_int));
    if (!x) return nullptr;

    x->length = bytes_num;
    x->sign = false;

    x->number = (unsigned char *) malloc(x->length * sizeof(unsigned char));
    if (!x->number) {
        free(x);
        return nullptr;
    }

    for (size_t i = 0; i < x->length; i++) {
        x->number[i] = rand();
    }

    if (!(x->number[x->length - 1])) x->number[x->length - 1] = 1;
    big_int_remove_zeroes(x);
    return x;
}

big_int *generate_big_int_prime(size_t bytes_num) {

    big_int *res = big_int_rnd_odd(bytes_num);
    while (true) {
        if (miller_rabin_test_big_int(res, 10)) return res;
        big_int_add_void(res, two);
    }
}

big_int *generate_big_int_prime2(size_t bytes_num) {

    big_int *res = (big_int *) malloc(sizeof(big_int));
    res->sign = false;
    res->length = bytes_num;
    res->number = (unsigned char *) calloc(res->length, sizeof(unsigned char));
    res->number[0] = 1;
    res->number[res->length - 1] = rand() % 255 + 1;
    while (true) {
        if (miller_rabin_test_big_int(res, 10)) return res;
        big_int_add_void(res, two);
    }
}

big_int *generate_big_int_prime3(size_t bytes_num) {
    big_int *r = big_int_rnd_odd(bytes_num);

    while (!miller_rabin_test_big_int(r, 10)) {
        big_int_free(r);
        r = big_int_rnd_odd(bytes_num);
    }
    return r;
}

big_int *generate_big_int_prime4(size_t bytes_num) {
    big_int *r = big_int_rnd_odd(bytes_num);

    big_int *a;
    big_int *rem;
    while (true) {
//        cout << clock() / CLOCKS_PER_SEC << endl;
        bool flag = true;
        for (int i = 0; i < 100; i++) {
            a = big_int_get(prime[i]);
            rem = big_int_mod(r, a);
            if (big_int_eq(rem, zero)) {
                flag = false;
                big_int_free(2, a, rem);
                break;
            }
            big_int_free(2, a, rem);
        }
        if (flag && miller_rabin_test_big_int(r, 10)) {
            return r;
        }
        big_int_free(r);
        r = big_int_rnd(bytes_num);
    }
}

big_int *generate_big_int_prime5(size_t bytes_num) {
    big_int *r = big_int_rnd_odd(bytes_num);

    big_int *a;
    big_int *rem;
    while (true) {
        bool flag = true;
        for (int i = 0; i < 100; i++) {
            a = big_int_get(prime[i]);
            rem = big_int_mod(r, a);
            if (big_int_eq(rem, zero)) {
                flag = false;
                big_int_free(2, a, rem);
                break;
            }
            big_int_free(2, a, rem);
        }
        if (flag && miller_rabin_test_big_int(r, 10)) {
            return r;
        }
        big_int_add_void(r, two);
    }
}

big_int *big_int_generate_range(const big_int *n1, const big_int *n2) {

    big_int *difference = big_int_sub(n2, n1);
    size_t num_bytes = difference->length;

    big_int *rnd_num = nullptr;

    do {
        if (rnd_num != nullptr) big_int_free(rnd_num);
        rnd_num = big_int_rnd(num_bytes);
    } while (!(big_int_geq(rnd_num, n1) && !big_int_geq(rnd_num, n2)));

    big_int *result = big_int_add(n1, rnd_num);

    big_int_free(difference);
    big_int_free(rnd_num);

    return result;
}

big_int *big_int_rnd_odd(size_t bytes_num) {
    big_int *x = (big_int *) malloc(sizeof(big_int));
    if (!x) return nullptr;

    x->length = bytes_num;
    x->sign = false;

    x->number = (unsigned char *) malloc(x->length * sizeof(unsigned char));
    if (!x->number) {
        free(x);
        return nullptr;
    }
    for (size_t i = 0; i < x->length; i++) {
        x->number[i] = rand();
    }
    // Make sure the number is odd
    x->number[0] |= 1;
    return x;
}

void generate_and_save_primes(const size_t size, const size_t count) {

    std::string filename = "./primes/" + std::to_string(size) + ".txt";
    std::ofstream outfile(filename, std::ios::app);
    ll start = clock();
    for (size_t i = 0; i < count; i++) {
        ll st = clock();
        big_int *num = generate_big_int_prime5(size);
        ll en = clock();
        cout << i + 1 << " prime time: " << double(en - st) / CLOCKS_PER_SEC << endl;
        outfile << big_int_to_string(num) << std::endl;
        big_int_free(num);

    }
    outfile.close();
    ll end = clock();
    cout << "Total time: " << double(end - start) / CLOCKS_PER_SEC;
}

#pragma endregion

#pragma region RSA_Functions

bool miller_rabin_test_big_int(const big_int *num, size_t iterations) {
    if (!num) return false;

    if (big_int_eq(num, two)) return true;

    if (!(num->number[0] & 1)) return false;

    big_int *num_1 = big_int_sub(num, one);
    big_int *d = big_int_copy(num_1);

    size_t s = 0;
    while (!(d->number[0] & 1)) {
        big_int_shift_right_once(d);
        s++;
    }
    big_int *a = big_int_rnd(1 + rand() % (num->length));
    if (big_int_eq(a, zero)) {
        //if (a->length = 1 && a->number[0] == 0) {
        big_int_free(a);
        a = big_int_copy(one);
    }
    big_int *x;
    big_int *temp_x;
    for (size_t i = 0; i < iterations; big_int_add_void(a, one), i++) {
        if (big_int_geq(a, d) || (a->length == 1 && a->number == (unsigned char *) (1))) {
            big_int_free(a);
            a = big_int_copy(two);
        }
        x = big_int_lr_mod_pow(a, d, num);
        if (big_int_eq(x, one) || big_int_eq(x, num_1)) {
            big_int_free(x);
            continue;
        }
        size_t j;
        for (j = 0; j < s - 1; j++) {
            temp_x = big_int_lr_mod_pow(x, two, num);
            big_int_free(x);
            x = temp_x;
            //big_int_mul_void(&x, x);
            //temp_x = big_int_mod(x, num);
            //big_int_free(x);
            //x = temp_x;
            if (big_int_eq(x, one)) {
                big_int_free(4, num_1, d, x, a);
                return false;
            }
            if (big_int_eq(x, num_1)) break;
        }
        big_int_free(x);
        if (j == s - 1) {
            big_int_free(3, num_1, d, a);
            return false;
        }
    }
    big_int_free(3, num_1, d, a);
    return true;
}

big_int *big_int_euclid_extended(const big_int *m, const big_int *n, big_int **x, big_int **y) {
    big_int *a = big_int_copy(m);
    big_int *b = big_int_copy(n);

    big_int *x_next = big_int_get(0);
    big_int *y_next = big_int_get(1);

    *x = big_int_get(1);
    *y = big_int_get(0);

    while (!big_int_eq(b, zero)) {
        big_int *q = big_int_div_quotient(a, b);

        // (a, b) = (b, a % b)
        big_int *tmp = big_int_copy(b);
        big_int_free(b);
        b = big_int_mod(a, tmp);
        big_int_free(a);
        a = big_int_copy(tmp);
        big_int_free(tmp);

        // (x, x_next) = (x_next, x - q * x_next)
        tmp = big_int_copy(x_next);
        big_int *t = big_int_mul(q, x_next);
        big_int_free(x_next);
        x_next = big_int_sub(*x, t);
        big_int_free(*x);
        *x = big_int_copy(tmp);
        big_int_free(tmp);
        big_int_free(t);

        // (y, y_next) = (y_next, y - q * y_next)
        tmp = big_int_copy(y_next);
        t = big_int_mul(q, y_next);
        big_int_free(y_next);
        y_next = big_int_sub(*y, t);
        big_int_free(*y);
        *y = big_int_copy(tmp);
        big_int_free(tmp);
        big_int_free(t);

        big_int_free(q);
    }
    big_int_free(3, x_next, y_next, b);

    return a;
}

big_int *big_int_mod_inverse(const big_int *num, const big_int *modulus) {
    big_int *a = big_int_copy(num);
    big_int *b = big_int_copy(modulus);

    big_int *x_next = big_int_get(0);
    big_int *y_next = big_int_get(1);

    big_int *x = big_int_get(1);
    big_int *y = big_int_get(0);

    while (!big_int_eq(b, zero)) {
        big_int *q = big_int_div_quotient(a, b);

        // (a, b) = (b, a % b)
        big_int *tmp = big_int_copy(b);
        big_int_free(b);
        b = big_int_mod(a, tmp);
        big_int_free(a);
        a = big_int_copy(tmp);
        big_int_free(tmp);

        // (x, x_next) = (x_next, x - q * x_next)
        tmp = big_int_copy(x_next);
        big_int *t = big_int_mul(q, x_next);
        big_int_free(x_next);
        x_next = big_int_sub(x, t);
        big_int_free(x);
        x = big_int_copy(tmp);
        big_int_free(tmp);
        big_int_free(t);

        // (y, y_next) = (y_next, y - q * y_next)
        tmp = big_int_copy(y_next);
        t = big_int_mul(q, y_next);
        big_int_free(y_next);
        y_next = big_int_sub(y, t);
        big_int_free(y);
        y = big_int_copy(tmp);
        big_int_free(tmp);
        big_int_free(t);

        big_int_free(q);
    }
    if (big_int_geq(zero, x)) big_int_add_void(x, modulus);
    big_int_free(5, x_next, y_next, a, b, y);

    return x;
}

big_int *big_int_mod_inverse1(const big_int *num, const big_int *modulus) {
    big_int *x;
    big_int *y;
    big_int *gcd = big_int_euclid_extended(num, modulus, &x, &y);
    big_int_free(y);
    big_int_free(gcd);
    return x;
}

void generate_rsa_keys(const size_t bytes_num, rsa_key **public_key, rsa_key **private_key) {
    if (bytes_num < 2) return;

    big_int *p = generate_big_int_prime4(bytes_num / 2);
    big_int *q = generate_big_int_prime4(bytes_num / 2);

    big_int *m = big_int_mul_karatsuba(p, q);

    big_int *p_minus_one = big_int_sub(p, one);
    big_int *q_minus_one = big_int_sub(q, one);
    big_int *phi_m = big_int_mul_karatsuba(p_minus_one, q_minus_one);

    big_int *d = big_int_mod_inverse(e, phi_m);

    *public_key = (rsa_key *) malloc(sizeof(rsa_key));
    *private_key = (rsa_key *) malloc(sizeof(rsa_key));

    (*public_key)->mod = m;
    (*public_key)->exp = big_int_copy(e);
    (*private_key)->mod = big_int_copy(m);
    (*private_key)->exp = d;

    big_int_free(5, p, q, p_minus_one, q_minus_one, phi_m);
}

big_int *rsa_encrypt(const big_int *plaintext, const rsa_key *public_key) {
    return big_int_lr_mod_pow(plaintext, public_key->exp, public_key->mod);
}

big_int *rsa_decrypt(const big_int *ciphertext, const rsa_key *private_key) {
    return big_int_lr_mod_pow(ciphertext, private_key->exp, private_key->mod);
}

big_int* big_int_from_char(const char* text) {
    big_int* n = (big_int*)malloc(sizeof(big_int));
    n->length = strlen(text);
    n->sign = false;
    n->number = (unsigned char*)calloc(n->length, sizeof(unsigned char));

    for (size_t i = 0; i < n->length; i++) {
        n->number[i] = text[i];
    }
    big_int_remove_zeroes(n);
    return n;
}

big_int* get_public_key(char* name) {
    FILE *file = fopen("public_keys.txt", "r");

    char keyName[256];
    char keyVal[4096];

    while (fscanf(file, "%255s %4095s", keyName, keyVal) == 2) {
        if (strcmp(keyName, name) == 0) {
            big_int *publicKey = big_int_get(keyVal);
            fclose(file);
            return publicKey;
        }
    }

    fclose(file);
    return nullptr;
}

void save_encrypted_message(char* name, big_int* encrypted_msg) {
    const int max_filename_length = 256;
    char filename[max_filename_length];

    int bytes_written = snprintf(filename, sizeof(filename), "encrypted_msg/%s_encrypted", name);

    char unique_filename[max_filename_length];
    FILE *outfile = nullptr;
    int suffix = 0;
    do {
        if (outfile) {
            fclose(outfile);
        }

        snprintf(unique_filename, max_filename_length, "%s_%d.txt", filename, suffix++);
        outfile = fopen(unique_filename, "r");
        if (outfile != nullptr) {
            fclose(outfile);
        }
    } while (outfile != nullptr);

    outfile = fopen(unique_filename, "w");

    const char *encrypted_str = big_int_to_string(encrypted_msg);

    fprintf(outfile, "%s", encrypted_str);
    fclose(outfile);

    printf("Encrypted message saved to %s.\n", unique_filename);
}

void save_rsa_keys(const char *name, const rsa_key *public_key, const rsa_key *private_key) {
    FILE *public_f = fopen("public_keys.txt", "a");
    if (public_f != nullptr) {
        const char *modulus_str = big_int_to_string(public_key->mod);
        fprintf(public_f, "%s %s\n", name, modulus_str);
        fclose(public_f);
    }

    FILE *private_f = fopen("private_keys.txt", "a");
    if (private_f != nullptr) {
        const char *private_key_str = big_int_to_string(private_key->exp);
        const char *modulus_str = big_int_to_string(private_key->mod);
        fprintf(private_f, "%s %s\n", name, modulus_str);
        fclose(private_f);
    }
}

void RSA_key_free(rsa_key* key) {
    if (!key) return;

    if (key->mod) {
        big_int_free(key->mod);
    }
    if (key->exp) {
        big_int_free(key->exp);
    }
    free(key);
}

void handle_help_command() {
    std::cout << "RSA Console Application Help:\n"
              << "Commands:\n"
              << "  encode [name] [message] - Encrypt a message\n"
              << "  decode [flag] [message] - Decrypt a message. Flag is the value of 'file'/'console'\n"
              << "  generate [name] [key_length] - Generate a new set of RSA keys of [key_length] bytes and save with the identifier [name]"
              << "  showkey [name] - Show public key for [name]\n"
              << "  showallkeys - Show all available public keys\n"
              << "  addkey [name] [binary_key] - Add public key in binary form for user [name]\n"
              << "  help - Show this help message\n"
              << "  exit - Exit the application"
              << std::endl;
}

void handle_encode_command(char *cmd) {
    char *name = strtok(cmd + 7, " ");
    char *message = strtok(nullptr, "\n");

    if (name == nullptr || message == nullptr) {
        std::cout << "Error: Invalid arguments for encode command.\n";
        return;
    }

    rsa_key *public_key = (rsa_key*)malloc(sizeof(rsa_key));
    public_key->exp = big_int_copy(e);
    public_key->mod = get_public_key(name);

    if (public_key->mod == nullptr) {
        std::cout << "Error: Public key not found for " << name << ".\n";
        RSA_key_free(public_key);
        return;
    }

    big_int *msg_bi = big_int_from_char(message);
    big_int *encrypted_msg = rsa_encrypt(msg_bi, public_key);

    save_encrypted_message(name, encrypted_msg);

    big_int_free(msg_bi);
    big_int_free(encrypted_msg);
    RSA_key_free(public_key);
}

void handle_decode_command(char *cmd) {
    char *flag = strtok(cmd + 7, " ");
    char *message = strtok(nullptr, "\n");

    if (flag == nullptr || message == nullptr) {
        printf("Error: Invalid arguments for decode command.\n");
        return;
    }

    rsa_key *private_key = (rsa_key*)malloc(sizeof(rsa_key));
    private_key->mod = big_int_copy(mod);
    private_key->exp = big_int_copy(ex);


    if (strcmp(flag, "console") == 0) {
        big_int *encrypted_msg = big_int_get(message);
        big_int *decrypted_msg = rsa_decrypt(encrypted_msg, private_key);
        big_int_print_text(decrypted_msg);
        big_int_free(encrypted_msg);
        big_int_free(decrypted_msg);
    } else if (strcmp(flag, "file") == 0) {
        FILE *file = fopen(message, "r");
        if (file == nullptr) {
            printf("Error: Cannot open file %s.\n", message);
            return;
        }

        fseek(file, 0, SEEK_END);
        long filesize = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *encrypted_msg_str = (char*)malloc((filesize + 1) * sizeof(char));
        fread(encrypted_msg_str, 1, filesize, file);
        fclose(file);

        encrypted_msg_str[filesize] = '\0';

        big_int *encrypted_msg = big_int_get(encrypted_msg_str);
        free(encrypted_msg_str);

        big_int *decrypted_msg = rsa_decrypt(encrypted_msg, private_key);
        big_int_print_text(decrypted_msg);
        big_int_free(encrypted_msg);
        big_int_free(decrypted_msg);
    } else {
        printf("Error: Invalid flag for decode command. Use 'file' or 'console'.\n");
    }
    RSA_key_free(private_key);
}

void handle_generate_command(const char *name, size_t key_length){
    rsa_key *public_key, *private_key;

    generate_rsa_keys(key_length, &public_key, &private_key);

    save_rsa_keys(name, public_key, private_key);

    RSA_key_free(public_key);
    RSA_key_free(private_key);

    printf("Keys generated and saved for '%s'.\n", name);
}

void handle_add_key_command(const char *name, const char *binary_key){
     big_int *existing_key = get_public_key((char *)name);

    if(existing_key != nullptr) {
        char response[10];
        printf("A key for '%s' already exists. Do you want to replace it? (yes/no): ", name);
        scanf("%9s", response);

        if (strcmp(response, "yes") != 0) {
            big_int_free(existing_key);
            return;
        }
        big_int_free(existing_key);
    }

    big_int *public_key = big_int_get(binary_key);

    if(public_key == nullptr) {
        std::cout << "Error: Invalid public key format.\n";
        return;
    }

    const char *temp_filename = "public_keys.tmp";
    FILE *public_f = fopen("public_keys.txt", "r");
    FILE *tempFile = fopen(temp_filename, "w");
    char line[1024];
    int found = 0;

    if (public_f != nullptr && tempFile != nullptr) {
        // Скопировать содержимое, пропуская строку с существующим ключом
        while (fgets(line, sizeof(line), public_f)) {
            if (strstr(line, name) && !found) {
                found = 1;
                fprintf(tempFile, "%s %s\n", name, binary_key);
            } else {
                fputs(line, tempFile);
            }
        }

        if (!found) {
            fprintf(tempFile, "%s %s\n", name, binary_key);
        }

        fclose(public_f);
        fclose(tempFile);

        remove("public_keys.txt");
        rename(temp_filename, "public_keys.txt");
        printf("Public key for '%s' updated.\n", name);
    }

    if (public_f != nullptr) {
        fclose(public_f);
    }
    if (tempFile != nullptr) {
        fclose(tempFile);
    }

    big_int_free(public_key);
}

void handle_addkey_command(char *cmd) {
    char *name = strtok(cmd + 7, " ");
    char *binary_key = strtok(nullptr, "\n");

    if (name == nullptr || binary_key == nullptr) {
        std::cout << "Error: Invalid arguments for addkey command.\n";
        return;
    }

    handle_add_key_command(name, binary_key);
}

void handle_show_public_key_command(const char *name) {
    big_int *public_key = get_public_key((char *)name);
    if (public_key == nullptr) {
        printf("Public key not found for '%s'.\n", name);
        return;
    }
    const char *key_str = big_int_to_string(public_key);
    printf("Public key for '%s': %s\n", name, key_str);
    free((void*)key_str);
    big_int_free(public_key);
}

void handle_show_all_public_keys_command() {
    FILE *file = fopen("public_keys.txt", "r");

    char line[4096];
    printf("Available public keys:\n");
    while (fgets(line, sizeof(line), file)) {
        char *nl_ptr = strchr(line, '\n');
        if (nl_ptr) *nl_ptr = '\0';
        printf("%s\n", line);
    }

    fclose(file);
}

void console_app() {
    char cmd[4000];
    while (true) {
        cout << ">>>";
        std::cin.getline(cmd, 4000);

        if (strncmp(cmd, "encode", 6) == 0) {
            handle_encode_command(cmd);
        } else if (strncmp(cmd, "decode", 6) == 0) {
            handle_decode_command(cmd);
        } else if (strncmp(cmd, "help", 4) == 0) {
            handle_help_command();
        } else if (strncmp(cmd, "exit", 4) == 0) {
            break;
        } else if (strncmp(cmd, "addkey", 6) == 0){
            handle_addkey_command(cmd);
            fseek(stdin,0,SEEK_END);
        } else if (strncmp(cmd, "showkey", 7) == 0) {
            char *name = strtok(cmd + 8, "\n");
            if (name == nullptr) {
                printf("Error: Invalid arguments for showkey command.\n");
                continue;
            }
            handle_show_public_key_command(name);
        } else if (strncmp(cmd, "showallkeys", 11) == 0) {
            handle_show_all_public_keys_command();
        } else if (strncmp(cmd, "generate", 8) == 0) {
            char *name = strtok(cmd + 9, " ");
            char *key_length_str = strtok(nullptr, "\n");
            if (name == nullptr || key_length_str == nullptr) {
                std::cout << "Error: Invalid arguments for generate command.\n";
                continue;
            }
            size_t key_length = strtoul(key_length_str, nullptr, 10);
            handle_generate_command(name, key_length);
        }
        else {
            std::cout << "Invalid command. Type 'help' for assistance.\n";
        }
    }
}

#pragma endregion

#pragma region GLOBAL

const big_int *const one = big_int_get(1);
const big_int *const zero = big_int_get(0);
const big_int *const two = big_int_get(2);
const big_int *const e = big_int_get("10000000000000001"); // F_4 = 65537 = 2 ^ 16 + 1
const big_int *const mod = big_int_get("10010101101010100000111101111100111101100000001111011011101100100011111110111011101010001100001111001110110111000100111000110011010001111000101001011100000111100100000100010100101000000100101001111011111001010001011100011001001101011000000110000000000110001110000100010111010100001110000110101110001011100100010111110001100011000100001110101111110000110011000001110010111101011111111000000110011011101101100100111111100010011010010100101101100001001111010001010000110110111100110110101111100010011110100001000011110100101001011000001111101111100101011010000100010101011000000011110111011111100110100011100110010011111101110011110000010100110110011110000110000011101001101100001010010000110110011110011100000001011100001010000001111111010010010001010011011110110110000010001010110110010011000001010010110001011010010001000101000110001000111100110101001101100011110110111010000011110000111000011100000111010011100011111001111000011000001110000001011110110110011100100101111011100101000110100100111010100110001110000001111");
const big_int *const ex = big_int_get("100010011100000000011100110000010100011100101110000001011110011111101010000000010010111100110100000010100001000101111011011010000111011101010000010000011011100100010110111011011000010100011011000010010010110110110010001101001110000110101010011011111101110001110110010100001001111110000001000110000111000000111110101100000011010000001001100110001001110001101110110110110110000011001110100011011101110100111000110101111110001000111000110001011100110010111001100100100011001010111100111111011111010001010000011010101111111000100100101100110111000011011100001000111010000011110011110000110111101001000101000011011001001100011010000101011010111110000001011101000001111011100101110011010110010110001001110100100110000000100001111000100110001100011101011001010011011101110001101000110101100010011101101100110000011101101111010000011010011111011000010100100100001110101000110010000100001111100011010101010001111010011010011011001011001100000010100111000011101111110101100101011101100000001011110001100111101010011001110000001");


#pragma endregion

