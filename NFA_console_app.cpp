//
// Coded by Zelendzu & Umato on 31.05.2024.
//


#include "NFA_console_app.h"

void NFA_console_app() {
    char input[256];
    cout << "Enter command (type 'exit' to quit):\n";
    while (true) {
        fflush(stdout);
        cout << ">";
        if (fgets(input, sizeof(input), stdin) == nullptr) {
            break;
        }

        to_lower_case(input);
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "exit") == 0) {
            cout << "Exiting NFA Console Application.\n";
            break;
        } else if (strcmp(input, "help") == 0) {
            print_help_command();
        } else if (strcmp(input, "hElp") == 0) {
            print_hElp_command();
        } else if (strcmp(input, "nfa_list") == 0) {
            nfa_list_command();
        } else if (strncmp(input, "def ", 4) == 0) {
            def_command(input);
        } else if (strncmp(input, "eval ", 5) == 0) {
            eval_command(input);
        } else if (strncmp(input, "for  ", 4) == 0) {
            for_command(input);
        } else if (strncmp(input, "eval2 ", 6) == 0) {
            eval2_command(input);
        } else if (strncmp(input, "visualize ", 10) == 0) {
            visualize_command(input);
        } else if (strncmp(input, "minimize $", 10) == 0) {
            minimization_command(input + 10);
        } else if (strncmp(input, "to_dfa $", 8) == 0) {
            convert_to_dfa_command(input + 8);
        } else if (strncmp(input, "remove_eps $", 12) == 0) {
            remove_epsilon_command(input + 12);
        } else if (strncmp(input, "cls", 3) == 0) {
            cls_command();
        } else if (strncmp(input, "regex ", 6) == 0) {
            regex_command(input + 6);
        } else {
                printf("Unknown command: %s\n", input);
        }
    }

}

#pragma region Commands

void print_help_command()
{
    int commandWidth = 55;

    std::cout << std::left;

    cout << "Available commands:\n";
    cout << std::setw(commandWidth) << "\texit" << " - Exit the NFA Console Application.\n";
    cout << std::setw(commandWidth) << "\thelp" << " - Display this help message.\n";
    cout << std::setw(commandWidth) << "\tcls" << " - Clear console\n";
    cout << std::setw(commandWidth) << "\tnfa_list" << " - List available automata.\n";
    cout << std::setw(commandWidth) << "\tdef <name> \"<predicate>\" [-m] [-v] [-re]" << " - Define a new NFA from a logical predicate and save it. Supports union(|), intersection(&), complement(~), right quotient(/), and left quotient(\\).\n";
    cout << std::setw(commandWidth) << "\tregex <name> \"<regular expression>\" [-m] [-v] [-re]" << " - Define a new NFA from a regular expression and save it. Supports 0, 1, or (|), concatenation (&), any symbol or empty (?), any symbol (.), any symbols (+), any symbols or empty (*).\n";
    cout << std::setw(commandWidth) << "\teval $automaton_name(num1, num2, ..., numN)" << " - Evaluate an automaton with a given numbers.\n";
    cout << std::setw(commandWidth) << "\teval \"<predicate>\" [-m] [-v] [-re]" << " - Evaluate an automaton, defined from a logical predicate, and returns True/False. Predicate must have no unquantified variables.\n";
    cout << std::setw(commandWidth) << "\teval2 $automaton_name(binary_num1, ..., binary_numN)" << " - Evaluate an automaton with a binary numbers.\n";
    cout << std::setw(commandWidth) << "\tfor $automaton_name(start, end, [step])" << " - Test an automaton with a range of decimal numbers.\n";
    cout << std::setw(commandWidth) << "\tvisualize $automaton_name" << " - Visualize an NFA as a graphical representation.\n";
    cout << std::setw(commandWidth) << "\tminimize $automaton_name" << " - Minimize.\n";
    cout << std::setw(commandWidth) << "\tto_dfa $automaton_name" << " - Convert an NFA to DFA.\n";
    cout << std::setw(commandWidth) << "\tremove_eps $automaton_name" << " - Remove epsilon transitions from an NFA.\n";
    cout << std::setw(commandWidth) << "\n\tPredicate example: \"Ax,y: Et: $sum(2x+1,2y,t) & ~$div2(t)\"\n";
}

void print_hElp_command() {
    int commandWidth = 55;
    int descWidth = 100;

    std::cout << std::left;

    cout << "Available commands:\n";
    cout << std::setw(commandWidth) << " exit" << " - " << std::setw(descWidth) << "Exit the NFA Console Application.\n";
    cout << std::setw(commandWidth) << " help" << " - " << std::setw(descWidth) << "Display this help message.\n";
    cout << std::setw(commandWidth) << " cls" << " - " << std::setw(descWidth) << "Clear console\n";
    cout << std::setw(commandWidth) << " nfa_list" << " - " << std::setw(descWidth) << "List available automata.\n";
    cout << std::setw(commandWidth) << " def <name> \"<predicate>\" [-m] [-v] [-re]" << " - " << std::setw(descWidth) << "Define a new NFA from a logical predicate and save it. Supports union(|), intersection(&), complement(~), right quotient(/), left quotient(\\), exist(E), forall(A).\n";
    cout << std::setw(commandWidth) << " regex <name> \"<regular expression>\" [-m] [-v] [-re]" << " - " << std::setw(descWidth) << "Define a new NFA from a regular expression and save it. Supports 0, 1, or (|), concatenation (&), any symbol or empty (?), any symbol (.), any symbols (+), any symbols or empty (*).\n";
    cout << std::setw(commandWidth) << " eval $automaton_name(num1, num2, ..., numN)" << " - " << std::setw(descWidth) << "Evaluate an automaton with a given numbers.\n";
    cout << std::setw(commandWidth) << " eval2 $automaton_name(binary_num1, ..., binary_numN)" << " - " << std::setw(descWidth) << "Evaluate an automaton with a binary numbers.\n";
    cout << std::setw(commandWidth) << " for $automaton_name(start, end, [step])" << " - " << std::setw(descWidth) << "Test an automaton with a range of decimal numbers.\n";
    cout << std::setw(commandWidth) << " visualize $automaton_name" << " - " << std::setw(descWidth) << "Visualize an NFA as a graphical representation.\n";
    cout << std::setw(commandWidth) << " minimize $automaton_name" << " - " << std::setw(descWidth) << "Minimize.\n";
    cout << std::setw(commandWidth) << " to_dfa $automaton_name" << " - " << std::setw(descWidth) << "Convert an NFA to DFA.\n";
    cout << std::setw(commandWidth) << " remove_eps $automaton_name" << " - " << std::setw(descWidth) << "Remove epsilon transitions from an NFA.\n";
}

void cls_command()
{
    system("cls");
    cout << "Enter command (type 'exit' to quit):\n";
}

void nfa_list_command() {
    const char* directory = "../NFA/NFAs/*"; // Путь к директории с маской для поиска всех файлов
    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(directory, &ffd);
    int count = 0;

    if (hFind == INVALID_HANDLE_VALUE) {
        printf("Failed to open directory\n");
        return;
    }

    do {
        if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0) {
            char* dot = strrchr(ffd.cFileName, '.');
            if (dot != nullptr) {
                *dot = '\0';
            }

            printf("%d) %s\n", ++count, ffd.cFileName);
        }
    } while (FindNextFile(hFind, &ffd) != 0);

    FindClose(hFind);
}

void def_command(const char* command) {
    char name[256], predicate[1024];
    bool minimize = false, visualize = false, remove_eps = false;

    if (sscanf(command, "def %s \"%[^\"]\"", name, predicate) == 2) {

        if (strstr(command, "-m") != nullptr){
            minimize = true;
        }
        if (strstr(command, "-v") != nullptr){
            visualize = true;
        }
        if (strstr(command, "-re") != nullptr){
            remove_eps = true;
        }

        NFA *nfa = NFA_from_predicate(predicate);
        if (nfa != nullptr) {

            if (remove_eps) {
                NFA_remove_epsilon_transitions(nfa);
            }

            if (minimize) {
                DFA_minimize_rec(&nfa);
            }
            char filename[300];
            sprintf(filename, "../NFA/NFAs/%s.txt", name);
            NFA_to_file(nfa, filename);

            if (visualize) {
                NFA_to_DOT(nfa);
            }
            NFA_free(nfa);
        } else {
            cout << "Failed to create NFA from predicate.\n";
        }
    } else {
        cout << "Invalid command format. Use def <name> \"<predicate>\" [-m] [-v] [-re]\n";
    }
}

void regex_command(const char* input)
{
    char name[256], regex[256];
    bool visualize = false, minimize = false;

    if (sscanf(input, "%s \"%[^\"]\"", name, regex) == 2) {

        if (strstr(input, "-m") != nullptr) minimize = true;
        if (strstr(input, "-v") != nullptr) visualize = true;

        NFA* nfa = NFA_from_regex(regex);
        if (nfa != nullptr) {

            if (minimize) {
                DFA_minimize_rec(&nfa);
            }

            char filename[300];
            sprintf(filename, "../NFA/NFAs/%s.txt", name);
            NFA_to_file(nfa, filename);

            if (visualize) {
                NFA_to_DOT(nfa);
            }
            NFA_free(nfa);
        }
        else {
            cout << "Failed to create NFA from predicate.\n";
        }
    }
    else {
        cout << "Invalid command format. Use regex <name> \"<regular expression>\" [-m] [-v]\n";
    }
}

void eval_command(const char* command)
{
    char automaton_name[256], number_string[1024], predicate[1024];
    int nums[100]; // max = 100
    int num_count = 0;

    if (sscanf(command, "eval $%[^(](%[^)]", automaton_name, number_string) == 2) {
        NFA* nfa = load_NFA_from_file(automaton_name);
        if (!nfa) {
            cout << "Failed to load NFA: " << automaton_name << endl;
            return;
        }

        char* token = strtok(number_string, ", ");
        while (token != nullptr && num_count < 100) {
            nums[num_count++] = atoi(token);
            token = strtok(nullptr, ", ");
        }

        if (num_count != nfa->alphabet_dim) {
            cout << "Error: Number of binary inputs does not match the automaton's dimension." << endl;
            NFA_free(nfa);
            return;
        }

        // Create a big_int_list with the parsed numbers
        big_int_list* bigint_list = big_int_list_init(num_count, nums);

        // Use the list to evaluate the NFA
        bool result = NFA_accept(nfa, bigint_list);

        cout << (result ? "True" : "False") << endl;

        NFA_free(nfa);
        big_int_list_free(bigint_list);
    }
    else if (sscanf(command, "eval \"%[^\"]\"", predicate) == 1) {

        NFA* nfa = NFA_from_predicate(predicate);
        if (!nfa) {
            cout << "Failed to create NFA from predicate.\n";
        }
        else if (nfa->alphabet_dim != 0) {
            NFA_free(nfa);
            cout << "There are undefined variables in given predicate.\n";
        }
        else {
            bool result = NFA_accept(nfa, (char*)nullptr);
            cout << (result ? "True" : "False") << endl;
            NFA_free(nfa);
        }
    }
    else  {
        cout << "Invalid command format. Use eval \"$<automaton_name>(num1, ... numN)\" or eval \"<predicate>\"." << endl;
    }
}

void eval2_command(const char* command)
{
    char automaton_name[256];
    char binary_numbers[1024];
    char* binary_strings[100];  // Assuming a max of 100 binary strings for simplicity
    int num_count = 0;

    if (sscanf(command, "eval2 $%[^(](%[^)]", automaton_name, binary_numbers) == 2) {
        NFA* nfa = load_NFA_from_file(automaton_name);
        if (!nfa) {
            cout << "Failed to load NFA: " << automaton_name << endl;
            return;
        }

        char* token = strtok(binary_numbers, ", ");
        while (token != nullptr && num_count < 100) {
            binary_strings[num_count++] = strdup(token); // Duplicate the string to store it
            token = strtok(nullptr, ", ");
        }

        // Validate the number of dimensions
        if (num_count != nfa->alphabet_dim) {
            cout << "Error: Number of binary inputs does not match the automaton's dimension." << endl;
            for (int i = 0; i < num_count; i++) {
                free(binary_strings[i]);
            }
            NFA_free(nfa);
            return;
        }

        bool result = NFA_accept(nfa, binary_strings, num_count);

        cout << (result ? "True" : "False") << endl;
        for (int i = 0; i < num_count; i++) {
            free(binary_strings[i]);
        }
        NFA_free(nfa);
    } else {
        cout << "Invalid command format. Use eval2 \"$automaton_name(binary_number1, binary_number2, ...)\"" << endl;
    }
}

void for_command(const char* command) {
    char automaton_name[256];
    int start, end;
    int step = 1;  // Default

    if (sscanf(command, "for $%[^(](%d, %d, %d)", automaton_name, &start, &end, &step) >= 2) {
        NFA* nfa = load_NFA_from_file(automaton_name);
        if (!nfa) {
            cout << "Failed to load NFA: " << automaton_name << endl;
            return;
        }

        for (int num = start; num <= end; num += step) {
            big_int* bg = big_int_get(num);
            bool result = NFA_accept(nfa, bg);
            big_int_free(bg);
            cout << num << " - " << (result ? "True" : "False") << endl;
        }

        NFA_free(nfa);
    } else {
        cout << "Invalid command format. Use for \"$automaton_name(start, end, [step])\"" << endl;
    }
}

void visualize_command(const char* command)
{
    char automaton_name[256];

    if (sscanf(command, "visualize $%s", automaton_name) == 1) {
        char filename[300];
        sprintf(filename, "../NFA/NFAs/%s.txt", automaton_name);

        NFA* nfa = NFA_from_file(filename);
        if (nfa == nullptr) {
            printf("Failed to load NFA: %s\n", automaton_name);
            return;
        }

        NFA_to_DOT(nfa);
        NFA_free(nfa);
        printf("Visualization for %s has been generated.\n", automaton_name);
    } else {
        printf("Invalid command format. Use visualize $automaton_name\n");
    }

}

void minimization_command(const char* automaton_name) {
    char filename[300];
    sprintf(filename, "../NFA/NFAs/%s.txt", automaton_name);
    NFA* nfa = NFA_from_file(filename);
    if (nfa) {
        NFA* minimized = DFA_minimize(nfa);
        if (minimized) {
            NFA_to_file(minimized, filename);
            NFA_free(minimized);
            cout << "NFA/DFA has been minimized and saved to " << filename << ".\n";
        } else {
            cout << "Error during minimization.\n";
        }
        NFA_free(nfa);
    } else {
        cout << "Failed to load automaton from file: " << filename << "\n";
    }
}

void convert_to_dfa_command(const char* automaton_name) {
    char filename[300];
    sprintf(filename, "../NFA/NFAs/%s.txt", automaton_name);
    NFA* nfa = NFA_from_file(filename);
    if (nfa) {
        NFA* dfa = NFA_to_DFA(nfa);
        if (dfa) {
            NFA_to_file(dfa, filename);
            NFA_free(dfa);
            cout << "NFA has been converted to DFA and saved to " << filename << ".\n";
        } else {
            cout << "Error during DFA conversion.\n";
        }
        NFA_free(nfa);
    } else {
        cout << "Failed to load automaton from file: " << filename << "\n";
    }
}

void remove_epsilon_command(const char* automaton_name) {
    char filename[300];
    sprintf(filename, "../NFA/NFAs/%s.txt", automaton_name);
    NFA* nfa = NFA_from_file(filename);
    if (nfa) {
        NFA_remove_epsilon_transitions(nfa);
        if (nfa) {        NFA_to_file(nfa, filename);
        NFA_free(nfa);
        cout << "Epsilon transitions removed and NFA saved to " << filename << ".\n";
        } else {
            cout << "Failed to remove epsilon transitions\n";
        }

    } else {
        cout << "Failed to load automaton from file: " << filename << "\n";
    }
}

#pragma endregion


#pragma region Support Functions

void remove_spaces(char* str) {
    char* dst = str;
    bool within_parentheses = false;
    bool within_exist_forall = false;

    while (*str != '\0') {
        if (*str == '(') {
            within_parentheses = true;
            *dst++ = *str;
        } else if (*str == ')') {
            within_parentheses = false;
            *dst++ = *str;
        } else if ((!within_parentheses && !within_exist_forall) || (*str != ' ' && *str != '\t')) {
            *dst++ = *str;
        }
        str++;
    }
    *dst = '\0';
}

int nfa_get_priority(char op) {
    switch (op) {
    case '(': return -1;
    case '~': return 5;
    case 'E':
    case 'A': return 4;
    case '&': return 3;
    case '|':
    case '/':
    case '\\': return 2;
    case '$': return 1;
    default:
        return -1;
    }
}

bool handle_operation(NFA_stack* stack, char* operation, NFA_variables* global_structure) {
    char op = operation[0];
    bool error_occured = false;
    switch (op)
    {
    case '&': {
        NFA* nfa2 = pop(stack);
        NFA* nfa1 = pop(stack);
        if (!nfa1 || !nfa2)
        {
            cout << "Amount of operations doesn't match number of given automatons.\n";
            error_occured = true;
        }
        else if (nfa1->alphabet_dim > nfa2->alphabet_dim)
        {
            cout << "Something went wrong (incorrect alphabet dimension).\n";
            error_occured = true;
        }
        if (error_occured)
        {
            NFA_free(nfa1);
            NFA_free(nfa2);
            break;
        }

        while (nfa1->alphabet_dim != nfa2->alphabet_dim)
        {
            NFA_extend_rec(&nfa1, nfa1->alphabet_dim);
        }
        NFA_intersect_rec(&nfa1, nfa2);
        push(stack, nfa1);
        NFA_free(nfa2);
        break;
    }
    case '|': {
        NFA *nfa2 = pop(stack);
        NFA *nfa1 = pop(stack);
        if (!nfa1 || !nfa2)
        {
            cout << "Amount of operations doesn't match number of given automatons.\n";
            error_occured = true;
        }
        else if (nfa1->alphabet_dim > nfa2->alphabet_dim)
        {
            cout << "Something went wrong (incorrect alphabet dimension).\n";
            error_occured = true;
        }
        if (error_occured)
        {
            NFA_free(nfa1);
            NFA_free(nfa2);
            break;
        }

        while (nfa1->alphabet_dim != nfa2->alphabet_dim)
        {
            NFA_extend_rec(&nfa1, nfa1->alphabet_dim);
        }
        NFA_union_rec(&nfa1, nfa2);
        push(stack, nfa1);
        NFA_free(nfa2);
        break;
    }
    case '~': {
        NFA *nfa = pop(stack);
        if (!nfa)
        {
            cout << "Amount of operations doesn't match number of given automatons.\n";
            error_occured = true;
        }
        if (error_occured)
        {
            NFA_free(nfa);
            break;
        }

        DFA_complement_rec(&nfa);
        push(stack, nfa);
        break;
    }
    case '/': {
        // Right quotient
        NFA *nfa2 = pop(stack);
        NFA *nfa1 = pop(stack);
        if (!nfa1 || !nfa2)
        {
            cout << "Amount of operations doesn't match number of given automatons.\n";
            error_occured = true;
        }
        else if (nfa1->alphabet_dim > nfa2->alphabet_dim)
        {
            cout << "Something went wrong (incorrect alphabet dimension).\n";
            error_occured = true;
        }
        if (error_occured)
        {
            NFA_free(nfa1);
            NFA_free(nfa2);
            break;
        }

        while (nfa1->alphabet_dim != nfa2->alphabet_dim)
        {
            NFA_extend_rec(&nfa1, nfa1->alphabet_dim);
        }
        NFA *result = NFA_rightquo(nfa1, nfa2);
        push(stack, result);
        NFA_free(nfa1);
        NFA_free(nfa2);
        break;
    }
    case '\\': {
        // Left quotient
        NFA *nfa2 = pop(stack);
        NFA *nfa1 = pop(stack);
        if (!nfa1 || !nfa2)
        {
            cout << "Amount of operations doesn't match number of given automatons.\n";
            error_occured = true;
        }
        else if (nfa1->alphabet_dim > nfa2->alphabet_dim)
        {
            cout << "Something went wrong (incorrect alphabet dimension).\n";
            error_occured = true;
        }
        if (error_occured)
        {
            NFA_free(nfa1);
            NFA_free(nfa2);
            break;
        }

        while (nfa1->alphabet_dim != nfa2->alphabet_dim)
        {
            NFA_extend_rec(&nfa1, nfa1->alphabet_dim);
        }
        NFA *result = NFA_leftquo(nfa1, nfa2);
        push(stack, result);
        NFA_free(nfa1);
        NFA_free(nfa2);
        break;
    }
    case 'E': {
        NFA* nfa = pop(stack);
        if (!nfa)
        {
            cout << "Amount of operations doesn't match number of given automatons.\n";
            error_occured = true;
        }
        else
        {
            char* op_end = strchr(operation, ':');
            if (op_end)
            {
                *op_end = '\0';
                char* context_E;
                char* var = strtok_s(operation + 1, ",", &context_E);
                while (var)
                {
                    if (NFA_variables_in(global_structure, var))
                    {
                        int var_index = NFA_variables_index(global_structure, var);
                        NFA_project_rec(&nfa, var_index, true);
                        NFA_variables_delete(global_structure, var_index);
                    }
                    else
                    {
                        cout << "Incorrect format of \"E\" quantifier. Variable \"" << var << "\" doesn\'t exist.\n";
                        error_occured = true;
                        break;
                    }

                    var = strtok_s(nullptr, ",", &context_E);
                }

                DFA_minimize_rec(&nfa);
                push(stack, nfa);
            }
            else
            {
                cout << "Incorrect format of \"E\" quantifier. Symbol \":\" was missed.\n";
                error_occured = true;
            }
        }

        if (error_occured)
        {
            NFA_free(nfa);
        }
        break;
    }
    case 'A': {
        NFA* nfa = pop(stack);
        if (!nfa)
        {
            cout << "Amount of operations doesn't match number of given automatons.\n";
            error_occured = true;
        }
        else
        {
            char* op_end = strchr(operation, ':');
            if (op_end)
            {
                *op_end = '\0';
                char* context_E;
                char* var = strtok_s(operation + 1, ",", &context_E);
                NFA* only_zeros = NFA_get_only_zeroes(nfa->alphabet_dim);
                while (var)
                {
                    if (NFA_variables_in(global_structure, var))
                    {
                        int var_index = NFA_variables_index(global_structure, var);
                        DFA_complement_rec(&nfa);
                        // может быть нужен right quotient?
                        NFA_project_rec(&nfa, var_index, true);
                        DFA_complement_rec(&nfa);
                        NFA_variables_delete(global_structure, var_index);
                    }
                    else
                    {
                        cout << "Incorrect format of \"A\" quantifier. Variable \"" << var << "\" doesn\'t exist.\n";
                        error_occured = true;
                        break;
                    }

                    var = strtok_s(nullptr, ",", &context_E);
                }

                NFA_free(only_zeros);
                DFA_minimize_rec(&nfa);
                push(stack, nfa);
            }
            else
            {
                cout << "Incorrect format of \"A\" quantifier. Symbol \":\" was missed.\n";
                error_occured = true;
            }
        }

        if (error_occured)
        {
            NFA_free(nfa);
        }
        break;
    }}

    return !error_occured;
}

char* NFA_RPN(const char* formula) {
    stack* operators = create_stack();
    int length = strlen(formula);
    char* rpn = (char*)malloc(length * 2);
    char* exist_forall = (char*)malloc(length + 1);
    int j = 0; //rpn[j]
    int k = 0; //exist_forall[k]

    for (const char *c = formula; *c; c++) {
        switch (*c) {
        case '(' :
            push(operators, *c);
            break;
        case 'E' :
        case 'A' :
            while (*c != ':' && *c != '\0'){
                if (*c == ' ') {
                    c++;
                    continue;
                }
                exist_forall[k++] = *c;
                c++;
            }
            if (*c == '\0') {
                cout << "Error: colon not found\n";
                free(rpn);
                free(exist_forall);
                free_stack(operators);
                return nullptr;
            }

            exist_forall[k++] = ':';
            exist_forall[k++] = ' ';
            break;
        case '&' :
        case '|' :
        case '~' :
        case '/' :
        case '\\':
            while (!is_stack_empty(operators) && nfa_get_priority(*c) <= nfa_get_priority(stack_top(operators))) {
                rpn[j++] = pop(operators);
                rpn[j++] = ' ';
            }
            push(operators, *c);
            break;
        case ')' :
            while (!is_stack_empty(operators) && stack_top(operators) != '(') {
                rpn[j++] = ' ';
                rpn[j++] = (char)pop(operators);
            }
            pop(operators); // Remove '(' from stack
            break;
        case '$' :
            rpn[j++] = *(c++);
            while (*c && *c != ')'){
                if (strchr("&|~/\\$", *c)){
                    fprintf(stderr, "Error: Invalid character '%c' in the expression\n", *c);
                    free(rpn);
                    free(exist_forall);
                    free_stack(operators);
                    return nullptr;
                }
                rpn[j++] = *(c++);
            }
            if (*c == '\0') {
                cout << "Error: closing parenthesis not found\n";
                free(rpn);
                free(exist_forall);
                free_stack(operators);
                return nullptr;
            } else if (*c == ')') {
                rpn[j++] = *c;
                //c--;
            }
            break;
        default:
            if (*c != ' ' || (j != 0 && rpn[j - 1] != ' '))
                rpn[j++] = *c;
            break;
        }
    }

    while (!is_stack_empty(operators)) {
        rpn[j++] = ' ';
        rpn[j++] = (char)pop(operators);
    }

    if (j > 0 && rpn[j - 1] == ' ') {
        j--; // Remove trailing space if it exists
    }

    rpn[j] = '\0';
    exist_forall[k] = '\0';
    char* result = (char*)malloc(strlen(rpn) + strlen(exist_forall) + 2);
    strcpy(result, rpn);
    strcat(result, " ");

    // exist_forall reversing
    char* tokens[10];
    int tokens_count = 0;
    char* context;
    char* token = strtok_s(exist_forall, " ", &context);
    while (token != nullptr) {
        tokens[tokens_count++] = strdup(token);
        token = strtok_s(nullptr, " ", &context);
    }
    for (int i = tokens_count - 1; i >= 0; i--) {
        strcat(result, tokens[i]);
        strcat(result, " ");
        free(tokens[i]);
    }

    free(rpn);
    free(exist_forall);
    free_stack(operators);
    return result;
}

NFA* NFA_from_predicate(const char* predicate) {
    char* rpn = NFA_RPN(predicate);
    if (!rpn) return nullptr;

    remove_spaces(rpn);

    NFA_stack* nfa_stack = create_nfa_stack();
    NFA_variables* global_vars = NFA_variables_init();

    char* context_rpn;
    char* token = strtok_s(rpn, " ", &context_rpn);
    while (token) {
        if (token[0] == '$') {
            char* name = extract_name(token);
            NFA* nfa = load_NFA_from_file(name);
            if (*(token + strlen(name) + 1) == '(') {
                char* term_start = token + strlen(name) + 2; // Move pointer past the character '('
                char* term_end = strchr(term_start, ')');
                if (term_end) {
                    *term_end = '\0'; // Replace ')' with '\0'

                    // 1. Create list of  linear expressions
                    int terms_count = nfa->alphabet_dim;
                    int terms_counter = 0;
                    linear_expression** terms = (linear_expression**)malloc(sizeof(linear_expression*) * terms_count);

                    char* context_terms;
                    char* terms_token = strtok_s(term_start, ",", &context_terms);
                    while (terms_token)
                    {
                        if (terms_counter < terms_count)
                        {
                            terms[terms_counter] = (linear_expression*)malloc(sizeof(linear_expression));
                            terms[terms_counter] = parse_linear_expression(terms_token);
                        }
                        terms_counter++;
                        terms_token = strtok_s(nullptr, ",", &context_terms);
                    }
                    if (terms_counter != terms_count)
                    {
                        cout << "Error: incorrect input - " << name << "(" << term_start
                            << ") needs " << terms_count <<" arguments, but gets " << terms_counter << ".\n";
                        free(rpn);
                        free_stack(nfa_stack);
                        NFA_variables_free(global_vars);
                        free(name);
                        NFA_free(nfa);
                        for (int k = 0; k < terms_count; k++)
                        {
                            free_linear_expression(terms[k]);
                        }

                        return nullptr;
                    }

                    // 2. Create mega term using the list
                    NFA_variables* unioned_term_vars = nullptr;
                    NFA* unioned_term = union_terms(terms_count, terms, &unioned_term_vars);

                    // 3. Intersect mega term with nfa, update global structure
                    NFA* new_nfa = NFA_with_term(nfa, unioned_term);
                    for (int k = 0; k < nfa->alphabet_dim; k++)
                    {
                        NFA_variables_delete(unioned_term_vars, 0);
                    }

                    merge_nfa_and_structure(&new_nfa, global_vars, unioned_term_vars);

                    // Free
                    for (int k = 0; k < terms_count; k++)
                    {
                        free_linear_expression(terms[k]);
                    }
                    free(terms);
                    NFA_free(unioned_term);
                    NFA_variables_free(unioned_term_vars);
                    NFA_free(nfa);
                    nfa = new_nfa;
                }
            }

            push(nfa_stack, nfa);
            free(name);
        } else {
            if (!handle_operation(nfa_stack, token, global_vars))
            {
                free(rpn);
                free_stack(nfa_stack);
                NFA_variables_free(global_vars);
                return nullptr;
            }
        }
        token = strtok_s(nullptr, " ", &context_rpn);
    }

    NFA* final_nfa = pop(nfa_stack);
    NFA_variables_free(global_vars);
    free(rpn);
    free_stack(nfa_stack);
    return final_nfa;
}

void sync_nfa_structure(NFA** main_nfa, NFA** sub_nfa, NFA_variables* main_vars, NFA_variables* sub_vars)
{
    if (!(*main_nfa) || !(*sub_nfa) || !main_vars || !sub_vars) return;

    int common_vars_count = 0;
    // swap common variables
    for (int i = 0; i < main_vars->count; i++)
    {
        char* var = main_vars->variables[i];
        if (NFA_variables_in(sub_vars, var))
        {
            if (var[0] != '*')
            {
                int index1 = i;
                if (common_vars_count != index1)
                {
                    NFA_swap_rec(main_nfa, common_vars_count, index1);
                    NFA_variables_swap(main_vars, common_vars_count, index1);
                }

                if (sub_vars->variables[common_vars_count] != var)
                {
                    int index2 = NFA_variables_index(sub_vars, var);
                    NFA_swap_rec(sub_nfa, common_vars_count, index2);
                    NFA_variables_swap(sub_vars, common_vars_count, index2);
                }
            }
            common_vars_count++;
        }
    }

    int main_vars_count = main_vars->count;
    int unique_sub_vars = sub_vars->count - common_vars_count;

    int sub_vars_count = sub_vars->count;
    int unique_main_vars = main_vars->count - common_vars_count;

    for (int i = 0; i < unique_main_vars; i++)
    {
        NFA_extend_rec(sub_nfa, common_vars_count);
        NFA_variables_insert(sub_vars, common_vars_count, main_vars->variables[main_vars_count - 1 - i]);
    }

    for (int i = 0; i < unique_sub_vars; i++)
    {
        NFA_extend_rec(main_nfa, (*main_nfa)->alphabet_dim);
        NFA_variables_add(main_vars, sub_vars->variables[common_vars_count + unique_main_vars + i]);
    }

}

NFA* union_terms(int terms_count, linear_expression** terms, NFA_variables** unioned_vars)
{
    if (!terms || !terms_count) return nullptr;
    if (*unioned_vars) NFA_variables_free(*unioned_vars);

    NFA* nfa_terms = NFA_from_linear_expression(terms[0]);
    NFA_variables* terms_vars = NFA_vars_from_linear_expr(terms[0]);
    NFA_variables_insert(terms_vars, 0, "*");

    for (int i = 1; i < terms_count; i++)
    {
        NFA* new_nfa = NFA_from_linear_expression(terms[i]);
        NFA_variables* new_vars = NFA_vars_from_linear_expr(terms[i]);
        NFA_variables_insert(new_vars, 0, "*");

        NFA_extend_rec(&nfa_terms, i);
        NFA_variables_insert(terms_vars, i, "*");

        for (int j = 0; j < i; j++)
        {
            NFA_extend_rec(&new_nfa, 0);
            NFA_variables_insert(new_vars, 0, "*");
        }

        sync_nfa_structure(&nfa_terms, &new_nfa, terms_vars, new_vars);
        NFA_intersect_rec(&nfa_terms, new_nfa);
        NFA_free(new_nfa);
        NFA_variables_free(new_vars);
    }

    (*unioned_vars) = terms_vars;
    return nfa_terms;
}

int merge_nfa_and_structure(NFA** added_nfa, NFA_variables* all_vars, NFA_variables* local_vars)
{
    if (!local_vars || !all_vars || !(*added_nfa) || ((*added_nfa)->alphabet_dim != local_vars->count)) return 0;

    int extend_size = all_vars->count - local_vars->count;
    // complete full structure
    for (int i = 0; i < local_vars->count; i++)
    {
        NFA_variables_add(all_vars, local_vars->variables[i]); // "IN" check is inside
    }

    // swap coordinates
    int k = 0;
    for (int i = 0; i < all_vars->count; i++)
    {
        char* var = all_vars->variables[i];
        if (NFA_variables_in(local_vars, var))
        {
            if (k != NFA_variables_index(local_vars, var))
            {
                int var_index = NFA_variables_index(local_vars, var);
                NFA_swap_rec(added_nfa, k, var_index);
                NFA_variables_swap(local_vars, k, var_index);
            }
            k++;
        }
    }

    // extend added_nfa
    for (int i = 0; i < all_vars->count; i++)
    {
        char* var = all_vars->variables[i];
        if (!NFA_variables_in(local_vars, var))
        {
            NFA_extend_rec(added_nfa, i);
            NFA_variables_add(local_vars, var);
        }
    }

    return extend_size;
}

#pragma endregion


#pragma region Regex

NFA* NFA_from_regex(const char* regex) {
    char* rpn = regex_to_rpn(regex);

    NFA_stack* nfa_stack = create_nfa_stack();

    for (const char* c = rpn; *c; c++) {
        if (isdigit(*c)) {
            NFA* operand_nfa = NFA_get_num_without_zeroes((int)(*c - '0'));
            push(nfa_stack, operand_nfa);
        }
        else if (*c == '.') {
            NFA* nfa = NFA_get_zero_or_one();
            push(nfa_stack, nfa);
        }
        else {
            handle_operation_regex(nfa_stack, *c);
        }
    }

    NFA* result = pop(nfa_stack);
    free_stack(nfa_stack);
    return result;
}

void handle_operation_regex(NFA_stack* stack, char op) {
    switch (op) {
    case '&': {
        NFA* nfa2 = pop(stack);
        NFA* nfa1 = pop(stack);
        NFA_concatenate_rec(&nfa1, nfa2);
        NFA_free(nfa2);
        push(stack, nfa1);
        break;
    }
    case '|': {
        NFA* nfa2 = pop(stack);
        NFA* nfa1 = pop(stack);
        NFA_union_rec(&nfa1, nfa2);
        push(stack, nfa1);
        NFA_free(nfa2);
        break;
    }
    case '*': {
        NFA* nfa = pop(stack);
        NFA_kleene_star_rec(&nfa);
        push(stack, nfa);
        break;
    }
    case '+': {
        NFA* nfa = pop(stack);
        NFA_plus_rec(&nfa);
        push(stack, nfa);
        break;
    }
    case '/': {
        // Right quotient
        NFA* nfa2 = pop(stack);
        NFA* nfa1 = pop(stack);
        NFA* result = NFA_rightquo(nfa1, nfa2);
        push(stack, result);
        NFA_free(nfa1);
        NFA_free(nfa2);
        break;
    }
    case '?': {
        NFA* nfa = pop(stack);
        NFA_with_empty_string_rec(&nfa);
        push(stack, nfa);
        break;
    }
    }
}

int precedence(char op) {
    switch(op) {
        case '|': return 1;
        case '&': return 2;
        case '*':
        case '+':
        case '?': return 3;
        default: return 0;
    }
}

char* regex_to_rpn(const char* regex)
{
    stack* operators = create_stack();
    size_t length = strlen(regex);
    char* rpn = (char*)malloc(length + 1);
    const char* reg = explicit_concatenation(regex);
    int j = 0; // rpn[j]

    for (const char *c = reg; *c; c++) {
        switch (*c){
            case '(':
                push(operators, *c);
                break;
            case ')':
                while (!is_stack_empty(operators) && stack_top(operators) != '(')
                {
                    rpn[j++] = (char)pop(operators);
                }
                pop(operators); // delete '(' from stack
                break;
            case '*':
            case '+':
            case '?':
            case '|':
            case '&':
                while (!is_stack_empty(operators) && precedence(*c) <= precedence((char)stack_top(operators))) {
                    rpn[j++] = (char)pop(operators);
                }
                push(operators, *c);
                break;
            default:
                rpn[j++] = *c;
        }
    }

    while (!is_stack_empty(operators)){
        rpn[j++] = (char)pop(operators);
    }
    rpn[j] = '\0';

    free_stack(operators);
    return rpn;
}

char* explicit_concatenation(const char* regex)
{
    size_t length = strlen(regex);
    char* result = (char*)malloc(sizeof(char) * (length + 1));
    int j = 0; // result[j];

    for (const char* c = regex; *c; c++){
        result[j++] = *c;
        if (*c != '(' && *c != '|' && *(c+1)) {
            char next_char = *(c+1);
            if (next_char != '*' && next_char != '+' && next_char != '?' &&
                next_char != '|' && next_char != ')' && !isspace(next_char)) {
                result[j++] = '&';
            }
        }
    }

    result[j] = '\0';
    return result;
}

#pragma endregion
