//
// Coded by Zelendzu & Umato on 31.05.2024.
//


#include <random>
#include "big_integer.h"
#include "NFA_console_app.h"


int main() {
    std::random_device rd;
    srand(rd());

    NFA_console_app();
    system("pause");

    big_int_free(6, zero, one, two, e, mod, ex);
    return 0;
}





