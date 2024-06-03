@echo off
g++ -o main main.cpp NFA.cpp NFA_console_app.cpp NFA_stack.cpp NFA_variables.cpp big_integer.cpp graph.cpp linear_term.cpp Algorithm.cpp
mkdir build
move main.exe build
cd build
main