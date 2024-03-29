//
// Coded by Zelendzu & Umato on 29.02.2024.
//

// maybe it'd be better to replace all "int letter" with "unsigned int letter"?...
// 0*x1, 0*x2 -> x1, x2
// что делать с нулями в начале и строки состоящей полностью из нулей?

// eval Ex(2|x)
// def (x=y+z /\ ~2|x) \/ 3|x
// def pair(x,y) "E x (x=y+z)"

// проследить везде за free


#include "NFA.h"

#pragma region Others

void list_free(list* l) {
    while (l && l->head) {
        node* tmp = l->head;
        l->head = l->head->next;
        free(tmp);
    }
    free(l);
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
        state->transitions[i] = (list*)calloc(1, sizeof(list));
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

    for (size_t i = 0; i < final_states_count; i++) {
        if (final_states[i] < states_count) {
            nfa->states[final_states[i]]->is_final = true;
        }
    }

    return nfa;
}

NFA* NFA_clone(NFA* nfa)
{
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
    new_state->next = nullptr;
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
    int id = nfa->states_count;
    if (!nfa) return false;

    nfa->states = (NFA_state**)realloc(nfa->states, (nfa->states_count + 1) * sizeof(NFA_state*));

    NFA_state* newState = NFA_state_init(id, is_final, (1 << nfa->alphabet_dim) + 1);

    nfa->states[id] = newState;
    nfa->states_count++;

    return true;
}

// NEED TO BE TESTED
bool NFA_state_remove(NFA* nfa, int id)
{
    if (!nfa || id < 0 || id >= nfa->states_count) return false;

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

// NEED TO BE TESTED
bool NFA_state_list_remove(NFA* nfa, int* removed_states, int list_size)
{
    // removed_states has to be in the next format: [0, 1, 1, 0, ..., 0], where states with "1" should be removed

    if (!nfa || list_size != nfa->states_count) return false;

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

void NFA_free(NFA* nfa)
{
    if (nfa == nullptr) return;

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
    free(nfa);
}

bool NFA_accept(NFA* nfa, big_int* num)
{
    stack* current_states = create_stack();
    stack* next_states;
    bool* next_states_added;

    char bit;
    size_t total_bits = num->length << 3;
    size_t leading_zeroes = 0;

    // first method to find leading zeroes
    //for (size_t i = total_bits - 8; i < total_bits && (num->number[i >> 3] & (1 << (7 - (i & 7)))) == 0; i++) leading_zeroes++;
    //second method to find leading zeroes
    int n = num->number[num->length - 1];
    while (n >> (leading_zeroes)) leading_zeroes++;
    leading_zeroes = 8 - leading_zeroes;

    total_bits -= leading_zeroes; // leading zeroes shouldn't be processed


    push(current_states, nfa->initial_state->id);
    // states that have epsilon-transition from initial_state are also initial
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
        bit = ((num->number[i >> 3]) & (1 << (i & 7))) ? 1 : 0; // current input
        //bit = ((num->number[(i + leading_zeroes) >> 3]) & ((i + leading_zeroes) & 7)) ? 1 : 0; // if we moved left->right

        while (!is_stack_empty(current_states))
        {
            node* dest = nfa->states[pop(current_states)]->transitions[bit]->head;


            while (dest) // check all states we can reach from this current_state
            {
                if (!next_states_added[dest->val]) // if destination_state is not in the next_states
                {
                    push(next_states, dest->val); // push it in next_states
                    next_states_added[dest->val] = 1;

                    // we also check all states that have epsilon-transition from this destination_state
                    // and push them in next_states
                    node* eps_dest = nfa->states[dest->val]->transitions[(1 << nfa->alphabet_dim)]->head;
                    while (eps_dest)
                    {
                        if (!next_states_added[eps_dest->val])
                        {
                            push(next_states, eps_dest->val);
                            next_states_added[eps_dest->val] = 1;
                        }
                        eps_dest = eps_dest->next;
                    }
                }

                dest = dest->next;
            }
        }

        free_stack(current_states);
        free(next_states_added);
        current_states = next_states;
        next_states = nullptr;
    }

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

bool NFA_accept(NFA* nfa, big_int_list* bigint_list)
{
    if (!nfa || !bigint_list || nfa->alphabet_dim != bigint_list->count) return 0;

    stack* current_states = create_stack();
    stack* next_states;
    bool* next_states_added;

    // finding total bits
    int total_bits = 0;
    for (int i = 0; i < bigint_list->count; i++)
    {
        int current_total_bits = bigint_list->big_ints[i]->length << 3;
        if ((current_total_bits >> 3) >= ((total_bits + 7) >> 3)) // compare bytes count
        {
            int digits = 0;
            int n = bigint_list->big_ints[i]->number[(current_total_bits >> 3) - 1]; //last byte
            while (n >> digits) digits++;
            current_total_bits -= (8 - digits); //subtract leading zeroes

            if (current_total_bits > total_bits) total_bits = current_total_bits;
        }
    }
    if (!total_bits) total_bits = 8;

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

        for (int j = 0; j < bigint_list->count; j++)
        {
            big_int* num = bigint_list->big_ints[j];
            letter <<= 1;
            if ((i >> 3) < num->length)
                letter += ((num->number[i >> 3]) & (1 << (i & 7))) ? 1 : 0;
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

                    node* eps_dest = nfa->states[dest->val]->transitions[(1 << nfa->alphabet_dim)]->head;
                    while (eps_dest)
                    {
                        if (!next_states_added[eps_dest->val])
                        {
                            push(next_states, eps_dest->val);
                            next_states_added[eps_dest->val] = 1;
                        }
                        eps_dest = eps_dest->next;
                    }
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

#pragma endregion

#pragma region NFA Operations

NFA* intersect_NFA(NFA* nfa1, NFA* nfa2)
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

NFA* union_NFA(NFA* nfa1, NFA* nfa2)
{
    if (!nfa1 || !nfa2 || nfa1->alphabet_dim != nfa2->alphabet_dim) return nullptr;

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

NFA* NFA_rightquo(NFA* nfa1, NFA* nfa2)
{
    // new nfa is the same as nfa1 except for final states

    if (!nfa1 || !nfa2 || nfa2->alphabet_dim > nfa1->alphabet_dim) return nullptr;

    NFA* new_nfa = NFA_clone(nfa1);
    NFA* nfa2_clone = NFA_clone(nfa2);
    // edit nfa2 so that it can be intersected with nfa1
    for (int i = 0; i < nfa1->alphabet_dim - nfa2->alphabet_dim; i++)
    {
        NFA* nfa_temp = NFA_extend(nfa2_clone, nfa2_clone->alphabet_dim); 
        NFA_free(nfa2_clone);
        nfa2_clone = nfa_temp;
        nfa_temp = nullptr;
    }

    // if there exist a path from current state to a final state that accepts a word from nfa2 language
    // then current state will be one of the new final states
    int* state_is_new_final = (int*)calloc(new_nfa->states_count, sizeof(int));
    for (int i = 0; i < new_nfa->states_count; i++)
    {
        new_nfa->initial_state = new_nfa->states[i];
        NFA* inter_nfa = intersect_NFA(new_nfa, nfa2_clone);
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

    NFA_free(nfa2_clone);

    return new_nfa;
}

#pragma endregion

#pragma region NFA Support Functions

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
    int states_count = get_random_num(2, 6);
    int alphabet_dim = 1;
    int initial_state = get_random_num(0, states_count);
    int final_states_count = get_random_num(1, states_count);
    int* final_states = (int*)calloc(final_states_count, sizeof(int));
    for (int i = 0; i < final_states_count; i++)
    {
        final_states[i] = get_random_num(0, states_count);
    }

    NFA* nfa = NFA_init(states_count, alphabet_dim, initial_state, final_states_count, final_states);

    for (int i = 0; i < states_count; i++)
    {
        for (int letter = 0; letter < (1 << alphabet_dim); letter++)
        {
            int transitions_count = get_random_num(0, states_count > 3 ? 3 : states_count);
            for (int tr = 0; tr < transitions_count; tr++)
            {
                NFA_transition_add(nfa, i, get_random_num(0, states_count), letter);
            }
        }
    }

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

#pragma endregion

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
        } else if (state->id == initial_state_id) {
            state_type = 0; // Only initial (0)
        } else if (state->is_final){
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
    if (filename == nullptr){
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
            } else {
                automaton->states[state_id]->is_final = state_type == 1 || state_type == 2;
            }

            if (state_type == 0 || state_type == 2)
            {
                if (initial_state == -1)
                {
                    initial_state = state_id;
                } else
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


                    } else break;


                }
            }

        }
    }

    if (initial_state == -1) {
        cout << "Error: there no initial state\n";
        fclose(file);
        NFA_free(automaton);
        return nullptr;
    } else {
        automaton->initial_state = automaton->states[initial_state];
    }

    fclose(file);
    return automaton;
}

void NFA_console_app() {
    char input[256];
    cout << "Enter command (type '>exit' to quit):\n";
    while (true) {
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, ">exit") == 0) {
            cout << "Exiting NFA Console Application.\n";
            break;
        } else if (strcmp(input, ">help") == 0) {
            print_help();
        } else if (strcmp(input, ">nfa_list") == 0) {
            NFA_list();
        } else {
                printf("You entered: %s\n", input);
            }
        }

}

#pragma region Handful Functions For Console App

void print_help() {
    cout << "Available commands:\n";
    cout << ">exit - Exit the NFA Console Application.\n";
    cout << ">help - Display this help message.\n";
    cout << ">nfa_list - List available automata.\n";
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

#pragma endregion