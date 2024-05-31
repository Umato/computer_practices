#include <random>
#include "big_integer.h"
#include "Algorithm.h"
#include "test.h"
#include "graph.h"
#include "NFA.h"



int main() {
    std::random_device rd;
    srand(rd());

    NFA_console_app();
    system("pause");

    big_int_free(6, zero, one, two, e, mod, ex);
    return 0;
}





