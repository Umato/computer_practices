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

        std::cout << " State " << state->id << " ["
                  << (state->is_final ? "final" : "non-final") << "]:" << std::endl;

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

bool NFA_accept(NFA* automaton, big_int* num)
{
    return 0;
}

void NFA_to_DOT(NFA* automaton)
{
    if (automaton == nullptr) {
        printf("Invalid NFA.\n");
        return;
    }

    char base_filename[] = "NFA/NFA";
    char extension[] = ".gv.txt";
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