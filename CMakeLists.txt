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
)
