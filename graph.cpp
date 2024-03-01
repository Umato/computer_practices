//
// Created by Umato on 16.11.2023.
//

#include "graph.h"

graph *graph_init(int vertices) {
    graph *g = (graph*)malloc(sizeof(graph));
    if (g) {
        g->count = vertices;
        g->adj_list = (list*)malloc(sizeof(list) * vertices);
        for (size_t i = 0; i < vertices; i++) {
            g->adj_list[i].head = nullptr;
        }
    }
    return g;
}

void add_arc(graph *g, int src, int dest) {
    node *new_node = (node*)malloc(sizeof(node));
    new_node->val = dest;
    new_node->next = nullptr;

    if(g->adj_list[src].head == nullptr) {
        g->adj_list[src].head = new_node;
    } else {
        node *current = g->adj_list[src].head;
        if (current->val == dest) return;
        while (current->next != nullptr) {
            current = current->next;
            if (current->val == dest) return;
        }
        current->next = new_node;
    }
}

void add_edge(graph *g, int src, int dest) {
    add_arc(g, src, dest);
    add_arc(g, dest, src);
}

void del_arc(graph *g, int src, int dest) {
    node *current = g->adj_list[src].head;
    node *previous = nullptr;

    while (current) {
        if (current->val == dest) {
            if (previous == nullptr) {
                g->adj_list[src].head = current->next;
            }
            else {
                previous->next = current->next;
            }

            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

void graph_print(graph *g) {
    std::cout << g->count << '\n';
    for (int i = 0; i < g->count; i++) {
        std::cout << i << ": ";
        node *temp = g->adj_list[i].head;
        while (temp) {
            std::cout << temp->val << " ";
            temp = temp->next;
        }
        std::cout << "\n";
    }
}

void graph_free(graph *g) {
    for (int i = 0; i < g->count; i++) {
        node *current = g->adj_list[i].head;
        while (current) {
            node *temp = current;
            current = current->next;
            free(temp);
        }
        g->adj_list[i].head = nullptr;
    }
    free(g->adj_list);

    g->count = 0;
    free(g);
}

graph *graph_create() {
    int n;
    std::cin >> n;
    graph *g = graph_init(n);
    int val;
    for (int i = 0; i < n; i++) {
        while (true) {
            std::cin >> val;
            if (val == -1) break;
            add_arc(g, i, val);
        }
    }
    return g;
}

graph *graph_create_undirected() {
    int n;
    std::cin >> n;

    graph *g = graph_init(n);
    int u, v;

    while (true) {
        std:: cin >> u;
        if (u == -1) break;
        std:: cin >> v;
        if (v == -1) break;

        add_edge(g, u, v);
    }
    return g;
}

graph *create_complete_graph(int num_vertices) {
    graph *g = graph_init(num_vertices);
    for (int i = 0; i < g->count; i++) {
        for (int j = i + 1; j < g->count; j++) {
            add_edge(g, i, j);
        }
    }
    return g;
}

int *topological_sort(graph *g) {
    stack *temp_result = create_stack();
    int *result = (int*)calloc(g->count, sizeof(int));
    stack* cycle = create_stack();
    int *visited = (int*)calloc(g->count,sizeof(int));
    int CycleTop = -1;

    for (int i = 0; i < g->count; i++) {
        if (visited[i] == 0) {
            if (!topological_sort_dfs(g, i, temp_result, visited, cycle, CycleTop)) {
                std::cout << "Cycle: ";
                while (!is_stack_empty(cycle)) {
                    std::cout << pop(cycle) << ' ';
                }
                free_stack(temp_result);
                free(result);
                free(visited);
                return nullptr;
            }
        }
    }

    int j = 0;
    while (!is_stack_empty(temp_result)) {
        int a = pop(temp_result);
        result[j++] = a;
    }
    free_stack(temp_result);
    free(visited);
    free_stack(cycle);

    return result;
}

bool topological_sort_dfs(graph *g, int vertex, stack* result, int *visited, struct stack* cycle, int& CycleTop)
{
    visited[vertex] = 1;
    node* n = g->adj_list[vertex].head;
    while (n != nullptr) {
        if (visited[n->val] == 0) {
            if (!topological_sort_dfs(g, n->val, result, visited, cycle, CycleTop)) {
                if (CycleTop != -1) {
                    push(cycle, n->val);
                    if (n->val == CycleTop) {
                        CycleTop = -1;
                    }
                }
                return false;
            }
        } else if (visited[n->val] == 1) {
            CycleTop = n->val;
            push(cycle, n->val);
            return false;
        }
        n = n->next;
    }

    push(result, vertex);
    visited[vertex] = 2;
    return true;
}

scc_list *tarjanSCC(graph *g) {
    TarjanInfo info;
    info.index = (int *)calloc(g->count, sizeof(int));
    info.lowLink = (int *)calloc(g->count, sizeof(int));
    info.onStack = (bool *)calloc(g->count, sizeof(bool));
    info.currentIndex = 0;
    info.s = create_stack();
    scc_list *result = InitSCCList();
    int sccCounter = 0;

    for (int i = 0; i < g->count; ++i) {
        info.index[i] = -1;
    }

    for (int i = 0; i < g->count; ++i) {
        if (info.index[i] == -1) {
            strongConnect(g, i, &info, &sccCounter, result);
        }
    }

    free(info.index);
    free(info.lowLink);
    free(info.onStack);
    free_stack(info.s);

    return result;
}

void strongConnect(graph *g, int v, TarjanInfo *info, int *sccCounter, scc_list* result)
{
    info->index[v] = info->currentIndex;
    info->lowLink[v] = info->currentIndex;
    info->currentIndex++;
    push(info->s, v);
    info->onStack[v] = true;

    for (node *w = g->adj_list[v].head; w != nullptr; w = w->next) {
        if (info->index[w->val] == -1) {
            strongConnect(g, w->val, info, sccCounter, result);
            info->lowLink[v] = (info->lowLink[v] < info->lowLink[w->val] ? info->lowLink[v] : info->lowLink[w->val]);
        } else if (info->onStack[w->val]) {
            info->lowLink[v] = (info->lowLink[v] < info->index[w->val] ? info->lowLink[v] : info->index[w->val]);
        }
    }

    if (info->lowLink[v] == info->index[v]) {
        list new_scc;
        new_scc.head = nullptr;

        int w;
        do {
            w = pop(info->s);
            info->onStack[w] = false;

            node *new_node = (node*)malloc(sizeof(node));
            new_node->val = w;
            new_node->next = new_scc.head;
            new_scc.head = new_node;

        } while (w != v);

        AppendSCCList(new_scc, result);

    }
}

scc_list *InitSCCList() {
    scc_list *list = (scc_list*)malloc(sizeof(scc_list));
    list->len = 0;
    list->array = nullptr;
    return list;
}

void AppendSCCList(list scc,scc_list *scc_list){
    (scc_list->len)++;
    scc_list->array=(list *)realloc( scc_list->array,(scc_list->len) * sizeof(list));
    scc_list->array[(scc_list->len)-1]=scc;
}

void FreeSCCList(scc_list *scc) {
    for (int i = 0; i < scc->len; i++) {
        node* nd = scc->array[i].head;
        while (nd) {
            node * temp = nd;
            nd = nd->next;
            free(temp);
        }
    }
    free(scc->array);
    free(scc);
}

void printSCCLis(scc_list *list) {
    std::cout << "SCC\n";
    for (int i = 0; i < list->len; i++) {
        std::cout << i << ") ";
        node *curr = list->array[i].head;
        while (curr) {
            std::cout << curr->val;
            curr = curr->next;
        }
        std::cout << "\n";
    }
}

CNF2 *InitCNF2(int count)
{
    CNF2 *cnf = (CNF2*)malloc(sizeof(CNF2));
    cnf->clauses = count;
    cnf->array = (int**) malloc(count * sizeof(int *));
    for (int i = 0; i < count; i++) {
        cnf->array[i] = (int *)malloc(2 * sizeof(int));
    }
    return cnf;
}

CNF2 *GetCNF2(char *str) {
    if (str == nullptr || strlen(str) == 0) {
        std::cout << "Empty string";
        return nullptr;
    }

    int count = 0;
    int mx = 0;
    int curr;
    int flag = 0;
    int ord;
    int clausesIndex = -1;

    bool flag2 = false;

    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '(') ++count;
        else if (!isdigit(str[i]) && str[i] != 'x' && str[i] != '|' && str[i] != '!' && str[i] != ')' && str[i] != ' ' && str[i] != '&') {
            std::cout << "Invalid character: " << str[i];
            return nullptr;
        }
    }

    CNF2 *cnf = InitCNF2(count);

    for (char *p = str; *p != '\0'; p++) {
        if (*p == '(') {
            clausesIndex++;
            flag = 0;
        } else if (*p == '!') flag2 = true;
        else if (*p == 'x') {
            curr = 0;
            ord = 0;
            p++;
            curr = strtol(p, &p, 10);
            mx = (mx >= curr) ? mx : curr;
            cnf->max = mx;

            cnf->array[clausesIndex][flag % 2] = ((flag2) ? -1 : 1) * curr;
            flag++;
            flag2 = false;
        } else if (*p == ')' && flag != 2) {
            std::cout << "Incorrect number of literals in the clause.\n";
            return nullptr;
        }
    }

    if (clausesIndex != count - 1) {
        std::cout << "Incorrect number of clauses\n";
        return nullptr;
    }
    return cnf;
}

void PrintCNF2(CNF2 *cnf) {
    for (int i = 0; i < cnf->clauses; i++) {
        std::cout << "(";
        if (cnf->array[i][0] >= 0) {
            std::cout << "x" << cnf->array[i][0] << "||";
        } else {
            std::cout << "!x" << (-1) * cnf->array[i][0] << "||";
        }

        if (cnf->array[i][1] >= 0) {
            std::cout << "x" << cnf->array[i][1] << ')';
        } else {
            std::cout << "!x" << (-1) * cnf->array[i][1] << ')';
        }

        if (i != (cnf->clauses) - 1) std::cout << "&&";
    }
    std::cout<<"\n";
}

void FreeCNF2(CNF2* cnf) {
    for (int i = 0; i < cnf->clauses; i++) {
        free(cnf->array[i]);
    }
    free(cnf->array);
    free(cnf);
}

graph *createImplicationGraph(CNF2 *cnf) {
    int M = cnf->clauses, N = cnf->max;
    graph *g = graph_init(1 + 2 * N);

    for (int i = 0; i < M; i++) {

        int x1 = cnf->array[i][0], x2 = cnf->array[i][1];

        if (x1 < 0) { x1 = abs(x1) + N; }
        if (x2 < 0) { x2 = abs(x2) + N; }

        add_arc(g, x1 + (x1 <= N ? N : -(1) * N), x2);
        add_arc(g, x2 + (x2 <= N ? N : -(1) * N), x1);
    }

    return g;
}

int *Solve2sat(CNF2 *cnf) {
    graph *g = createImplicationGraph(cnf);

    scc_list *sccs = tarjanSCC(g);

    int *comp = (int *)calloc(g->count, sizeof(int));
    for (int i = 0; i < g->count; i++) {
        comp[i] = -1;
    }

    int scc_id = 0;
    for (int i = 0; i < sccs->len; i++) {
        node *current = sccs->array[i].head;
        while (current) {
            if (comp[current->val] == -1) {
                comp[current->val] = scc_id;
            }
            current = current->next;
        }
        scc_id++;
    }

    for (int i = 1; i <= cnf->max; i++) {
        if (comp[i] == comp[i + cnf->max]) {
            FreeSCCList(sccs);
            graph_free(g);
            free(comp);
            std::cout << "Unsatisfiable\n";
            return nullptr;
        }
    }
    int* result = (int*)calloc(cnf->max + 1, sizeof(int));
    for (int i = 1; i <= cnf->max; i++) {
        result[i] = (comp[i] <= comp[i + cnf->max]) ? 1: 0;
    }

    for (int i = 1; i <= cnf->max; i++) {
        std::cout << "Variable " << i << ": " << (result[i] ? "True" : "False") << std::endl;
    }

    FreeSCCList(sccs);
    graph_free(g);
    free(comp);

    return result;
}