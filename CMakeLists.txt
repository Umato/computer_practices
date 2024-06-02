cmake_minimum_required(VERSION 3.26)
project(computer_practices)

set(CMAKE_CXX_STANDARD 23)

add_executable(computer_practices main.cpp
        Algorithm.cpp
        Algorithm.h
        big_integer.cpp
        big_integer.h
        graph.cpp
        graph.h
        NFA.cpp
        NFA.h
        test.cpp
        test.h
        NFA_console_app.cpp
        NFA_console_app.h
        NFA_variables.cpp
        NFA_variables.h
        NFA_stack.cpp
        NFA_stack.h
        linear_term.cpp
        linear_term.h
)

