//
// Coded by Zelendzu & Umato on 29.02.2024.
//

// maybe it'd be better to replace all "int letter" with "unsigned int letter"?...
// 0*x1, 0*x2 -> x1, x2
// что делать с нулями в начале и строки состоящей полностью из нулей? 1) строка 2) количество незначащих нулей

// eval Ex(2|x)
// def (x=y+z /\ ~2|x) \/ 3|x
// def pair(x,y) "E x (x=y+z)"

// add epsilon-reached to accept methods and nfa_to_dfa where we search for epsilon-initial states
// TEST ALL FUNCTIONS!!!!!!!
// ОШИБКА В NFA_to_DOT если нет NFA0
// NFA_remove_unreachable_states в minimize и других

#include "NFA.h"

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

    stack* current_states = create_stack();
    stack* next_states;
    bool* next_states_added;

    char bit;
    size_t total_bits = strlen(new_num);


    push(current_states, nfa->initial_state->id);
    node* eps_initial = nfa->initial_state->transitions[(1 << nfa->alphabet_dim)]->head;
    while (eps_initial)
    {
        push(current_states, eps_initial->val);
        eps_initial = eps_initial->next;
    }

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

    push(current_states, nfa->initial_state->id);
    node* eps_initial = nfa->initial_state->transitions[(1 << nfa->alphabet_dim)]->head;
    while (eps_initial)
    {
        push(current_states, eps_initial->val);
        eps_initial = eps_initial->next;
    }

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

    fprintf(file, "digraph finite_state_machine{\n");
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

    // structure for storing labels of transitions
    char*** labels = (char***)malloc(nfa->states_count * sizeof(char**));
    for (int i = 0; i < nfa->states_count; i++) {
        labels[i] = (char**)malloc(nfa->states_count * sizeof(char*));
        for (int j = 0; j < nfa->states_count; j++) {
            labels[i][j] = (char*)calloc(128, sizeof(char)); // Предположим, что достаточно 128 символов для всех меток
        }
    }

    // filling label for every transition
    for (size_t i = 0; i < nfa->states_count; i++) {
        NFA_state* state = nfa->states[i];
        for (size_t letter = 0; letter <= (1 << nfa->alphabet_dim); letter++) {
            list* transition_list = state->transitions[letter];
            if (!transition_list || !transition_list->head) {
                continue;
            }

            node* current = transition_list->head;
            while (current) {
                if (strlen(labels[state->id][current->val]) > 0) {
                    strcat(labels[state->id][current->val], ",");
                }
                char letter_str[10];
                if (letter == (1 << nfa->alphabet_dim))
                    sprintf(letter_str, "%c", 'E');
                else {
                    char binary_representation[32];
                    sprintf(binary_representation, "");
                    for (int bit = nfa->alphabet_dim - 1; bit >= 0; --bit) {
                        strcat(binary_representation, (letter & (1 << bit)) ? "1" : "0");
                    }
                    strcpy(letter_str, binary_representation);
                }
                strcat(labels[state->id][current->val], letter_str);

                current = current->next;
            }
        }
    }

    // output transitions
    for (int i = 0; i < nfa->states_count; i++) {
        for (int j = 0; j < nfa->states_count; j++) {
            if (strlen(labels[i][j]) > 0) {
                fprintf(file, "\t%d -> %d [label = \"%s\"];\n", i, j, labels[i][j]);
            }
        }
    }

    fprintf(file, "}\n");
    fclose(file);

    // free memory
    for (int i = 0; i < nfa->states_count; i++) {
        for (int j = 0; j < nfa->states_count; j++) {
            free(labels[i][j]);
        }
        free(labels[i]);
    }
    free(labels);

    printf("NFA has been written to %s\n", filename);
    sprintf(filename, "%s%d", base_filename, counter);
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

        for (size_t letter = 0; letter < (1 << automaton->alphabet_dim); letter++) {
            list* transition_list = state->transitions[letter];
            if (transition_list == nullptr || transition_list->head == nullptr) continue;

            node* current = transition_list->head;
            while (current != nullptr) {
                for (size_t bit = 0; bit < automaton->alphabet_dim; bit++) {
                    fprintf(file, "%d ", (letter >> (automaton->alphabet_dim - 1 - bit)) & 1);
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
        //        cout << line << endl;

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
                //                cout << line << endl;
                int letter, end_state;
                int bit;
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

            for (size_t letter = 0; letter <= (1 << alphabet_dim); letter++) {
                list* list1 = nfa1->states[i]->transitions[letter];
                list* list2 = nfa2->states[j]->transitions[letter];

                for (node* node1 = list1->head; node1 != nullptr; node1 = node1->next) {
                    for (node* node2 = list2->head; node2 != nullptr; node2 = node2->next) {
                        int new_end_state = node1->val * nfa2->states_count + node2->val;
                        NFA_transition_add(intersected_nfa, combined_state_id, new_end_state, letter);
                    }
                }

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
    int combined_states_count = nfa1->states_count * nfa2->states_count + 1;
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

void DFA_complement(NFA* nfa) {
    if (!nfa) return;

    if (!NFA_is_DFA(nfa)) {
        printf("The automaton is not a DFA.");
        return;
    }

    for (int i = 0; i < nfa->states_count; i++) {
        nfa->states[i]->is_final = !nfa->states[i]->is_final;
    }
}

NFA* DFA_complement_return(NFA* nfa)
{
    if (!nfa || !NFA_is_DFA(nfa)) return nullptr;

    NFA* complement_dfa = NFA_clone(nfa);
    for (int i = 0; i < complement_dfa->states_count; i++){
        complement_dfa->states[i]->is_final = !complement_dfa->states[i]->is_final;
    }

    return complement_dfa;
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
    NFA_remove_unreachable_states(inter_nfa);

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
        NFA_remove_unreachable_states(inter_nfa);

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

NFA* NFA_to_DFA(NFA* nfa)
{
    if (!nfa || nfa->states_count == 0) return nfa;
    NFA_remove_unreachable_states(nfa); // Be careful! Removes unreachable states from initial nfa!

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

    return dfa;
}

void NFA_to_DFA_rec(NFA** nfa)
{
    NFA* dfa = NFA_to_DFA(*nfa);
    NFA_free(*nfa);
    *nfa = dfa;
}

NFA* DFA_minimize(NFA* nfa)
{
    if (!nfa || !NFA_is_DFA(nfa)) return nullptr;

    NFA_remove_unreachable_states(nfa);

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
                    need_check = 1;
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
    int* final_states = NULL;
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

    NFA_remove_unreachable_states(new_nfa);

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

// NEED TO BE TESTED
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
                ((dest_state1 == NULL ^ dest_state2 == NULL) || ((*state_group)[dest_state1->val] != (*state_group)[dest_state2->val])))
            {
                add_to_list(new_group, current->val);
                (*state_group)[current->val] = *groups_count;
                group_was_changed = 1;
                break;
            }
        }

        if (!group_was_changed) add_to_list(current_group, current->val);

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

// NEED TO BE TESTED
void NFA_remove_unreachable_states(NFA* nfa)
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
        state->transitions[epsilon] = nullptr;

        free(epsilon_closure);
    }
}

#pragma endregion


#pragma region NFA Examples
NFA* NFA_get_div_2()
{
    int* final_states = (int*)malloc(sizeof(int));
    final_states[0] = 1;
    NFA* nfa = NFA_init(3, 1, 0, 1, final_states);

    NFA_transition_add(nfa, 0, 1, 0);
    NFA_transition_add(nfa, 0, 2, 1);
    NFA_transition_add(nfa, 1, 1, 0);
    NFA_transition_add(nfa, 1, 1, 1);
    NFA_transition_add(nfa, 2, 2, 0);
    NFA_transition_add(nfa, 2, 2, 1);

    return nfa;
}

NFA* NFA_get_div_3()
{
    int* final_states = (int*)malloc(sizeof(int));
    final_states[0] = 0;
    NFA* nfa = NFA_init(3, 1, 0, 1, final_states);

    NFA_transition_add(nfa, 0, 0, 0);
    NFA_transition_add(nfa, 0, 1, 1);
    NFA_transition_add(nfa, 1, 0, 1);
    NFA_transition_add(nfa, 1, 2, 0);
    NFA_transition_add(nfa, 2, 1, 0);
    NFA_transition_add(nfa, 2, 2, 1);

    return nfa;
}

NFA* NFA_get_div_power_of_2(int power)
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

NFA* NFA_get_sum3()
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
    int alphabet_dim = 2;
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

NFA* NFA_get_div_num(int num)
{
    // x = 2y автомат проверяющий что <-
    if (!num) return nullptr;
    num = abs(num);


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
        }
        else {
            printf("Unknown command: %s\n", input);
        }
    }

}

void print_help() {
    cout << "Available commands:\n";
    cout << "exit - Exit the NFA Console Application.\n";
    cout << "help - Display this help message.\n";
    cout << "nfa_list - List available automata.\n";
    cout << "def <name> \"<predicate>\" - Define a new NFA from a logical predicate and save it.\n";
    cout << "eval \"$automaton_name(num1, num2, ... numN)\" - Evaluate an automaton with a given number.\n";
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
    case '~': return 4;
    case 'E':
    case 'A':
        return 3;
    case '&':
        return 2;
    case '|':
        return 1;
    case '$':
        return 0;
    defaul:
        return -1;
    }
}

char* NFA_RPN(const char* formula) {
    stack* operators = create_stack();
    char* rpn = (char*)malloc(2 * strlen(formula) + 1);
    int j = 0;
    for (char *c = (char*)formula; *c != '\0'; c++) {
        switch (*c)
        {
        case '(' :
            push(operators, (int)(*c));
            break;
        case '&' :
        case '|' :
        case '~' :
        case 'E' :
        case 'A' :
            while (!is_stack_empty(operators) && nfa_get_priority(*c) <= nfa_get_priority(stack_top(operators))) {
//                cout << (char)stack_top(operators) << " " << nfa_get_priority(stack_top(operators)) << " " << nfa_get_priority(*c) << endl;
                rpn[j++] = pop(operators);
                rpn[j++] = ' ';
            }
            push(operators, (int)(*c));
            break;
        case ')' :
            while (!is_stack_empty(operators) && stack_top(operators) != '(') {
                rpn[j++] = ' ';
                rpn[j++] = (char)pop(operators);
            }
            pop(operators);
            break;
        case '$' :
            rpn[j++] = *c;
            c++;
            while (*c != ' ' && *c != '\0' && !strchr("&|~E", *c)) {
                if (rpn[j - 1] == ')') break;
                rpn[j++] = *(c++);
            }
            c--;
            break;
        default:
            if (j != 0 && *c == ' ' && rpn[j - 1] != ' ')
                rpn[j++] = *c;
            else if (*c != ' ') // ? Exception ?? with pointer to its position?
                rpn[j++] = *c;
            break;
        }
    }

    while (!is_stack_empty(operators)) {
        rpn[j++] = ' ';
        rpn[j++] =(char)pop(operators);
    }

    if (j > 0 && rpn[j - 1] == ' ') {
        j--;
    }

    rpn[j] = '\0';
    free_stack(operators);
    return rpn;
}

NFA* load_NFA_from_file(const char* filename)
{
    char path[256];
    sprintf(path, "../NFA/NFAs/%s.txt", filename);

    return NFA_from_file(path);
}

NFA* NFA_from_predicate(const char* predicate) {
    char* rpn = NFA_RPN(predicate);

    nfa_stack* nfa_stack = create_nfa_stack();

    char* token = strtok(rpn, " ");
    while (token) {
        if (token[0] == '$') {
            char filename[256];
            const char* start = token + 1;
            const char* end = strchr(start, '(');

            if (end == nullptr) {
                end = start + strlen(start);
            }

            size_t length = end - start;
            strncpy(filename, start, length);
            filename[length] = '\0';

            NFA* nfa = load_NFA_from_file(filename);
            NFA_to_DOT(nfa);
            push(nfa_stack, nfa);
        } else {

            if (strcmp(token, "&") == 0) {
                NFA* nfa2 = pop(nfa_stack);
                NFA* nfa1 = pop(nfa_stack);
                NFA_to_DOT(nfa2);
                NFA_to_DOT(nfa1);

                NFA* result = NFA_intersect(nfa1, nfa2);
                NFA_free(nfa1);
                NFA_free(nfa2);
                NFA_to_DOT(result);

                push(nfa_stack, result);
            } else if (strcmp(token, "|") == 0) {
                NFA* nfa2 = pop(nfa_stack);
                NFA* nfa1 = pop(nfa_stack);
                NFA_to_DOT(nfa2);
                NFA_to_DOT(nfa1);
                NFA* result = NFA_union(nfa1, nfa2);
                NFA_free(nfa1);
                NFA_free(nfa2);
                NFA_to_DOT(result);

                push(nfa_stack, result);
            } else if (strcmp(token, "~") == 0) {
                NFA* nfa = pop(nfa_stack);
                NFA_to_DOT(nfa);

                NFA* comp = DFA_complement_return(nfa);
                NFA_to_DOT(comp);
                NFA_free(nfa);

                if(!comp) {
                    free(rpn);
                    NFA_free(comp);
                    free_stack(nfa_stack);
                    return nullptr;}
                push(nfa_stack, comp);
            }
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
    if (sscanf(command, "def %s \"%[^\"]\"", name, predicate) == 2) {
        NFA *nfa = NFA_from_predicate(predicate);
        if (nfa != nullptr) {
            char filename[300];
            sprintf(filename, "../NFA/NFAs/%s.txt", name);
            NFA_to_file(nfa, filename);
            NFA_free(nfa);
        } else {
            cout << "Failed to create NFA from predicate.\n";
        }
    } else {
        cout << "Invalid command format. Use def <name> \"<predicate>\"\n";
    }
}

void NFA_eval_command(const char* command)
{
    char automaton_name[256];
    char number_string[1024];
    int nums[100]; // max = 100
    int num_count = 0;

    if (sscanf(command, "eval \"$%[^(](%[^)]", automaton_name, number_string) == 2) {
        NFA* nfa = load_NFA_from_file(automaton_name);
        if (!nfa) {
            cout << "Failed to load NFA: " << automaton_name << endl;
            return;
        }

        char* token = strtok(number_string, ", ");
        while (token != nullptr && num_count != nfa->alphabet_dim) {
            nums[num_count++] = atoi(token);
            token = strtok(nullptr, ", ");
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