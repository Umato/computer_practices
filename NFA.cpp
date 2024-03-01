//
// Created by Umato on 29.02.2024.
//

#include "NFA.h"

NFA_state* NFA_state_init(int id, bool is_final, size_t letters_count)
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

NFA* NFA_init(size_t states_count, size_t alphabet_dim, int initial_state, int final_states_count, int* final_states)
{
    if ((initial_state >= states_count) || (final_states_count > states_count))
		return nullptr;

    size_t letters_count = (1 << alphabet_dim) + 1;
    NFA* automaton = (NFA*)malloc(sizeof(NFA));
    automaton->states_count = states_count;
    automaton->alphabet_dim = alphabet_dim;
    automaton->states = (NFA_state**) malloc(sizeof(NFA_state*) * states_count);

    for (size_t i = 0; i < states_count; i++) {
        automaton->states[i] = NFA_state_init(i, false, letters_count);
    }

    automaton->initial_state = automaton->states[initial_state];
    automaton->current_state = automaton->initial_state;

    for (size_t i = 0; i < final_states_count; i++) {
        if (final_states[i] < states_count) {
            automaton->states[final_states[i]]->is_final = true;
        }
    }

    return automaton;
}

void list_free(list* l) {
    while (l && l->head) {
        node* tmp = l->head;
        l->head = l->head->next;
        free(tmp);
    }
    free(l);
}

void NFA_free(NFA* automaton)
{
    if (automaton == nullptr) return;

    for (int i = 0; i < automaton->states_count; i++) {
        NFA_state* state = automaton->states[i];
        if (state) {
            for (size_t j = 0; j <= (1 << automaton->alphabet_dim); j++) {
                list_free(state->transitions[j]);
            }
            free(state->transitions);
            free(state);
        }
    }

    free(automaton->states);
    free(automaton);
}

void NFA_transition_add(NFA* automaton, int start_state, int end_state, int letter)
{
	if (!automaton || (letter > (1 << automaton->alphabet_dim)) || (start_state >= automaton->states_count) || (end_state >= automaton->states_count))
		return;

	node* new_state = (node*)malloc(sizeof(node));
	new_state->next = nullptr;
	new_state->val = end_state;

	if (!automaton->states[start_state]->transitions[letter]->head)
	{
		automaton->states[start_state]->transitions[letter]->head = new_state;
	}
	else
	{
		node* cur_state = automaton->states[start_state]->transitions[letter]->head;
		while (cur_state->val != end_state && cur_state->next)
		{
			cur_state = cur_state->next;
		}

		if (cur_state->val != end_state) cur_state->next = new_state;
	}
}

void NFA_transition_remove(NFA* automaton, int start_state, int end_state, int letter) {
    if (!automaton || start_state >= automaton->states_count || end_state >= automaton->states_count || letter > (1 << automaton->alphabet_dim)) return;

    list* trans = automaton->states[start_state]->transitions[letter];
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

void NFA_print(NFA* automaton) {
    if (automaton == nullptr) {
        std::cout << "Automaton is NULL." << std::endl;
        return;
    }

    std::cout << "NFA: States Count = " << automaton->states_count
              << ", Alphabet Size = " << automaton->alphabet_dim << std::endl;
    std::cout << "Initial State: " << automaton->initial_state->id << std::endl;
    std::cout << "Current state:" << automaton->current_state->id << std::endl;
    std::cout << "States:" << std::endl;

    for (size_t i = 0; i < automaton->states_count; i++) {
        NFA_state* state = automaton->states[i];
        if (state == nullptr) continue;

        std::cout << " State " << state->id << (state->is_final ? " [final]" : "") << std::endl;

        for (size_t letter = 0; letter <= (1 << automaton->alphabet_dim); letter++) {
            list* transition_list = state->transitions[letter];
            if (transition_list == nullptr || transition_list->head == nullptr) {
                continue;
            }

            std::cout << "  On '" << letter << "': ";
            node* current = transition_list->head;
            while (current != nullptr) {
                std::cout << current->val << " ";
                current = current->next;
            }
            std::cout << std::endl;
        }
    }
}

void NFA_to_DOT(NFA* automaton)
{
    if (automaton == nullptr) {
        printf("Invalid NFA.\n");
        return;
    }

    char base_filename[] = "NFA/NFA";
    char extension[] = ".gv";
    char filename[40];
    FILE *file;
    int counter = 0;

    do {
        if (counter == 0) {
            sprintf(filename, "%s%s", base_filename, extension);
        } else {
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
    fprintf(file, "\tnode [shape = doublecircle];");
    for (size_t i = 0; i < automaton->states_count; i++) {
        if (automaton->states[i]->is_final) {
            fprintf(file, " %d", automaton->states[i]->id);
        }
    }
    fprintf(file, ";\n");
    fprintf(file, "\tnode [shape = circle];\n");
    fprintf(file, "\tinit [shape=none, label=\"\"];\n");
    fprintf(file, "\tinit -> %d;\n", automaton->initial_state->id);

    for (size_t i = 0; i < automaton->states_count; i++) {
        NFA_state* state = automaton->states[i];
        for (size_t letter = 0; letter <= (1 << automaton->alphabet_dim);letter++) {
            list* transition_list = state->transitions[letter];
            if (!transition_list || !transition_list->head) {
                continue;
            }

            node* current = transition_list->head;
            while (current) {
                if (letter == (1 << automaton->alphabet_dim)) {
                    fprintf(file, "\t%d -> %d [label = \"e\"];\n", state->id, current->val);
                } else {
                    fprintf(file, "\t%d -> %d [label = \"%lu\"];\n", state->id, current->val, letter);
                }
                current = current->next;
            }
        }
    }

    fprintf(file, "}\n");
    fclose(file);

    printf("NFA has been written to %s\n", filename);
}

NFA* NFA_from_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Cannot open file\n");
        return nullptr;
    }

    size_t states_count = 0, alphabet_dim = 0;
    int initial_state = 0;
    bool* is_final = nullptr;

    char line[256];
    int state, next_state;
    char symbol;



}

bool NFA_accept(NFA* nfa, big_int* num)
{
	stack* current_states = create_stack();
	stack* next_states;
	bool* next_states_added;
	char bit;
	size_t total_bits = num->length << 3;
	size_t leading_zeroes = 0;

	for (int i = total_bits - 8; i < total_bits && (num->number[i >> 3] & (1 << (7 - (i & 7)))) == 0; i++) leading_zeroes++;
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
			return 1;
	}

	return 0;
}