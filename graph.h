//
// Created by Umato on 16.11.2023.
//

#ifndef COMPUTER_PRACTICE_GRAPH_H
#define COMPUTER_PRACTICE_GRAPH_H

#include <iostream>
#include <fstream>
#include "algorithm.h"
#include <cstdlib>
#include <iostream>
#include <cstdio>

typedef struct TarjanInfo {
    int *index;
    int *lowLink;
    int currentIndex;
    bool *onStack;
    struct stack *s;
} TarjanInfo;

typedef struct node {
    int val;
    struct node *next;
} node;

typedef struct list {
    node *head;
} list;

typedef struct graph {
    int count;
    list *adj_list;
} graph;

typedef struct scc_list {
    struct list* array;
    int len;
} scc_list;

typedef struct CNF2 {
    int clauses;
    int **array;
    int max;
} CNF2;

graph *graph_init(int vertices);
void add_arc(graph *g, int src, int dest);
void add_edge(graph *g, int src, int dest);
void del_arc(graph *g, int src, int dest);
graph *graph_create();
graph *graph_create_undirected();
void graph_print(graph *g);
void graph_free(graph *g);
graph *create_complete_graph(int num_vertices);
bool topological_sort_dfs(graph *g, int vertex, stack* result, int *visited, struct stack* cycle, int& CycleTop);
int *topological_sort(graph *g);

void strongConnect(graph *g, int v, TarjanInfo *info, int *sccCounter, scc_list *result);

scc_list *tarjanSCC(graph *g);

scc_list *InitSCCList();
void AppendSCCList(list scc,scc_list *scc_list);
void FreeSCCList(scc_list *scc);
void printSCCLis(scc_list *list);

CNF2 *InitCNF2(int count);
CNF2 *GetCNF2(char *str);
void PrintCNF2(CNF2* cnf);
void FreeCNF2(CNF2* cnf);
graph *createImplicationGraph(CNF2 *cnf);
int* Solve2sat(CNF2* cnf);

#endif //COMPUTER_PRACTICE_GRAPH_H
