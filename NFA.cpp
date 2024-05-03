#include "NFA.h"
//
// Coded by Zelendzu & Umato on 29.02.2024.
//

// add epsilon-reached to accept methods and nfa_to_dfa where we search for epsilon-initial states
// NFA_remove_unreachable_states в minimize и других
// Убрать эпсилон переходы при НФА-ту-ДФА

#pragma region NFA_variables
NFA_variables* NFA_variables_init()
{
    NFA_variables* vars = (NFA_variables*)malloc(sizeof(NFA_variables));
    vars->count = 0;
    vars->variables = nullptr;

    return vars;
}

void NFA_variables_free(NFA_variables* vars)
{
    if (!vars) return;
    for (int i = 0; i < vars->count; i++)
    {
        free(vars->variables[i]);
    }
    free(vars->variables);
    free(vars);
}


void NFA_variables_add(NFA_variables* vars, const char* new_var)
{
    if (!vars || NFA_variables_in(vars, new_var)) return;

    vars->count++;
    vars->variables = (char**)realloc(vars->variables, sizeof(char*) * (vars->count));
    vars->variables[vars->count - 1] = (char*)calloc(strlen(new_var) + 1, sizeof(char));
    strcpy(vars->variables[vars->count - 1], new_var);
}

int NFA_variables_index(NFA_variables* vars, const char* var)
{
    if (!vars) return -1;
    for (int i = 0; i < vars->count; i++)
    {
        char* current_var = vars->variables[i];
        if ((strlen(var) == strlen(current_var)) && (strncmp(current_var, var, strlen(var)) == 0)) return i;
    }

    return -1;
}

bool NFA_variables_in(NFA_variables* vars, const char* var)
{
    return NFA_variables_index(vars, var) != -1;
}
#pragma endregion


#pragma region Others

list* list_init()
{
    list* new_list = (list*)malloc(sizeof(list));
    new_list->head = NULL;
    return new_list;
}

void list_free(list* l) {
    while (l && l->head) {
        node* tmp = l->head;
        l->head = l->head->next;
        free(tmp);
    }
    free(l);
}

void add_to_list(list* l, int val)
{
    node* new_node = (node*)malloc(sizeof(node));
    new_node->val = val;
    new_node->next = NULL;
    node* current = l->head;

    if (!current)
    {
        l->head = new_node;
    }
    else
    {
        while (current->next)
        {
            current = current->next;
        }

        current->next = new_node;
    }

    return;
}

void print_bin(unsigned number, unsigned int bits) {
    for (int i = (bits - 1); i >= 0; i--) {
        printf("%d", (number >> i) & 1);
    }
}

int get_random_num(int start, int end)
{
    if (start > end)
    {
        end = end + start;
        start = end - start;
        end = end - start;
    }

    return rand() % (end - start + 1) + start;
}

char* format_string_to_bin(const char* string)
{
    char* new_string = (char*)malloc(sizeof(char) * (strlen(string) + 1));
    int new_length = 0;
    for (int i = 0; i < strlen(string); i++)
    {
        if (string[i] == '0' || string[i] == '1')
        {
            new_string[new_length] = string[i];
            new_length++;
        }
    }
    new_string[new_length] = '\0';

    new_string = (char*)realloc(new_string, (new_length + 1));
    return new_string;
}

char* int_to_string(int num)
{
    char* string;

    if (!num)
    {
        string = (char*)malloc(sizeof(char) * (2));
        string[0] = '0';
        string[1] = '\0';
        return string;
    }

    num = abs(num);
    int bits_count = floor(log2(num)) + 1;
    string = (char*)malloc(sizeof(char) * (bits_count + 1));
    string[bits_count] = '\0';
    while (bits_count)
    {
        string[bits_count - 1] = (num & 1) ? '1' : '0';
        num >>= 1;
        bits_count--;
    }

    return string;
}

void to_lower_case(char *str) {
    for (; *str; ++str)
    {
        if (*str != 'E' && *str != 'A')
            *str = tolower((unsigned char)*str);
    }
}

char* extract_name(const char* token) {
    char* name = (char*)malloc(256 * sizeof(char));
    if (sscanf(token + 1, "%255[^ (]", name) == 1) {
        return name;
    }
    free(name);
    return nullptr;
}

#pragma endregion


#pragma region NFA Main

NFA_state* NFA_state_init(int id, bool is_final, int letters_count)
{
    NFA_state* state = (NFA_state*)malloc(sizeof(NFA_state));
    if (!state) return nullptr;


    state->id = id;
    state->is_final = is_final;
    state->transitions = (list**)malloc(sizeof(list*) * letters_count);
    if (!state->transitions) {
        free(state);
        return nullptr;
    }

    for (size_t i = 0; i < letters_count; i++) {
        state->transitions[i] = list_init();
        if (!state->transitions[i]) {
            while (i--) free(state->transitions[i]);
            free(state->transitions);
            free(state);
            return nullptr;
        }
    }

    return state;
}

NFA* NFA_init(int states_count, int alphabet_dim, int initial_state, int final_states_count, int* final_states)
{
    if ((initial_state >= states_count) || (final_states_count > states_count))
        return nullptr;

    size_t letters_count = (1 << alphabet_dim) + 1;
    NFA* nfa = (NFA*)malloc(sizeof(NFA));
    nfa->states_count = states_count;
    nfa->alphabet_dim = alphabet_dim;
    nfa->states = (NFA_state**)malloc(sizeof(NFA_state*) * states_count);

    for (size_t i = 0; i < states_count; i++) {
        nfa->states[i] = NFA_state_init(i, false, letters_count);
    }

    nfa->initial_state = nfa->states[initial_state];

    if (final_states)
    {
        for (size_t i = 0; i < final_states_count; i++)
        {
            if (final_states[i] < states_count)
            {
                nfa->states[final_states[i]]->is_final = true;
            }
        }
    }

    return nfa;
}

NFA* NFA_clone(NFA* nfa)
{
    if (!nfa) return nullptr;
    int final_states_count = 0;
    int* final_states = NFA_get_final_states(nfa, &final_states_count);
    NFA* new_nfa = NFA_init(nfa->states_count, nfa->alphabet_dim, nfa->initial_state->id, final_states_count, final_states);
    free(final_states);

    for (int i = 0; i < nfa->states_count; i++)
    {
        for (int letter = 0; letter <= (1 << nfa->alphabet_dim); letter++)
        {
            NFA_transitions_list_add(new_nfa, i, nfa->states[i]->transitions[letter], letter);
        }
    }

    return new_nfa;
}

void NFA_transition_add(NFA* nfa, int start_state, int end_state, int letter)
{
    if (!nfa || (letter > (1 << nfa->alphabet_dim)) || (start_state >= nfa->states_count) || (end_state >= nfa->states_count))
        return;

    node* new_state = (node*)malloc(sizeof(node));
    new_state->next = NULL;
    new_state->val = end_state;

    if (!nfa->states[start_state]->transitions[letter]->head)
    {
        nfa->states[start_state]->transitions[letter]->head = new_state;
    }
    else
    {
        node* cur_state = nfa->states[start_state]->transitions[letter]->head;
        while (cur_state->val != end_state && cur_state->next)
        {
            cur_state = cur_state->next;
        }

        if (cur_state->val != end_state) cur_state->next = new_state;
        else free(new_state);
    }
}

void NFA_transition_remove(NFA* nfa, int start_state, int end_state, int letter) {
    if (!nfa || start_state >= nfa->states_count || end_state >= nfa->states_count || letter > (1 << nfa->alphabet_dim)) return;

    list* trans = nfa->states[start_state]->transitions[letter];
    node** current = &trans->head;
    while (*current && (*current)->val != end_state) {
        current = &(*current)->next;
    }

    if (*current) {
        node* temp = *current;
        *current = (*current)->next;
        free(temp);
    }
}

bool NFA_state_add(NFA* nfa, bool is_final)
{
    if (!nfa) return false;

    int id = nfa->states_count;

    nfa->states = (NFA_state**)realloc(nfa->states, (nfa->states_count + 1) * sizeof(NFA_state*));

    NFA_state* newState = NFA_state_init(id, is_final, (1 << nfa->alphabet_dim) + 1);

    nfa->states[id] = newState;
    nfa->states_count++;

    return true;
}

bool NFA_state_remove(NFA* nfa, int id)
{
    if (!nfa || id < 0 || id >= nfa->states_count) return false;

    if (id == nfa->initial_state->id)
    {
        if (nfa->states_count == 1) nfa->initial_state = nullptr;
        else nfa->initial_state = nfa->states[(id + 1) % nfa->states_count];
    }

    // free memory of deleted state
    for (size_t i = 0; i <= (1 << nfa->alphabet_dim); i++) {
        list_free(nfa->states[id]->transitions[i]);
    }
    free(nfa->states[id]->transitions);
    free(nfa->states[id]);

    // move on 1 index left all states after the current one
    for (int i = id; i < nfa->states_count - 1; i++)
    {
        nfa->states[i] = nfa->states[i + 1];
        nfa->states[i]->id = i;
    }

    // reallocate memory
    nfa->states[nfa->states_count - 1] = nullptr;
    nfa->states = (NFA_state**)realloc(nfa->states, (nfa->states_count - 1) * sizeof(NFA_state*));
    nfa->states_count--;

    // in every transitions[letter] list:
    //      delete all nodes with val=id
    //      edit all nodes with val>id so that val -= 1

    for (int i = 0; i < nfa->states_count; i++)
    {
        for (int letter = 0; letter <= (1 << nfa->alphabet_dim); letter++)
        {
            node** prev = &nfa->states[i]->transitions[letter]->head;
            node* current = nfa->states[i]->transitions[letter]->head;

            while (current)
            {
                if (current->val == id)
                {
                    if (*prev == current) // if node.val=id is at the beginning of list
                    {
                        current = current->next;
                        free(*prev);
                        *prev = current;
                    }
                    else
                    {
                        current = current->next;
                        free((*prev)->next);
                        (*prev)->next = current;
                    }
                }
                else
                {
                    if (current->val > id)
                    {
                        current->val--;
                    }
                    prev = &current;
                    current = current->next;
                }
            }
        }
    }



    return true;
}

bool NFA_state_list_remove(NFA* nfa, int* removed_states, int list_size)
{
    // removed_states has to be in the next format: [0, 1, 1, 0, ..., 0], where states with "1" should be removed

    if (!nfa || list_size != nfa->states_count) return false;

    // change initial state if current one should be removed
    int counter = 0;
    while (removed_states[(nfa->initial_state->id + counter) % nfa->states_count])
    {
        counter++;
        if (counter == nfa->states_count)
        {
            nfa->initial_state = nullptr;
            break;
        }
    }
    nfa->initial_state = nfa->states[(nfa->initial_state->id + counter) % nfa->states_count];

    // free memory of deleted states
    // initialize new_states_id, which has format: [new_id_of_state0, new_id_of_state1, ...]
    int* new_states_id = (int*)malloc(sizeof(int) * nfa->states_count);
    for (int id = 0; id < nfa->states_count; id++)
    {
        new_states_id[id] = -1;

        if (removed_states[id])
        {
            for (size_t letter = 0; letter <= (1 << nfa->alphabet_dim); letter++) {
                list_free(nfa->states[id]->transitions[letter]);
            }
            free(nfa->states[id]->transitions);
            free(nfa->states[id]);
        }
    }

    // move states left
    int next_state = 0; // id of next state that is not removed

    for (int i = 0; i < nfa->states_count; i++)
    {
        while (next_state < nfa->states_count && removed_states[next_state])
        {
            next_state++;
        }
        if (next_state >= nfa->states_count)
        {
            next_state = i; // remaining states count
            break;
        }

        nfa->states[i] = nfa->states[next_state];
        nfa->states[i]->id = i;
        new_states_id[next_state] = i;
        next_state++;
    }

    // reallocate memory
    nfa->states_count = next_state;
    nfa->states = (NFA_state**)realloc(nfa->states, (nfa->states_count) * sizeof(NFA_state*));

    // in every transitions[letter] list:
    //      delete all nodes with removed_states[id] = 1
    //      edit all nodes ids

    for (int i = 0; i < nfa->states_count; i++)
    {
        for (int letter = 0; letter <= (1 << nfa->alphabet_dim); letter++)
        {
            node** prev = &nfa->states[i]->transitions[letter]->head;
            node* current = nfa->states[i]->transitions[letter]->head;

            while (current)
            {
                if (removed_states[current->val])
                {
                    if (*prev == current) // if node.val=removed_id is at the beginning of list
                    {
                        current = current->next;
                        free(*prev);
                        *prev = current;
                    }
                    else
                    {
                        current = current->next;
                        free((*prev)->next);
                        (*prev)->next = current;
                    }
                }
                else
                {
                    current->val = new_states_id[current->val];
                    prev = &current;
                    current = current->next;
                }
            }
        }
    }

    free(new_states_id);
    return true;
}

void NFA_transitions_list_add(NFA* nfa, int start_state, list* end_states, int letter)
{
    if (!nfa || start_state >= nfa->states_count || letter > (1 << nfa->alphabet_dim)) return;
    node* end_state = end_states->head;
    while (end_state)
    {
        NFA_transition_add(nfa, start_state, end_state->val, letter);
        end_state = end_state->next;
    }
}

void NFA_states_free(NFA* nfa)
{
    if (!nfa) return;
    nfa->initial_state = nullptr;
    for (int i = 0; i < nfa->states_count; i++) {
        NFA_state* state = nfa->states[i];
        if (state) {
            for (size_t j = 0; j <= (1 << nfa->alphabet_dim); j++) {
                list_free(state->transitions[j]);
            }
            free(state->transitions);
            free(state);
        }
    }
    free(nfa->states);
}

void NFA_free(NFA* nfa)
{
    if (!nfa) return;
    NFA_states_free(nfa);
    free(nfa);
}

bool NFA_accept(NFA* nfa, char* num)
{
    char* new_num = format_string_to_bin(num);
    if (!nfa || !new_num || strlen(new_num) == 0) return 0;

    stack* current_states;
    stack* next_states;
    bool* next_states_added;

    char bit;
    size_t total_bits = strlen(new_num);

    current_states = NFA_get_reachable_states(nfa, nfa->initial_state->id, (1 << nfa->alphabet_dim));
    push(current_states, nfa->initial_state->id);

    for (int i = 0; i < total_bits; i++)
    {
        next_states_added = (bool*)calloc(nfa->states_count, sizeof(bool)); // 0: state is not in "next_states", 1: state IS in "next_states"
        next_states = create_stack(); // stack of states we can reach from current set of states

        bit = new_num[total_bits - i - 1] == '1';

        while (!is_stack_empty(current_states))
        {
            node* dest = nfa->states[pop(current_states)]->transitions[bit]->head;

            while (dest) // check all states we can reach from this current_state
            {
                if (!next_states_added[dest->val]) // if destination_state is not in the next_states
                {
                    push(next_states, dest->val); // push it into next_states
                    next_states_added[dest->val] = 1;

                    // we also check all states that are epsilon-reached from this destination_state
                    // and push them in next_states
                    queue* eps_queue = create_queue();
                    enqueue(eps_queue, dest->val);
                    while (!is_queue_empty(eps_queue))
                    {
                        node* eps_dest = nfa->states[dequeue(eps_queue)]->transitions[(1 << nfa->alphabet_dim)]->head;
                        while (eps_dest)
                        {
                            if (!next_states_added[eps_dest->val])
                            {
                                enqueue(eps_queue, eps_dest->val);
                                push(next_states, eps_dest->val);
                                next_states_added[eps_dest->val] = 1;
                            }
                            eps_dest = eps_dest->next;
                        }
                    }
                    free_queue(eps_queue);
                }
                dest = dest->next;
            }
        }

        free_stack(current_states);
        free(next_states_added);
        current_states = next_states;
        next_states = nullptr;
    }

    free(new_num);
    // check if any final state in current_states
    while (!is_stack_empty(current_states))
    {
        int state_id = pop(current_states);
        if (nfa->states[state_id]->is_final)
        {
            free_stack(current_states);
            return 1;
        }
    }


    free_stack(current_states);
    return 0;
}

bool NFA_accept(NFA* nfa, char** nums, int nums_count)
{
    if (!nfa || !nums || nums_count != nfa->alphabet_dim) return 0;

    char** new_nums = (char**)malloc(sizeof(char*) * nums_count);
    int max_length = 0;
    for (int i = 0; i < nums_count; i++)
    {
        new_nums[i] = format_string_to_bin(nums[i]);
        if (strlen(new_nums[i]) > max_length) max_length = strlen(new_nums[i]);
    }

    for (int i = 0; i < nums_count; i++)
    {
        int zeroes_count = max_length - strlen(new_nums[i]);
        if (zeroes_count > 0)
        {
            char* new_string = (char*)calloc(max_length + 1, sizeof(char));
            memset(new_string, '0', zeroes_count);
            memcpy(new_string + zeroes_count, new_nums[i], strlen(new_nums[i]));
            new_string[max_length] = '\0';
            free(new_nums[i]);
            new_nums[i] = new_string;
        }
    }

    stack* current_states = create_stack();
    stack* next_states;
    bool* next_states_added;
    int total_bits = max_length;

    current_states = NFA_get_reachable_states(nfa, nfa->initial_state->id, (1 << nfa->alphabet_dim));
    push(current_states, nfa->initial_state->id);

    for (int i = 0; i < total_bits; i++)
    {
        next_states_added = (bool*)calloc(nfa->states_count, sizeof(bool));
        next_states = create_stack();
        int letter = 0;

        for (int j = 0; j < nums_count; j++)
        {
            letter <<= 1;
            letter += new_nums[j][total_bits - 1 - i] == '1' ? 1 : 0;
        }

        while (!is_stack_empty(current_states))
        {
            node* dest = nfa->states[pop(current_states)]->transitions[letter]->head;
            while (dest)
            {
                if (!next_states_added[dest->val])
                {
                    push(next_states, dest->val);
                    next_states_added[dest->val] = 1;

                    // check epsilon-reached states
                    queue* eps_queue = create_queue();
                    enqueue(eps_queue, dest->val);
                    while (!is_queue_empty(eps_queue))
                    {
                        node* eps_dest = nfa->states[dequeue(eps_queue)]->transitions[(1 << nfa->alphabet_dim)]->head;
                        while (eps_dest)
                        {
                            if (!next_states_added[eps_dest->val])
                            {
                                enqueue(eps_queue, eps_dest->val);
                                push(next_states, eps_dest->val);
                                next_states_added[eps_dest->val] = 1;
                            }
                            eps_dest = eps_dest->next;
                        }
                    }
                    free_queue(eps_queue);
                }
                dest = dest->next;
            }
        }

        free_stack(current_states);
        free(next_states_added);
        current_states = next_states;
        next_states = nullptr;
    }

    while (!is_stack_empty(current_states))
    {
        int state_id = pop(current_states);
        if (nfa->states[state_id]->is_final)
        {
            free_stack(current_states);
            return 1;
        }
    }

    free_stack(current_states);
    return 0;
}

bool NFA_accept(NFA* nfa, big_int* num)
{
    if (!nfa || !num) return 0;
    char* num_string = big_int_to_string(num);
    bool result = NFA_accept(nfa, num_string);
    free(num_string);
    return result;
}

bool NFA_accept(NFA* nfa, big_int_list* bigint_list)
{
    if (!nfa || !bigint_list || nfa->alphabet_dim != bigint_list->count) return 0;

    char** nums_list = (char**)malloc(sizeof(char*) * bigint_list->count);
    for (int i = 0; i < bigint_list->count; i++)
    {
        nums_list[i] = big_int_to_string(bigint_list->big_ints[i]);
    }

    int result = NFA_accept(nfa, nums_list, bigint_list->count);

    for (int i = 0; i < bigint_list->count; i++)
    {
        free(nums_list[i]);
    }
    free(nums_list);

    return result;
}

#pragma endregion


#pragma region NFA Input/Output

void NFA_print(NFA* nfa) {
    if (nfa == nullptr) {
        std::cout << "nfa is NULL." << std::endl;
        return;
    }

    std::cout << "NFA: States Count = " << nfa->states_count
        << ", Alphabet Size = " << nfa->alphabet_dim << std::endl;
    std::cout << "Initial State: " << nfa->initial_state->id << std::endl;
    std::cout << "States:" << std::endl;

    for (size_t i = 0; i < nfa->states_count; i++) {
        NFA_state* state = nfa->states[i];
        if (state == nullptr) continue;

        std::cout << " State " << state->id << (state->is_final ? " [final]" : "") << std::endl;

        for (size_t letter = 0; letter <= (1 << nfa->alphabet_dim); letter++) {
            list* transition_list = state->transitions[letter];
            if (transition_list == nullptr || transition_list->head == nullptr) {
                continue;
            }

            std::cout << "  On '";
            if (letter < (1 << nfa->alphabet_dim))
                print_bin(letter, nfa->alphabet_dim);
            else
                printf("E");
            std::cout << "': ";
            node* current = transition_list->head;
            while (current != nullptr) {
                std::cout << current->val << " ";
                current = current->next;
            }
            std::cout << std::endl;
        }
    }
}

void NFA_to_DOT(NFA* nfa)
{
    if (nfa == nullptr) {
        printf("Invalid NFA.\n");
        return;
    }

    char base_filename[] = "../NFA/NFA";
    char extension[] = ".gv";
    char filename[40];
    FILE* file;
    int counter = 0;

    // forming filename
    do {
        if (counter == 0) {
            sprintf(filename, "%s%s", base_filename, extension);
        }
        else {
            fclose(file);
            sprintf(filename, "%s%d%s", base_filename, counter, extension);
        }
        file = fopen(filename, "r");
        if (file != nullptr) {
            counter++;
        }
    } while (file != nullptr);

    file = fopen(filename, "w");

    fprintf(file, "digraph finite_state_machine {\n");
    fprintf(file, "\trankdir=LR;\n");

    char final_states_str[256] = "";
    bool has_final_states = false;

    for (size_t i = 0; i < nfa->states_count; i++) {
        if (nfa->states[i]->is_final) {
            char state_id_str[32];
            sprintf(state_id_str, " %d", nfa->states[i]->id);
            strcat(final_states_str, state_id_str);
            has_final_states = true;
        }
    }

    if (has_final_states) {
        fprintf(file, "\tnode [shape = doublecircle];%s;\n", final_states_str);
    }

    fprintf(file, "\tnode [shape = circle];\n");
    fprintf(file, "\tinit [shape=none, label=\"\"];\n");
    fprintf(file, "\tinit -> %d;\n", nfa->initial_state->id);

    for (size_t i = 0; i < nfa->states_count; i++) {
        // Ensure that all states are accounted for by declaring them even if no transitions are listed
        fprintf(file, "\t%d [label=\"%d\"];\n", i, i);
    }

    // structure for storing labels of transitions
    char*** labels = (char***)malloc(nfa->states_count * sizeof(char**));
    for (size_t i = 0; i < nfa->states_count; i++) {
        labels[i] = (char**)malloc(nfa->states_count * sizeof(char*));
        for (size_t j = 0; j < nfa->states_count; j++) {
            labels[i][j] = (char*)calloc(128, sizeof(char)); // Assuming enough space for labels
        }
    }

        for (size_t i = 0; i < nfa->states_count; i++) {
        NFA_state* state = nfa->states[i];
        for (size_t letter = 0; letter <= (1 << nfa->alphabet_dim); letter++) {
            list* transition_list = state->transitions[letter];
            if (!transition_list || !transition_list->head) {
                continue;
            }

            node* current = transition_list->head;
            while (current) {
                if (strlen(labels[i][current->val]) > 0) {
                    strcat(labels[i][current->val], ",");
                }
                char letter_str[10];
                if (letter == (1 << nfa->alphabet_dim)) {
                    sprintf(letter_str, "E");
                } else {
                    char binary_representation[32] = "";
                    for (int bit = nfa->alphabet_dim - 1; bit >= 0; --bit) {
                        strcat(binary_representation, (letter & (1 << bit)) ? "1" : "0");
                    }
                    strcpy(letter_str, binary_representation);
                }
                strcat(labels[i][current->val], letter_str);

                current = current->next;
            }
        }
    }

    // Output transitions with aggregated labels
    for (size_t i = 0; i < nfa->states_count; i++) {
        for (size_t j = 0; j < nfa->states_count; j++) {
            if (strlen(labels[i][j]) > 0) {
                fprintf(file, "\t%d -> %d [label=\"%s\"];\n", i, j, labels[i][j]);
            }
        }
    }

    fprintf(file, "}\n");
    fclose(file);

//    // free memory
    for (int i = 0; i < nfa->states_count; i++) {
        for (int j = 0; j < nfa->states_count; j++) {
            free(labels[i][j]);
        }
        free(labels[i]);
    }
    free(labels);

    printf("NFA has been written to %s\n", filename);
    if (counter == 0) {
        sprintf(filename, "%s", base_filename);
    } else {
        sprintf(filename, "%s%d", base_filename, counter);
    }
    generate_png_from_dot(filename);
}

void generate_png_from_dot(char* dot_filename) {
    char command[256];

    sprintf(command, "dot -Tpng %s.gv -o %s.png", dot_filename, dot_filename);
    system(command);

    sprintf(command, "start %s.png", dot_filename);
    system(command);
}

void NFA_to_file(NFA* automaton, const char* filename)
{
    if (automaton == nullptr || filename == nullptr) {
        cout << "Invalid automaton or filename.\n";
        return;
    }

    FILE* file = fopen(filename, "w");
    if (!file) {
        cout << "Failed to open file for writing.\n";
        return;
    }

    fprintf(file, "lsd_%d\n\n", automaton->alphabet_dim);

    int initial_state_id = automaton->initial_state->id;

    for (size_t i = 0; i < automaton->states_count; i++) {
        NFA_state* state = automaton->states[i];
        if (state == nullptr) continue;

        int state_type = -1; // Default to neither initial nor final (-1).
        if (state->id == initial_state_id && state->is_final) {
            state_type = 2; // Both initial and final (2)
        }
        else if (state->id == initial_state_id) {
            state_type = 0; // Only initial (0)
        }
        else if (state->is_final) {
            state_type = 1; // Only final (1)
        }

        fprintf(file, "%d %d\n", state->id, state_type);

        for (size_t letter = 0; letter <= (1 << automaton->alphabet_dim); letter++) {
            list* transition_list = state->transitions[letter];
            if (transition_list == nullptr || transition_list->head == nullptr) continue;

            node* current = transition_list->head;
            while (current != nullptr) {
                if (letter == (1 << automaton->alphabet_dim)) {
                    fprintf(file, "%s ", "E");
                } else {
                    for (size_t bit = 0; bit < automaton->alphabet_dim; bit++) {
                        fprintf(file, "%d ", (letter >> (automaton->alphabet_dim - 1 - bit)) & 1);
                    }
                }

                fprintf(file, "-> %d\n", current->val);
                current = current->next;
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
    cout << "NFA has been written to " << filename << endl;
}

NFA* NFA_from_file(const char* filename)
{
    if (filename == nullptr) {
        cout << "Invalid filename.\n";
        return nullptr;
    }

    FILE* file = fopen(filename, "r");
    if (file == nullptr) {
        cout << "Unable to open file.\n";
        return nullptr;
    }

    int alphabet_dim, state_id, state_type, end_state, bit;

    if (fscanf(file, "lsd_%d\n\n", &alphabet_dim) != 1) {
        cout << "Error reading alphabet dimension.\n";
        fclose(file);
        return nullptr;
    }

    NFA* automaton = NFA_init(0, alphabet_dim, -1, 0, nullptr);

    int initial_state = -1;
    char line[256];

    while (fgets(line, sizeof(line), file))
    {

        int state_id, state_type;

        if (sscanf(line, "%d %d\n", &state_id, &state_type) == 2)
        {
            if (state_id >= automaton->states_count)
            {
                for (int i = automaton->states_count; i <= state_id; i++)
                {
                    NFA_state_add(automaton, state_type == 1 || state_type == 2);
                }
            }
            else {
                automaton->states[state_id]->is_final = state_type == 1 || state_type == 2;
            }

            if (state_type == 0 || state_type == 2)
            {
                if (initial_state == -1)
                {
                    initial_state = state_id;
                }
                else
                {
                    cout << "Multiple initial states encountered, which is invalid.\n";
                    NFA_free(automaton);
                    fclose(file);
                    return nullptr;
                }
            }

            while (fgets(line, sizeof(line), file) && line[0] != '\n')
            {
                int letter, end_state;
                int bit;
                if (line[0] == 'E') {
                    if (sscanf(line, "E -> %d\n", &end_state) == 1) {
                        if (end_state >= automaton->states_count)
                        {
                            for (int i = automaton->states_count; i <= end_state; i++)
                            {
                                NFA_state_add(automaton, false);
                            }
                        }
                        NFA_transition_add(automaton, state_id, end_state, alphabet_dim<<1);
                        continue;
                    };
                }
                if (sscanf(line, "%d ", &letter) == 1)
                {
                    char* ptr = line + 2;
                    int offset;

                    for (int i = 0; i < alphabet_dim - 1; i++)
                    {
                        sscanf(ptr, "%d%n", &bit, &offset);
                        ptr += 2;
                        letter = (letter << 1) | bit;
                    }

                    if (sscanf(ptr, "-> %d\n", &end_state) == 1)
                    {
                        if (end_state >= automaton->states_count)
                        {
                            for (int i = automaton->states_count; i <= end_state; i++)
                            {
                                NFA_state_add(automaton, false);
                            }
                        }
                        NFA_transition_add(automaton, state_id, end_state, letter);
                    }
                    else break;


                }
            }

        }
    }

    if (initial_state == -1) {
        cout << "Error: there no initial state\n";
        fclose(file);
        NFA_free(automaton);
        return nullptr;
    }
    else {
        automaton->initial_state = automaton->states[initial_state];
    }

    fclose(file);
    return automaton;
}

NFA* load_NFA_from_file(const char* filename)
{
    if (strncmp(filename, "div_", 4) == 0 || strncmp(filename, "div", 3) == 0) {
        int number;
        if (sscanf(filename, "div_%d", &number) == 1 || sscanf(filename, "div%d", &number) == 1) {
            return NFA_get_div_a(number);
        }
    }

    char path[256];
    sprintf(path, "../NFA/NFAs/%s.txt", filename);

    return NFA_from_file(path);
}

#pragma endregion


#pragma region NFA Operations

NFA* NFA_intersect(NFA* nfa1, NFA* nfa2)
{
    if (!nfa1 || !nfa2 || nfa1->alphabet_dim != nfa2->alphabet_dim) return nullptr;

    int combined_states_count = nfa1->states_count * nfa2->states_count;
    int alphabet_dim = nfa1->alphabet_dim;

    NFA* intersected_nfa = NFA_init(combined_states_count, alphabet_dim, 0, 0, nullptr);

    for (size_t i = 0; i < nfa1->states_count; i++) {
        for (size_t j = 0; j < nfa2->states_count; j++) {
            int combined_state_id = i * nfa2->states_count + j;
            NFA_state* combined_state = intersected_nfa->states[combined_state_id];

            combined_state->is_final = nfa1->states[i]->is_final && nfa2->states[j]->is_final;

            for (size_t letter = 0; letter < (1 << alphabet_dim); letter++)
            {
                list* list1 = nfa1->states[i]->transitions[letter];
                list* list2 = nfa2->states[j]->transitions[letter];

                for (node* node1 = list1->head; node1; node1 = node1->next) {
                    for (node* node2 = list2->head; node2; node2 = node2->next) {
                        int new_end_state = node1->val * nfa2->states_count + node2->val;
                        NFA_transition_add(intersected_nfa, combined_state_id, new_end_state, letter);
                    }
                }
            }

            list* eps_list1 = nfa1->states[i]->transitions[1 << alphabet_dim];
            list* eps_list2 = nfa2->states[j]->transitions[1 << alphabet_dim];

            for (node* eps1 = eps_list1->head; eps1; eps1 = eps1->next) {
                int target_state = eps1->val * nfa2->states_count + j;
                NFA_transition_add(intersected_nfa, combined_state_id, target_state, 1 << alphabet_dim);
            }
            for (node* eps2 = eps_list2->head; eps2; eps2 = eps2->next) {
                int target_state = i * nfa2->states_count + eps2->val;
                NFA_transition_add(intersected_nfa, combined_state_id, target_state, 1 << alphabet_dim);
            }

        }
    }

    intersected_nfa->initial_state = intersected_nfa->states[nfa1->initial_state->id * nfa2->states_count + nfa2->initial_state->id];

    return intersected_nfa;

}

void NFA_intersect_rec(NFA** nfa1, NFA* nfa2)
{
    // we use ** because of nullptr
    NFA* nfa_intersected = NFA_intersect(*nfa1, nfa2);
    NFA_free(*nfa1);
    *nfa1 = nfa_intersected;
}

NFA* NFA_union(NFA* nfa1, NFA* nfa2)
{
    if (!nfa2) return NFA_clone(nfa1);
    if (!nfa1) return NFA_clone(nfa2);
    if (nfa1->alphabet_dim != nfa2->alphabet_dim) return nullptr;

    // + 1, because the added state is the new initial one
    int combined_states_count = nfa1->states_count * nfa2->states_count + 2;
    int alphabet_dim = nfa1->alphabet_dim;

    NFA* unioned_NFA = NFA_init(combined_states_count, alphabet_dim, 0, 0, nullptr);

    // adding epsilon-transitions from new initial state to old initial states of both NFA
    NFA_transition_add(unioned_NFA, 0, nfa1->initial_state->id + 1, (1 << alphabet_dim));
    NFA_transition_add(unioned_NFA, 0, nfa2->initial_state->id + 1 + nfa1->states_count, (1 << alphabet_dim));

    for (size_t i = 0; i < nfa1->states_count; i++) {
        for (size_t letter = 0; letter <= (1 << alphabet_dim); letter++) {
            for (node* trans = nfa1->states[i]->transitions[letter]->head; trans != nullptr; trans = trans->next) {
                NFA_transition_add(unioned_NFA, i + 1, trans->val + 1, letter);
            }
        }
        if (nfa1->states[i]->is_final) {
            unioned_NFA->states[i + 1]->is_final = true;
        }
    }

    for (int i = 0; i < nfa2->states_count; i++) {
        for (int letter = 0; letter <= (1 << alphabet_dim); letter++) {
            for (node* trans = nfa2->states[i]->transitions[letter]->head; trans != nullptr; trans = trans->next) {
                NFA_transition_add(unioned_NFA, i + 1 + nfa1->states_count, trans->val + 1 + nfa1->states_count, letter);
            }
        }
        if (nfa2->states[i]->is_final) {
            unioned_NFA->states[i + 1 + nfa1->states_count]->is_final = true;
        }
    }

    return unioned_NFA;
}

void NFA_union_rec(NFA** nfa1, NFA* nfa2)
{
    NFA* nfa_unioned = NFA_union(*nfa1, nfa2);
    NFA_free(*nfa1);
    *nfa1 = nfa_unioned;
}

NFA* DFA_complement(NFA* nfa)
{
    if (!nfa) return nullptr;

    NFA* complement_dfa = NFA_clone(nfa);
    DFA_make_complete_rec(&complement_dfa);
    if (!complement_dfa) return nullptr;

    for (int i = 0; i < complement_dfa->states_count; i++)
    {
        complement_dfa->states[i]->is_final = !complement_dfa->states[i]->is_final;
    }

    DFA_minimize_rec(&complement_dfa);
    return complement_dfa;
}

void DFA_complement_rec(NFA** nfa)
{
    NFA* dfa_complement = DFA_complement(*nfa);
    NFA_free(*nfa);
    *nfa = dfa_complement;
}

NFA* NFA_project(NFA* nfa, unsigned char n)
{
    if (!nfa || n >= nfa->alphabet_dim) return nullptr;

    n = nfa->alphabet_dim - n; // old n = coordinate number (start from left = 0); new n = digit (разряд) number (start from right = 0)
    int final_states_count = 0;
    int* final_states = NFA_get_final_states(nfa, &final_states_count);
    NFA* new_nfa = NFA_init(nfa->states_count, nfa->alphabet_dim - 1, nfa->initial_state->id, final_states_count, final_states);
    free(final_states);

    for (int i = 0; i < nfa->states_count; i++)
    {
        for (int letter = 0; letter < (1 << nfa->alphabet_dim); letter++)
        {
            int new_letter = ((letter >> n) << (n - 1)) | (letter & ((1 << (n - 1)) - 1));
            NFA_transitions_list_add(new_nfa, i, nfa->states[i]->transitions[letter], new_letter);
        }
        NFA_transitions_list_add(new_nfa, i, nfa->states[i]->transitions[1 << nfa->alphabet_dim], 1 << new_nfa->alphabet_dim);
    }

    return new_nfa;
}

void NFA_project_rec(NFA** nfa, unsigned char n)
{
    NFA* nfa_project = NFA_project(*nfa, n);
    NFA_free(*nfa);
    *nfa = nfa_project;
}

NFA* NFA_project(NFA* nfa, unsigned char n, bool with_quotient)
{
    if (!nfa || n >= nfa->alphabet_dim) return nullptr;
    NFA* result;
    if (with_quotient)
    {
        NFA* zeroes = NFA_get_only_zeroes(nfa->alphabet_dim - 1);
        NFA_extend_rec(&zeroes, n);
        result = NFA_rightquo(nfa, zeroes);
        NFA_union_rec(&result, nfa);
        NFA_project_rec(&result, n);
        NFA_free(zeroes);
    }
    else
    {
        result = NFA_project(nfa, n);
    }

    return result;
}

void NFA_project_rec(NFA** nfa, unsigned char n, bool with_quotient)
{
    NFA* nfa_project = NFA_project(*nfa, n, 1);
    NFA_free(*nfa);
    *nfa = nfa_project;
}

NFA* NFA_extend(NFA* nfa, unsigned char n)
{
    if (!nfa || n > nfa->alphabet_dim) return nullptr;
    n = nfa->alphabet_dim - n;
    int final_states_count = 0;
    int* final_states = NFA_get_final_states(nfa, &final_states_count);
    NFA* new_nfa = NFA_init(nfa->states_count, nfa->alphabet_dim + 1, nfa->initial_state->id, final_states_count, final_states);
    free(final_states);

    for (int i = 0; i < nfa->states_count; i++)
    {
        for (int letter = 0; letter < (1 << nfa->alphabet_dim); letter++)
        {
            int new_letter0 = ((((letter >> n) << 1) | 0) << n) | (letter & ((1 << n) - 1));
            int new_letter1 = ((((letter >> n) << 1) | 1) << n) | (letter & ((1 << n) - 1));
            NFA_transitions_list_add(new_nfa, i, nfa->states[i]->transitions[letter], new_letter0);
            NFA_transitions_list_add(new_nfa, i, nfa->states[i]->transitions[letter], new_letter1);
        }
        NFA_transitions_list_add(new_nfa, i, nfa->states[i]->transitions[1 << nfa->alphabet_dim], 1 << new_nfa->alphabet_dim);
    }

    return new_nfa;
}

void NFA_extend_rec(NFA** nfa, unsigned char n)
{
    NFA* nfa_extend = NFA_extend(*nfa, n);
    NFA_free(*nfa);
    *nfa = nfa_extend;
}

NFA* NFA_leftquo(NFA* nfa1, NFA* nfa2)
{
    // new nfa is the same as nfa1 except for initial states
    // nfa2 should be EXTENDED to have the same alphabet_dim as nfa1 (each time differently)

    if (!nfa1 || !nfa2 || nfa2->alphabet_dim != nfa1->alphabet_dim) return nullptr;

    stack* initial_states = create_stack();
    NFA* inter_nfa = NFA_intersect(nfa1, nfa2);
    NFA_remove_unreachable_states_rec(inter_nfa);

    // combined_state_id = i * nfa2->states_count + j;
    // if nfa2.states[j] is final => nfa1.states[i] is new initial
    for (int i = 0; i < inter_nfa->states_count; i++)
    {
        int j = i % nfa2->states_count;
        if (nfa2->states[j]->is_final)
        {
            push(initial_states, i / nfa2->states_count);
        }
    }

    NFA_free(inter_nfa);
    NFA* new_nfa = NFA_clone(nfa1);
    // add new initial state
    if (!NFA_state_add(new_nfa, false))
    {
        NFA_free(new_nfa);
        free_stack(initial_states);
        return nullptr;
    }

    int new_initial = new_nfa->states_count - 1;
    new_nfa->initial_state = new_nfa->states[new_initial];
    // add epsilon-transitions from new initial state to states from stack
    while (!is_stack_empty(initial_states))
    {
        NFA_transition_add(new_nfa, new_initial, pop(initial_states), (1 << new_nfa->alphabet_dim));
    }

    free_stack(initial_states);
    return new_nfa;
}

void NFA_leftquo_rec(NFA** nfa1, NFA* nfa2)
{
    NFA* nfa_leftquot = NFA_leftquo(*nfa1, nfa2);
    NFA_free(*nfa1);
    *nfa1 = nfa_leftquot;
}

void NFA_leftquo_unioned(NFA** nfa1, NFA* nfa2)
{
    NFA* nfa_leftquo = NFA_leftquo(*nfa1, nfa2);
    NFA_union_rec(nfa1, nfa_leftquo);
    NFA_free(nfa_leftquo);
}

NFA* NFA_rightquo(NFA* nfa1, NFA* nfa2)
{
    // new nfa is the same as nfa1 except for final states
    // nfa2 should be EXTENDED to have the same alphabet_dim as nfa1 (each time differently)

    if (!nfa1 || !nfa2 || nfa2->alphabet_dim != nfa1->alphabet_dim) return nullptr;

    NFA* new_nfa = NFA_clone(nfa1);
    NFA* nfa2_clone = NFA_clone(nfa2);

    // if there exist a path from current state to a final state that accepts a word from nfa2 language
    // then current state will be one of the new final states
    int* state_is_new_final = (int*)calloc(new_nfa->states_count, sizeof(int));
    for (int i = 0; i < new_nfa->states_count; i++)
    {
        new_nfa->initial_state = new_nfa->states[i];
        NFA* inter_nfa = NFA_intersect(new_nfa, nfa2_clone);
        NFA_remove_unreachable_states_rec(inter_nfa);

        if (NFA_get_final_states_count(inter_nfa) != 0)
        {
            state_is_new_final[i] = 1;
        }

        NFA_free(inter_nfa);
    }

    new_nfa->initial_state = new_nfa->states[nfa1->initial_state->id];
    for (int i = 0; i < new_nfa->states_count; i++)
    {
        new_nfa->states[i]->is_final = state_is_new_final[i];
    }

    free(state_is_new_final);
    NFA_free(nfa2_clone);

    return new_nfa;
}

void NFA_rightquo_rec(NFA** nfa1, NFA* nfa2)
{
    NFA* nfa_rightquot = NFA_rightquo(*nfa1, nfa2);
    NFA_free(*nfa1);
    *nfa1 = nfa_rightquot;
}

void NFA_rightquo_unioned(NFA** nfa1, NFA* nfa2)
{
    NFA* nfa_rightquo = NFA_rightquo(*nfa1, nfa2);
    NFA_union_rec(nfa1, nfa_rightquo);
    NFA_free(nfa_rightquo);
}

NFA* NFA_swap(NFA* nfa, int n1, int n2)
{
    if (!nfa || n1 >= nfa->alphabet_dim || n1 < 0 || n2 >= nfa->alphabet_dim || n2 < 0) return nullptr;

    n1 = nfa->alphabet_dim - n1 - 1;
    n2 = nfa->alphabet_dim - n2 - 1;

    int  final_states_count = 0;
    int* final_states = NFA_get_final_states(nfa, &final_states_count);
    NFA* new_nfa = NFA_init(nfa->states_count, nfa->alphabet_dim, nfa->initial_state->id, final_states_count, final_states);
    free(final_states);

    for (size_t state_id = 0; state_id < nfa->states_count; state_id++)
    {
        for (int letter = 0; letter <= (1 << nfa->alphabet_dim); letter++)
        {
            if (letter == (1 << nfa->alphabet_dim)) {
                node* current = nfa->states[state_id]->transitions[1 << nfa->alphabet_dim]->head;
                while (current != nullptr) {
                    NFA_transition_add(new_nfa, state_id, current->val, 1 << nfa->alphabet_dim);
                    current = current->next;
                }
                continue;
            }

            int swapped_letter = letter;
            bool bit_n1 = (letter >> n1) & 1;
            bool bit_n2 = (letter >> n2) & 1;

            if (bit_n1 != bit_n2) {
                swapped_letter ^= (1 << n1) | (1 << n2);
            }

            node* current = nfa->states[state_id]->transitions[letter]->head;
            while (current != nullptr) {
                NFA_transition_add(new_nfa, state_id, current->val, swapped_letter);
                current = current->next;
            }
        }
    }


    return new_nfa;

}

void NFA_swap_rec(NFA** nfa, int n1, int n2)
{
    NFA* nfa_swap = NFA_swap(*nfa, n1, n2);
    NFA_free(*nfa);
    *nfa = nfa_swap;
}

NFA* NFA_to_DFA(NFA* nfa_original)
{
    if (!nfa_original) return nullptr;

    NFA* nfa = NFA_clone(nfa_original);

    NFA_remove_epsilon_transitions(nfa);

    NFA_remove_unreachable_states_rec(nfa);


    if (nfa->states_count == 0 || NFA_is_DFA(nfa))
    {
        return nfa;
    }

    // Each big_int in new_states represents a new state, which is a set of old states of given nfa
    // Least significant bit (from right) of big_int is state with id = 0, lefter - with id = 1, etc.
    big_int_list* new_states = big_int_list_init(1, (nfa->states_count + 7) >> 3);
    big_int* next_state_added = nullptr;
    // Table of transitions, where first id is state_id, and second - is letter. Stores destinations states ids
    int** transitions = (int**)malloc(sizeof(int*));
    int final_states_count = 0;
    int* final_states = (int*)malloc(sizeof(int));
    big_int* empty_state = big_int_init_zeroes((nfa->states_count + 7) >> 3); // for comparing

    // Adds initial state and all epsilon-initial states from current
    big_int_list_add_bit(new_states, 0, nfa->initial_state->id);
    node* eps_initial = nfa->initial_state->transitions[(1 << nfa->alphabet_dim)]->head;
    while (eps_initial)
    {
        big_int_list_add_bit(new_states, 0, eps_initial->val);
        eps_initial = eps_initial->next;
    }

    // Iterate until there will be no new states
    for (int new_state_id = 0; new_state_id < new_states->count; new_state_id++)
    {
        bool is_final = 0;
        // Adds new row to transitions table for currently processed new state
        transitions = (int**)realloc(transitions, new_states->count * sizeof(int*));
        transitions[new_state_id] = (int*)malloc(((1 << nfa->alphabet_dim) + 1) * sizeof(int));

        for (int letter = 0; letter < (1 << nfa->alphabet_dim); letter++)
        {
            transitions[new_state_id][letter] = -1;
            next_state_added = big_int_init_zeroes((nfa->states_count + 7) >> 3);

            for (int s = 0; s < nfa->states_count; s++)
            {
                // For each old state checks if it's in new_states[new_state_id]
                // and adds states, reached by the letter, to next_state_added
                if (big_int_list_get_bit(new_states, new_state_id, s) == 1) // may return -1, so need to check
                {
                    if (nfa->states[s]->is_final) is_final = 1;
                    node* dest = nfa->states[s]->transitions[letter]->head;
                    while (dest)
                    {
                        if (big_int_get_bit(next_state_added, dest->val) == 0) // may return -1, so need to check
                        {
                            big_int_add_bit(next_state_added, dest->val);

                            // Check all epsilon-reached states
                            queue* eps_queue = create_queue();
                            enqueue(eps_queue, dest->val);
                            while (!is_queue_empty(eps_queue))
                            {
                                node* eps_dest = nfa->states[dequeue(eps_queue)]->transitions[(1 << nfa->alphabet_dim)]->head;
                                while (eps_dest)
                                {
                                    if (!big_int_get_bit(next_state_added, eps_dest->val))
                                    {
                                        enqueue(eps_queue, eps_dest->val);
                                        big_int_add_bit(next_state_added, eps_dest->val);
                                    }
                                    eps_dest = eps_dest->next;
                                }
                            }
                            free_queue(eps_queue);
                        }
                        dest = dest->next;
                    }
                }
            }

            // Compares next_state_added and previosly added new states and adds record to transitions
            int state_id = -1;
            for (int id = 0; id < new_states->count; id++)
            {
                if (big_int_eq(next_state_added, new_states->big_ints[id]))
                {
                    transitions[new_state_id][letter] = id;
                    state_id = id;
                    break;
                }
            }

            // If it's new - adds it to new_states list
            if ((state_id == -1) && !big_int_eq(next_state_added, empty_state))
            {
                big_int_list_add_num(new_states, next_state_added);
                next_state_added = nullptr;
                transitions[new_state_id][letter] = new_states->count - 1;
            }

            big_int_free(next_state_added);
        }

        if (is_final)
        {
            final_states_count++;
            final_states = (int*)realloc(final_states, final_states_count * sizeof(int));
            final_states[final_states_count - 1] = new_state_id;
        }
    }

    NFA* dfa = NFA_init(new_states->count, nfa->alphabet_dim, 0, final_states_count, final_states);

    for (int state_id = 0; state_id < new_states->count; state_id++)
    {
        for (int letter = 0; letter < (1 << nfa->alphabet_dim); letter++)
        {
            if (transitions[state_id][letter] != -1)
                NFA_transition_add(dfa, state_id, transitions[state_id][letter], letter);
        }
        free(transitions[state_id]);
    }

    big_int_list_free(new_states);
    big_int_free(empty_state);
    free(transitions);
    free(final_states);

    NFA_remove_unreachable_states_rec(dfa);

    return dfa;
}

void NFA_to_DFA_rec(NFA** nfa)
{
    NFA* dfa = NFA_to_DFA(*nfa);
    NFA_free(*nfa);
    *nfa = dfa;
}

NFA* DFA_minimize(NFA* nfa_original)
{
    if (!nfa_original) return nullptr;

    NFA* nfa = NFA_clone(nfa_original);
    NFA_remove_unreachable_states_rec(nfa);

    if (!NFA_is_DFA(nfa))
    {
        NFA_to_DFA_rec(&nfa);
    }

    int groups_count = 2;
    int* state_group = (int*)calloc(nfa->states_count, sizeof(int)); // represents group number of each state
    list** groups = (list**)malloc(2 * sizeof(list*)); // list of groups, where each group is a set of states {s1,s2,..}
    // initialize group0 - final states and group1 - non-final states
    for (int i = 0; i < 2; i++)
    {
        groups[i] = list_init();
    }

    for (int i = 0; i < nfa->states_count; i++)
    {
        //if final = 1 -> add to groups[0], if final = 0 => groups[1]
        add_to_list(groups[nfa->states[i]->is_final == 0], i);
        state_group[i] = nfa->states[i]->is_final == 0;
    }

    bool need_check = 1; // =1 if groups changed during the previous iteration
    while (need_check)
    {
        need_check = 0;
        for (int i = 0; i < groups_count; i++)
        {
            int old_groups_count = groups_count;

            list** new_groups = divide_into_groups(nfa, groups[i], &state_group, &groups_count);
            if (new_groups)
            {
                list_free(groups[i]);
                groups[i] = new_groups[0];
                if (old_groups_count != groups_count)
                {
                    need_check = 1; // groups changed, so we need to check all of them again
                    groups = (list**)realloc(groups, groups_count * sizeof(list*));
                    for (int j = 1; j < (groups_count - old_groups_count + 1); j++)
                    {
                        groups[old_groups_count + j - 1] = new_groups[j];
                    }
                }

                free(new_groups);
            }
        }
    }

    int final_states_count = 0;
    int* final_states = nullptr;
    for (int i = 0; i < groups_count; i++)
    {
        node* current = groups[i]->head;
        while (current)
        {
            if (nfa->states[current->val]->is_final)
            {
                final_states_count++;
                final_states = (int*)realloc(final_states, final_states_count * sizeof(int));
                final_states[final_states_count - 1] = i;
                break;
            }

            current = current->next;
        }
    }

    // now each group is a new state in new nfa
    NFA* new_nfa = NFA_init(groups_count, nfa->alphabet_dim, state_group[nfa->initial_state->id], final_states_count, final_states);
    for (int i = 0; i < groups_count; i++)
    {
        node* current = groups[i]->head;
        while (current)
        {
            for (int letter = 0; letter < (1 << nfa->alphabet_dim); letter++)
            {
                node* destination = nfa->states[current->val]->transitions[letter]->head;
                if (destination) NFA_transition_add(new_nfa, i, state_group[destination->val], letter);
            }
            current = current->next;
        }
        list_free(groups[i]);
    }
    free(groups);
    free(final_states);
    free(state_group);

    NFA_remove_unreachable_states_rec(new_nfa);

    // If there is a state which have no transition from it (besides itself) and it's non-final or initial,
    // then we can remove it

    bool need_remove = true;

    while (need_remove)
    {
        need_remove = false;
        int* removed_states = (int*)calloc(new_nfa->states_count, sizeof(int));

        for (int i = 0; i < new_nfa->states_count; i++)
        {
            if (!new_nfa->states[i]->is_final && new_nfa->states[i]->id != new_nfa->initial_state->id)
            {
                bool have_transitions = 0;
                for (int letter = 0; letter <= (1 << new_nfa->alphabet_dim); letter++)
                {
                    node* head = new_nfa->states[i]->transitions[letter]->head;
                    if (head && !(head->val == i))
                    {
                        have_transitions = 1;
                        break;
                    }
                }

                if (!have_transitions)
                {
                    removed_states[i] = 1;
                    need_remove = true;
                }
            }
        }

        if (need_remove) NFA_state_list_remove(new_nfa, removed_states, new_nfa->states_count);
        free(removed_states);
    }

    if (new_nfa->states_count == 1 && !new_nfa->initial_state->is_final) // its initial state
    {
        for (int letter = 0; letter <= (1 << new_nfa->alphabet_dim); letter++)
        {
            free(new_nfa->initial_state->transitions[letter]->head);
            new_nfa->initial_state->transitions[letter]->head = nullptr;
        }
    }

    return new_nfa;
}

void DFA_minimize_rec(NFA** nfa)
{
    NFA* new_nfa = DFA_minimize(*nfa);
    NFA_free(*nfa);
    *nfa = new_nfa;
}

#pragma endregion


#pragma region NFA Support Functions

NFA* DFA_make_complete(NFA* nfa_original)
{
    if (!nfa_original) return nullptr;

    NFA* nfa = NFA_clone(nfa_original);

    if (!NFA_is_DFA(nfa))
    {
        NFA_to_DFA_rec(&nfa);
    }

    int states_count = nfa->states_count;
    NFA_state_add(nfa, false);

    for (int i = 0; i < states_count; i++)
    {
        for (int letter = 0; letter < (1 << nfa->alphabet_dim); letter++)
        {
            if (!nfa->states[i]->transitions[letter]->head)
            {
                NFA_transition_add(nfa, i, states_count, letter);
            }
        }
    }

    for (int letter = 0; letter < (1 << nfa->alphabet_dim); letter++)
    {
        NFA_transition_add(nfa, states_count, states_count, letter);
    }

    return nfa;
}

void DFA_make_complete_rec(NFA** nfa)
{
    NFA* new_nfa = DFA_make_complete(*nfa);
    NFA_free(*nfa);
    *nfa = new_nfa;
}

list** divide_into_groups(NFA* nfa, list* group, int** state_group, int* groups_count)
{
    if (!nfa || !group || !group->head || !state_group) return nullptr;

    // divide group into current_group - all states that are equiv to state0, and new_group - others
    list** groups;
    list* current_group = list_init();;
    list* new_group = list_init();
    node* current = group->head->next;

    add_to_list(current_group, group->head->val);

    while (current)
    {
        bool group_was_changed = 0;
        for (int letter = 0; letter < (1 << nfa->alphabet_dim); letter++)
        {
            // as we have dfa, then we need to compare only heads of the transition lists
            node* dest_state1 = nfa->states[group->head->val]->transitions[letter]->head;
            node* dest_state2 = nfa->states[current->val]->transitions[letter]->head;

            // if both != null AND (only one of them = null OR groups are not equal)
            if (!(dest_state1 == NULL && dest_state2 == NULL) &&
                (((dest_state1 == NULL) != (dest_state2 == NULL)) || ((*state_group)[dest_state1->val] != (*state_group)[dest_state2->val])))
            {
                add_to_list(new_group, current->val);
                //(*state_group)[current->val] = *groups_count;
                group_was_changed = 1;
                break;
            }
        }

        if (!group_was_changed) add_to_list(current_group, current->val);

        current = current->next;
    }

    current = new_group->head;
    while (current)
    {
        (*state_group)[current->val] = *groups_count;
        current = current->next;
    }

    groups = (list**)malloc(1 * sizeof(list*));
    groups[0] = current_group;

    if (new_group->head)
    {
        int old_groups_count = *groups_count;
        (*groups_count)++;
        list** new_groups = divide_into_groups(nfa, new_group, state_group, groups_count);
        groups = (list**)realloc(groups, sizeof(list*) * ((*groups_count) - old_groups_count + 1));
        for (int i = 1; i < (*groups_count) - old_groups_count + 1; i++)
        {
            //groups[i] = (list*)malloc(sizeof(list)); //MAYBE SHOULD BE REMOVED
            groups[i] = new_groups[i - 1];
        }
        free(new_groups);
    }

    current_group = nullptr;
    list_free(new_group);
    return groups;
}

stack* NFA_get_reachable_states(NFA* nfa, int start_state, char letter)
{
    if (!nfa || start_state < 0 || start_state >= nfa->states_count || letter >(1 << nfa->alphabet_dim)) return nullptr;
    stack* reached_states = create_stack();
    bool* reached_states_added = (bool*)calloc(nfa->states_count, sizeof(bool));
    node* dest = nfa->states[start_state]->transitions[letter]->head;
    while (dest)
    {
        if (!reached_states_added[dest->val]) // if destination_state is not in the next_states
        {
            push(reached_states, dest->val); // push it into next_states
            reached_states_added[dest->val] = 1;

            // we also check all states that are epsilon-reached from this destination_state
            // and push them in next_states
            queue* eps_queue = create_queue();
            enqueue(eps_queue, dest->val);
            while (!is_queue_empty(eps_queue))
            {
                node* eps_dest = nfa->states[dequeue(eps_queue)]->transitions[(1 << nfa->alphabet_dim)]->head;
                while (eps_dest)
                {
                    if (!reached_states_added[eps_dest->val])
                    {
                        enqueue(eps_queue, eps_dest->val);
                        push(reached_states, eps_dest->val);
                        reached_states_added[eps_dest->val] = 1;
                    }
                    eps_dest = eps_dest->next;
                }
            }
            free_queue(eps_queue);
        }
        dest = dest->next;
    }

    free(reached_states_added);
    return reached_states;
}

NFA* NFA_remove_unreachable_states(NFA* nfa)
{
    NFA* nfa_result = NFA_clone(nfa);
    NFA_remove_unreachable_states_rec(nfa_result);
    return nfa_result;
}

void NFA_remove_unreachable_states_rec(NFA* nfa)
{
    int* state_is_reachable = (int*)calloc(nfa->states_count, sizeof(int));
    queue* states_queue = create_queue(); // quueueueu of states which were reached from the initial
    enqueue(states_queue, nfa->initial_state->id);
    state_is_reachable[nfa->initial_state->id] = 1;

    while (!is_queue_empty(states_queue))
    {
        int state = dequeue(states_queue);
        for (int letter = 0; letter <= (1 << nfa->alphabet_dim); letter++)
        {
            node* dest_state = nfa->states[state]->transitions[letter]->head;
            while (dest_state)
            {
                if (!state_is_reachable[dest_state->val])
                {
                    state_is_reachable[dest_state->val] = 1;
                    enqueue(states_queue, dest_state->val);
                }
                dest_state = dest_state->next;
            }
        }
    }


    for (int i = 0; i < nfa->states_count; i++)
    {
        state_is_reachable[i] = (state_is_reachable[i] + 1) & 1; // changes 0 -> 1 and 1 -> 0
    }

    NFA_state_list_remove(nfa, state_is_reachable, nfa->states_count);

    free_queue(states_queue);
    free(state_is_reachable);
}

bool NFA_is_empty(NFA* nfa)
{
    if (!nfa) return true;

    for (int i = 0; i < nfa->states_count; i++)
    {
        for (int j = 0; j <= (1 << nfa->alphabet_dim); j++)
        {
            if (nfa->states[i]->transitions[j]->head) return false;
        }
    }

    return true;
}

int NFA_get_final_states_count(NFA* nfa)
{
    int count = 0;
    for (int i = 0; i < nfa->states_count; i++)
    {
        if (nfa->states[i]->is_final) count++;
    }
    return count;
}

int* NFA_get_final_states(NFA* nfa, int* states_count)
{
    int count = 0;
    int* final_states = nullptr;

    for (int i = 0; i < nfa->states_count; i++)
    {
        if (nfa->states[i]->is_final)
        {
            count++;
            final_states = (int*)realloc(final_states, sizeof(int) * count);
            final_states[count - 1] = nfa->states[i]->id;
        }
    }

    *states_count = count;
    return final_states;
}

bool NFA_is_DFA(NFA* nfa)
{
    if (!nfa) return false;

    for (size_t i = 0; i < nfa->states_count; i++) {
        NFA_state* state = nfa->states[i];

        for (size_t letter = 0; letter < (1 << nfa->alphabet_dim); letter++) {
            list* transition_list = state->transitions[letter];

            if (!transition_list || !transition_list->head) continue;

            size_t transitions_count = 0;
            for (node* trans_node = transition_list->head; trans_node != nullptr; trans_node = trans_node->next) {
                transitions_count++;
                if (transitions_count > 1) return false;
            }

        }

        if (state->transitions[1 << nfa->alphabet_dim]->head != nullptr) {
            return false;
        }
    }

    return true;
}

void copy_transitions(NFA* nfa, int from_state, int to_state) {
    for (int letter = 0; letter < (1 << nfa->alphabet_dim); letter++) {
        node* current = nfa->states[to_state]->transitions[letter]->head;
        while (current) {
            NFA_transition_add(nfa, from_state, current->val, letter);
            current = current->next;
        }
    }
}

void find_epsilon_closure(NFA* nfa, int state_id, bool* epsilon_closure) {
    epsilon_closure[state_id] = true;

    node* current = nfa->states[state_id]->transitions[1 << nfa->alphabet_dim]->head;
    while (current) {
        if (!epsilon_closure[current->val]) {
            find_epsilon_closure(nfa, current->val, epsilon_closure);
        }
        current = current->next;
    }
}

void NFA_remove_epsilon_transitions(NFA* nfa) {
    if (nfa == nullptr) return;

    int epsilon = 1 << nfa->alphabet_dim;

    for (size_t i = 0; i < nfa->states_count; i++) {
        NFA_state* state = nfa->states[i];

        bool* epsilon_closure = (bool*)calloc(nfa->states_count, sizeof(bool));
        find_epsilon_closure(nfa, i, epsilon_closure);

        for (size_t j = 0; j < nfa->states_count; j++) {
            if (epsilon_closure[j]) {
                copy_transitions(nfa, i, j);
                if (nfa->states[j]->is_final) {
                    nfa->states[i]->is_final = true;
                }
            }
        }

        list_free(state->transitions[epsilon]);
        state->transitions[epsilon] = list_init();

        free(epsilon_closure);
    }
}

#pragma endregion


#pragma region NFA Examples

NFA* NFA_get_div_power_of_2_custom(int power)
{
    if (power < 1) return nullptr;

    int states_count = power + 1;

    int* final_states = (int*)malloc(sizeof(int));
    final_states[0] = power;

    NFA* nfa = NFA_init(states_count, 1, 0, 1, final_states);

    free(final_states);

    for (int i = 0; i < states_count; i++) {
        if (i < power) {
            NFA_transition_add(nfa, i, i + 1, 0);
        }
        else {
            NFA_transition_add(nfa, i, i, 0);
            NFA_transition_add(nfa, i, i, 1);
        }

    }

    return nfa;
}

NFA* NFA_get_div_2()
{
    NFA* nfa = NFA_get_equal();
    NFA* sum = NFA_get_sum();
    NFA_extend_rec(&nfa, nfa->alphabet_dim);
    NFA_intersect_rec(&nfa, sum);
    NFA_project_rec(&nfa, 0);
    NFA_project_rec(&nfa, 0);
    NFA_to_DFA_rec(&nfa);
    DFA_minimize_rec(&nfa);
    NFA_free(sum);
    return nfa;
}

NFA* NFA_get_div_3()
{
    NFA* nfa = NFA_get_equal(); // x = y
    NFA_extend_rec(&nfa, nfa->alphabet_dim); //extend by z
    NFA_extend_rec(&nfa, nfa->alphabet_dim); //extend by M
    NFA* sum = NFA_get_sum(); // x + y = z
    NFA_extend_rec(&sum, sum->alphabet_dim); // extend by M
    NFA_intersect_rec(&nfa, sum); //x = y && x + y = z

    NFA_project_rec(&sum, sum->alphabet_dim - 1); // project by M
    NFA_extend_rec(&sum, 1); // extend by y
    NFA_intersect_rec(&nfa, sum); // x = y && x + y = z && x + z = M

    NFA_project_rec(&nfa, 0); // exist x
    NFA_project_rec(&nfa, 0); // exist y
    NFA_project_rec(&nfa, 0); // exist z

    NFA_to_DFA_rec(&nfa);
    DFA_minimize_rec(&nfa);
    NFA_free(sum);
    return nfa;
}

NFA* NFA_get_equal_num(int num)
{
    int* final_states = (int*)malloc(sizeof(int));
    NFA* nfa;
    if (!num)
    {
        final_states[0] = 1;
        nfa = NFA_init(2, 1, 0, 1, final_states);
        NFA_transition_add(nfa, 0, 1, 0);
    }
    else
    {
        int states_count = (int)floor(log2(num)) + 2;
        final_states[0] = states_count - 1;
        nfa = NFA_init(states_count, 1, 0, 1, final_states);

        for (int i = 0; i < states_count - 1; i++)
        {
            unsigned char letter = num & 1;
            num >>= 1;
            NFA_transition_add(nfa, i, i + 1, letter);
        }
    }

    // ДОБАИВТЬ НОЛИКИ
    NFA_transition_add(nfa, nfa->states_count - 1, nfa->states_count - 1, 0);

    return nfa;
}

NFA* NFA_get_2_power_m_y(int power)
{
    NFA* nfa_equal = NFA_get_equal();
    NFA* nfa_sum = NFA_get_sum();
    NFA* nfa_result = NFA_get_equal(); // [y, y]
    NFA_extend_rec(&nfa_equal, 2); // [y, y, -]
    NFA_extend_rec(&nfa_sum, 3); // [x, y, z, -]

    for (int i = 0; i < power; i++)
    {
        // NFA result = [2^i * y, y]
        NFA_extend_rec(&nfa_result, 0); // [-, 2^i * y, y]
        NFA_intersect_rec(&nfa_result, nfa_equal); // [2^i * y, 2^i * y, y]
        NFA_extend_rec(&nfa_result, 2); // [2^i * y, 2^i * y, -, y]
        NFA_intersect_rec(&nfa_result, nfa_sum); // [2^i * y, 2^i * y, 2^i+1 * y, y]
        NFA_project_rec(&nfa_result, 0); // [2^i * y, 2^i+1 * y, y]
        NFA_project_rec(&nfa_result, 0); // [2^i+1 * y, y]
        NFA_to_DFA_rec(&nfa_result);
        DFA_minimize_rec(&nfa_result);
    }

    NFA_free(nfa_sum);
    NFA_free(nfa_equal);

    return nfa_result;
}

NFA* NFA_get_div_2_power_m(int power)
{
    NFA* nfa_result = NFA_get_2_power_m_y(power);
    NFA_project_rec(&nfa_result, 1);
    DFA_minimize_rec(&nfa_result);
    return nfa_result;
}

NFA* NFA_get_sum()
{
    int* final_states = (int*)malloc(sizeof(int));
    final_states[0] = 0;
    NFA* nfa = NFA_init(2, 3, 0, 1, final_states);

    NFA_transition_add(nfa, 0, 0, 0);
    NFA_transition_add(nfa, 0, 0, 3);
    NFA_transition_add(nfa, 0, 0, 5);
    NFA_transition_add(nfa, 0, 1, 6);
    NFA_transition_add(nfa, 1, 1, 2);
    NFA_transition_add(nfa, 1, 1, 4);
    NFA_transition_add(nfa, 1, 1, 7);
    NFA_transition_add(nfa, 1, 0, 1);

    return nfa;
}

NFA* NFA_get_equal()
{
    int* final_states = (int*)malloc(sizeof(int));
    final_states[0] = 0;
    NFA* nfa = NFA_init(2, 2, 0, 1, final_states);
    free(final_states);

    NFA_transition_add(nfa, 0, 0, 0);
    NFA_transition_add(nfa, 0, 0, 3);
    NFA_transition_add(nfa, 0, 1, 1);
    NFA_transition_add(nfa, 0, 1, 2);

    return nfa;
}

NFA* NFA_get_random()
{
    int states_count = get_random_num(5, 6);
    //int states_count = get_random_num(2, 6);
    int alphabet_dim = 3;
    //int alphabet_dim = get_random_num(1,3);
    int initial_state = 0;
    int final_states_count = get_random_num(1, states_count);
    int* final_states = (int*)calloc(final_states_count, sizeof(int));
    for (int i = 0; i < final_states_count; i++)
    {
        final_states[i] = get_random_num(0, states_count);
    }

    NFA* nfa = NFA_init(states_count, alphabet_dim, initial_state, final_states_count, final_states);

    NFA_transition_add(nfa, initial_state, get_random_num(1, states_count - 1), get_random_num(0, (1 << alphabet_dim)));

    for (int i = 0; i < states_count; i++)
    {
        for (int j = 0; j < states_count; j++)
        {
            // with probability of 30% there will be a transition btw states[i] and states[j] by letter
            int transition_chance = get_random_num(1, 10);
            if (transition_chance <= 3)
            {
                int transitions_count = get_random_num(1, 3);
                for (int tr = 0; tr < transitions_count; tr++)
                {
                    NFA_transition_add(nfa, i, j, get_random_num(0, (1 << alphabet_dim)));
                }
            }
        }
    }

    free(final_states);
    return nfa;
}

NFA* NFA_get_automaton_1()
{
    int final_states[] = { 7 };
    int* final_states_list = &(final_states[0]);
    NFA* nfa = NFA_init(8, 1, 0, 1, final_states_list);

    NFA_transition_add(nfa, 0, 1, 2);
    NFA_transition_add(nfa, 0, 2, 1);
    NFA_transition_add(nfa, 1, 5, 0);
    NFA_transition_add(nfa, 1, 4, 0);
    NFA_transition_add(nfa, 1, 4, 1);
    NFA_transition_add(nfa, 2, 2, 0);
    NFA_transition_add(nfa, 2, 2, 1);
    NFA_transition_add(nfa, 2, 3, 0);
    NFA_transition_add(nfa, 3, 4, 2);
    NFA_transition_add(nfa, 3, 7, 2);
    NFA_transition_add(nfa, 4, 5, 2);
    NFA_transition_add(nfa, 4, 6, 0);
    NFA_transition_add(nfa, 5, 5, 0);
    NFA_transition_add(nfa, 5, 5, 1);
    NFA_transition_add(nfa, 5, 6, 1);
    NFA_transition_add(nfa, 6, 4, 1);
    NFA_transition_add(nfa, 6, 7, 0);

    return nfa;
}

NFA* NFA_get_only_zeroes()
{
    int states_count = 1;
    int alphabet_dim = 1;
    int initial_state = 0;
    int final_states_count = 1;
    int final_states[] = { 0 };

    NFA* nfa = NFA_init(states_count, alphabet_dim, initial_state, final_states_count, final_states);

    NFA_transition_add(nfa, 0, 0, 0);
    return nfa;
}

NFA* NFA_get_only_zeroes(int dim)
{
    int states_count = 1;
    int alphabet_dim = dim;
    int initial_state = 0;
    int final_states_count = 1;
    int final_states[] = { 0 };

    NFA* nfa = NFA_init(states_count, alphabet_dim, initial_state, final_states_count, final_states);

    NFA_transition_add(nfa, 0, 0, 0);
    return nfa;
}

NFA* NFA_get_trivial()
{
    int states_count = 1;
    int alphabet_dim = 1;
    int initial_state = 0;
    int final_states_count = 1;
    int final_states[] = { 0 };

    NFA* nfa = NFA_init(states_count, alphabet_dim, initial_state, final_states_count, final_states);

    NFA_transition_add(nfa, 0, 0, 0);
    NFA_transition_add(nfa, 0, 0, 1);

    return nfa;
}

NFA* NFA_get_equal_coordinates(NFA* nfa, int n1, int n2)
{
    if (!nfa || n1 >= nfa->alphabet_dim || n2 >= nfa->alphabet_dim || n1 < 0 || n2 < 0) return nullptr;
    if (n1 == n2) return NFA_clone(nfa);

    NFA* nfa_equal = NFA_get_equal();
    NFA* nfa_result = NFA_clone(nfa);
    int dim_difference = nfa->alphabet_dim - nfa_equal->alphabet_dim;
    for (int i = 0; i < dim_difference; i++)
    {
        NFA_extend_rec(&nfa_equal, nfa_equal->alphabet_dim);
    }

    if (n1 + n2 > 1)
    {
        NFA_swap_rec(&nfa_result, 0, n1);
        NFA_swap_rec(&nfa_result, 1, n2);
    }

    NFA_intersect_rec(&nfa_result, nfa_equal);
    DFA_minimize_rec(&nfa_result);

    if (n1 + n2 > 1)
    {
        NFA_swap_rec(&nfa_result, 1, n2);
        NFA_swap_rec(&nfa_result, 0, n1);
    }

    NFA_free(nfa_equal);
    return nfa_result;
}

void NFA_get_equal_coordinates_rec(NFA** nfa, int n1, int n2)
{
    NFA* new_nfa = NFA_get_equal_coordinates(*nfa, n1 ,n2);
    NFA_free(*nfa);
    *nfa = new_nfa;
}

NFA* NFA_get_a_y(int num)
{
    if (!num)  //num = 0
    {
        return NFA_get_equal_num(0);
    }
    num = abs(num);

    NFA* nfa_result = NFA_get_equal(); // [y, y]
    NFA* nfa_sum = NFA_get_sum(); // [x, y, z]
    NFA_swap_rec(&nfa_sum, 0, 2); // [z, y, x]
    NFA_extend_rec(&nfa_sum, 2); // [z, y, -, x]
    NFA_extend_rec(&nfa_sum, 4); // [z, y, -, x, -]

    num--;
    int power = 0;

    while (num)
    {
        // NFA_result: [12y, y]
        unsigned char bit = num & 1;
        num >>= 1;
        if (bit)
        {
            NFA* nfa = NFA_get_2_power_m_y(power); // [4z, z]
            NFA_extend_rec(&nfa_result, nfa_result->alphabet_dim); // [12y, y, -]
            NFA_extend_rec(&nfa_result, nfa_result->alphabet_dim); // [12y, y, -, -]
            NFA_extend_rec(&nfa, 0); // [-, 4z, z]
            NFA_extend_rec(&nfa, 0); // [-, -, 4z, z]
            NFA_intersect_rec(&nfa_result, nfa); //[12y, y, 4z, z]

            NFA_get_equal_coordinates_rec(&nfa_result, 1, 3); //[12y, y, 4y, y]
            NFA_extend_rec(&nfa_result, 0); //[-, 12y, y, 4y, y]
            NFA_intersect_rec(&nfa_result, nfa_sum); //[14y, 12y, y, 2y, y]

            NFA_project_rec(&nfa_result, 1); //[14y, y, 2y, y]
            NFA_project_rec(&nfa_result, 2); //[14y, y, y]
            NFA_project_rec(&nfa_result, 2); //[14y, y]

            NFA_free(nfa);
        }

        power++;
    }

    DFA_minimize_rec(&nfa_result);
    NFA_free(nfa_sum);

    return nfa_result;
}

NFA* NFA_get_div_a(int a)
{
    NFA* nfa_result = NFA_get_a_y(a);
    NFA_project_rec(&nfa_result, 1);
    DFA_minimize_rec(&nfa_result);
    return nfa_result;
}

NFA* NFA_get_linear_term(int* a, int count)
{
    if (!a) return nullptr;
    if (count == 0) return NFA_get_only_zeroes();

    NFA* nfa_sum = NFA_get_sum();
    NFA* nfa_result = NFA_get_only_zeroes();

    NFA_swap_rec(&nfa_sum, 0, 2); // [z, y, x]

    for (int i = count - 1; i > 0; i--)
    {
        // y = anxn
        // NFA_result: [y, xn]
        NFA_extend_rec(&nfa_sum, nfa_sum->alphabet_dim); // [z, y, x, -, -]

        NFA* nfa_a = NFA_get_a_y(a[i]); // [ai*xi, xi], y = a[i] * xi
        NFA_extend_rec(&nfa_a, 0); // [-, ai*xi, xi]
        NFA_extend_rec(&nfa_a, 0); // [-, -, ai*xi, xi]
        for (int j = 0; j < count - 1 - i; j++)
        {
            NFA_extend_rec(&nfa_a, nfa_a->alphabet_dim); // [-, -, ai*xi, xi, -]
        }

        NFA_extend_rec(&nfa_result, 0); // [-, y, xn]
        NFA_extend_rec(&nfa_result, 2); // [-, y, -, xn]
        NFA_extend_rec(&nfa_result, 3); // [-, y, -, -, xn]
        NFA_intersect_rec(&nfa_result, nfa_a); // [-, y, ai*xi, xi, xn]
        DFA_minimize_rec(&nfa_result);
        NFA_intersect_rec(&nfa_result, nfa_sum); // [new_y, y, ai*xi, xi, xn]
        //DFA_minimize_rec(&nfa_result);
        NFA_project_rec(&nfa_result, 1);
        NFA_project_rec(&nfa_result, 1); // [new_y, xi, xn]
        DFA_minimize_rec(&nfa_result);

        NFA_free(nfa_a);
    }

    NFA_extend_rec(&nfa_result, 0);
    NFA_extend_rec(&nfa_result, 0); // [-, -, y, x1, x2, ..., xn-1]
    NFA* nfa_a0 = NFA_get_equal_num(a[0]); // [a0]
    NFA_extend_rec(&nfa_a0, 0); // [-, a0]
    NFA_extend_rec(&nfa_a0, 2); // [-, a0, -]

    for (int i = 0; i < count - 1; i++)
    {
        NFA_extend_rec(&nfa_a0, nfa_a0->alphabet_dim); // [-, a0, -, -, ...]
    }

    NFA_intersect_rec(&nfa_result, nfa_a0); // [-, a0, y, x1, x2, ..., xn-1]
    NFA_intersect_rec(&nfa_result, nfa_sum); // [new_y, a0, y, x1, x2, ..., xn-1]
    NFA_project_rec(&nfa_result, 1);
    NFA_project_rec(&nfa_result, 1);
    DFA_minimize_rec(&nfa_result); // [new_y, x1, x2, ..., xn-1]

    NFA_free(nfa_sum);
    NFA_free(nfa_a0);

    return nfa_result;
}

NFA* NFA_with_term(NFA* nfa, NFA* term)
{
    if (nfa->alphabet_dim != 1 || !nfa || !term) return nullptr;
    NFA* nfa_copy = NFA_clone(nfa);

    while (nfa_copy->alphabet_dim != term->alphabet_dim)
    {
        NFA_extend_rec(&nfa_copy, nfa_copy->alphabet_dim);
    }


    NFA_intersect_rec(&nfa_copy, term);
    DFA_minimize_rec(&nfa_copy);

    NFA_project_rec(&nfa_copy, 0);


    NFA* zeroes = NFA_get_only_zeroes(nfa_copy->alphabet_dim);

    NFA_rightquo_rec(&nfa_copy, zeroes);
    NFA_free(zeroes);

    DFA_minimize_rec(&nfa_copy);

    return nfa_copy;
}

NFA* NFA_from_linear_expression(linear_expression* expr)
{
    if (!expr) return nullptr;

    int* coefficients = (int*)malloc(sizeof(int) * (expr->terms_count + 1));

    coefficients[0] = expr->constant;
    for (int i = 1; i < expr->terms_count + 1; i++){
        coefficients[i] = expr->terms[i - 1].coefficient;
    }

    NFA* result = NFA_get_linear_term(coefficients, expr->terms_count + 1);
    free(coefficients);

    return result;
}

#pragma endregion


#pragma region Handful Functions For Console App

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
        }
        else if (strcmp(input, "help") == 0) {
            print_help();
        }
        else if (strcmp(input, "nfa_list") == 0) {
            NFA_list();
        } else if (strncmp(input, "def ", 4) == 0) {
            NFA_def(input);
        } else if (strncmp(input, "eval ", 5) == 0) {
            NFA_eval_command(input);
        } else if (strncmp(input, "for  ", 4) == 0) {
            NFA_for_command(input);
        } else if (strncmp(input, "eval2 ", 6) == 0) {
            NFA_eval2_command(input);
        } else if (strncmp(input, "visualize ", 10) == 0) {
            NFA_visualize_command(input);
        } else if (strncmp(input, "minimize $", 10) == 0) {
            handle_minimization(input + 10);
        } else if (strncmp(input, "to_dfa $", 8) == 0) {
            handle_conversion_to_dfa(input + 8);
        } else if (strncmp(input, "remove_eps $", 12) == 0) {
            handle_remove_epsilon(input + 12);
        } else if (strncmp(input, "cls", 3) == 0) {
            handle_cls();
        } else {
                printf("Unknown command: %s\n", input);
        }
    }

}

void print_help() {
    cout << "Available commands:\n";
    cout << "exit - Exit the NFA Console Application.\n";
    cout << "help - Display this help message.\n";
    cout << "cls - Clear console\n";
    cout << "nfa_list - List available automata.\n";
    cout << "def <name> \"<predicate>\" [-m] [-v] [-re] - Define a new NFA from a logical predicate and save it. Supports union(|), intersection(&), complement(~), right quotient(/), and left quotient(\\).\n";
    cout << "eval $automaton_name(num1, num2, ..., numN) - Evaluate an automaton with a given numbers.\n";
    cout << "eval2 $automaton_name(binary_num1, binary_num2, ..., binary_numN) - Evaluate an automaton with a binary numbers.\n";
    cout << "for $automaton_name(start, end, [step]) - Test an automaton with a range of decimal numbers.\n";
    cout << "visualize $automaton_name - Visualize an NFA as a graphical representation.\n";
    cout << "minimize $automaton_name - Minimize.\n";
    cout << "to_dfa $automaton_name - Convert an NFA to DFA.\n";
    cout << "remove_eps $automaton_name - Remove epsilon transitions from an NFA.\n";
}

void NFA_list() {
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

char* NFA_RPN(const char* formula) {
    stack* operators = create_stack();
    int length = strlen(formula);
    char* rpn = (char*)malloc(length + 1);
    char* exist_forall = (char*)malloc(length + 1);
    int j = 0, k = 0;

    for (const char *c = formula; *c; c++) {
        switch (*c) {
        case '(' :
            push(operators, *c  );
            break;
        case 'E' :
        case 'A' :
            while (*c != ':' && *c != '\0'){
                exist_forall[k++] = *c;
                c++;
            }
            if (*c == '\0') {
                cout << "Error: quotes not found\n";
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
                c--;
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
    strcat(result, exist_forall);

    free(rpn);
    free(exist_forall);
    free_stack(operators);
    return result;
}

NFA* NFA_from_predicate(const char* predicate) {
    char* rpn = NFA_RPN(predicate);

    remove_spaces(rpn);

    nfa_stack* nfa_stack = create_nfa_stack();

    char* token = strtok(rpn, " ");
    while (token) {
        if (token[0] == '$') {
            char* name = extract_name(token);
            NFA* nfa = load_NFA_from_file(name);

            if (*(token + strlen(name) + 1) == '(') {
                char* term_start = token + strlen(name) + 2; // Move pointer past the character '('
                char* term_end = strchr(term_start, ')');
                if (term_end) {
                    *term_end = '\0'; // Replace ')' with '\0'
                    linear_expression* expr = parse_linear_expression(term_start);
                    NFA* term_nfa = NFA_from_linear_expression(expr);
                    free_linear_expression(expr);

                    NFA* new_nfa = NFA_with_term(nfa, term_nfa);
                    NFA_free(nfa);
                    NFA_free(term_nfa);
                    nfa = new_nfa;
                }
            }

            push(nfa_stack, nfa);
            free(name);
        } else {
            handle_operation(nfa_stack, token[0]);
        }
        token = strtok(nullptr, " ");
    }

    NFA* final_nfa = pop(nfa_stack);
    free(rpn);
    free_stack(nfa_stack);
    return final_nfa;
}

void NFA_def(const char* command) {
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

void NFA_eval_command(const char* command)
{
    char automaton_name[256];
    char number_string[1024];
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
    } else  {
        cout << "Invalid command format. Use eval \"$automaton_name(num1, num2, ... numN)\"" << endl;
    }
}

void NFA_for_command(const char* command) {
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

void NFA_eval2_command(const char* command)
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

void NFA_visualize_command(const char* command)
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

void handle_minimization(const char* automaton_name) {
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

void handle_conversion_to_dfa(const char* automaton_name) {
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

void handle_operation(nfa_stack* stack, char op) {
    switch (op)
    {
    case '&': {
        NFA *nfa2 = pop(stack);
        NFA *nfa1 = pop(stack);
        NFA_intersect_rec(&nfa1, nfa2);
        push(stack, nfa1);
        NFA_free(nfa2);
        break;
    }
    case '|': {
        NFA *nfa2 = pop(stack);
        NFA *nfa1 = pop(stack);
        NFA_union_rec(&nfa1, nfa2);
        push(stack, nfa1);
        NFA_free(nfa2);
        break;
    }
    case '~': {
        NFA *nfa = pop(stack);
        DFA_complement_rec(&nfa);
        push(stack, nfa);
        break;
    }
    case '/': {
        // Right quotient
        NFA *nfa2 = pop(stack);
        NFA *nfa1 = pop(stack);
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
        NFA *result = NFA_leftquo(nfa1, nfa2);
        push(stack, result);
        NFA_free(nfa1);
        NFA_free(nfa2);
        break;
    }
    case 'E': { // So far, the projection is only based on the first variable
        NFA* nfa = pop(stack);
        NFA_project_rec(&nfa, 0);
        NFA* only_zeros = NFA_get_only_zeroes(nfa->alphabet_dim);
        NFA_rightquo_rec(&nfa, only_zeros);
        NFA_free(only_zeros);
        DFA_minimize_rec(&nfa);
        push(stack, nfa);
        break;
    }
    case 'A': {
        NFA* nfa = pop(stack);
        DFA_complement_rec(&nfa);
        NFA_project_rec(&nfa, 0);
        NFA* only_zeros = NFA_get_only_zeroes(nfa->alphabet_dim);
        NFA_rightquo_rec(&nfa, only_zeros);
        NFA_free(only_zeros);
        DFA_complement_rec(&nfa);
        DFA_minimize_rec(&nfa);
        push(stack, nfa);
        break;
    }}
}

void handle_remove_epsilon(const char* automaton_name) {
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

void handle_cls()
{
    system("cls");
    cout << "Enter command (type 'exit' to quit):\n";
}

void remove_spaces(char* str) {
    char* dst = str;
    bool within_parentheses = false;

    while (*str != '\0') {
        if (*str == '(') {
            within_parentheses = true;
            *dst++ = *str;
        } else if (*str == ')') {
            within_parentheses = false;
            *dst++ = *str;
        } else if (!within_parentheses || (*str != ' ' && *str != '\t')) {
            *dst++ = *str;
        }
        str++;
    }
    *dst = '\0';
}


#pragma endregion


#pragma region NFA Stack

nfa_stack* create_nfa_stack() {
    nfa_stack* s = (nfa_stack*)malloc(sizeof(nfa_stack));
    if (!s) return nullptr;
    s->top = nullptr;
    return s;
}

void push(nfa_stack* s, NFA* nfa) {
    nfa_node* new_node = (nfa_node*)malloc(sizeof(nfa_node));
    if (!new_node) return;
    new_node->nfa = nfa;
    new_node->next = s->top;
    s->top = new_node;
}

NFA* pop(nfa_stack* s)
{
    if (s->top == nullptr) return nullptr;
    nfa_node* temp = s->top;
    NFA* nfa = temp->nfa;
    s->top = temp->next;
    free(temp);
    return nfa;
}

bool is_stack_empty(nfa_stack* s) {
    return s->top == nullptr;
}

void free_stack(nfa_stack* s)
{
    while (!is_stack_empty(s)){
        NFA_free(pop(s));
    }
    free(s);
}

#pragma endregion


#pragma region Linear Term parser

void add_term(linear_expression* expr, int coefficient, const char* variable)
{
    for (int i = 0; i < expr->terms_count; i++) {
        if (strcmp(expr->terms[i].variable, variable) == 0) {
            expr->terms[i].coefficient += coefficient;
            return;
        }
    }
    expr->terms = (term*)realloc(expr->terms, (expr->terms_count + 1) * sizeof(term));
    expr->terms[expr->terms_count].coefficient = coefficient;
    strcpy(expr->terms[expr->terms_count].variable, variable);
    expr->terms_count++;
}

linear_expression* init_linear_expression()
{
    linear_expression* expr = (linear_expression*)malloc(sizeof(linear_expression));
    expr->terms = nullptr;
    expr->terms_count = 0;
    expr->constant = 0;
    return expr;
}

void free_linear_expression(linear_expression* expr)
{
    free(expr->terms);
    free(expr);
}

linear_expression* parse_linear_expression(const char* input)
{
    linear_expression* expr = init_linear_expression();

    const char* p = input;
    while (*p)
    {
        int coefficient = 0;
        char variable[100] = {0};
        char* var_start = variable;

        while (*p == ' ' || *p == '+') p++;

        if (isdigit(*p)){
            while (isdigit(*p)){
                coefficient = coefficient * 10 + (*p - '0');
                p++;
            }
        } else {
            coefficient = 1;
        }

        if (*p == '*') p++;

        while (isalpha(*p) || *p == '_' || isdigit(*p))
        {
            *var_start++ = *p++;
        }
        *var_start = '\0';

        if (strlen(variable) > 0) {
            add_term(expr, coefficient, variable);
        } else {
            expr->constant += coefficient;
        }
    }

    return expr;
}

void print_linear_expression(linear_expression* expr)
{
    for (int i = 0; i < expr->terms_count; i++)
    {
        printf("%d%s ", expr->terms[i].coefficient, expr->terms[i].variable);
        if (i < expr->terms_count - 1) cout << "+ ";
    }
    if (expr->constant != 0)
    {
        cout << "+ " << expr->constant;
    }
    cout << "\n";
}

#pragma endregion;