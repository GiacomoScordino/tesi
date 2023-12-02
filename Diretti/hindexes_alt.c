// nodi e archi sono due array, grafo ha un puntatore a ciascun array
// malloc e realloc su archi e nodi ma se V e E sono min della dimensione effettiva di nodi e archi non funziona
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define V 3600000
#define E 9100000
// 68,349,466 &  1,811,849,342

// struttura di input
typedef struct vicino_struct
{
    int id;
    struct vicino_struct *next;
} vicino;

typedef struct vertex_struct
{
    int id;
    int indegree;
    int outdegree;
    int h_in;
    int h_out;
    int new_h_in;
    int new_h_out;
    vicino *primo_vicino_in;
    vicino *primo_vicino_out;
} vertex;

typedef struct edge_struct
{
    int coreness;
    vertex *a;
    vertex *b;
} edge;

typedef struct graph_struct
{
    int num_edges;
    int num_vertices;
    int len_v;
    int len_e;
    edge *edges;
    vertex *vertices;
} graph;

//************************************************************SETTING GRAPH INPUT************************************************************//

graph *set_input(char *input_path)
{

     //LARGE_INTEGER frequency;        // ticks per second
     //LARGE_INTEGER t1, t2, t3;           // ticks
     //double diff_sec;

    // get ticks per second
    // QueryPerformanceFrequency(&frequency);

    FILE *file = fopen(input_path, "r");

    if (file != NULL)
    {

        graph *grafo = (graph *)malloc(sizeof(graph));
        vertex *v_array = (vertex *)calloc(V, sizeof(vertex));
        edge *e_array = (edge *)calloc(E, sizeof(edge));

        if (grafo == NULL || v_array == NULL || e_array == NULL)
        {
            printf("malloc fallito\n");
            exit(1);
        }

        grafo->len_v = V;
        grafo->vertices = v_array;

        grafo->len_e = E;
        grafo->edges = e_array;

        int len_list_edges = 0;
        int len_list_vertex = 0;
        int c = 0;
        // int n = V;
        // int m = E;
        char line[1000];
        vertex *v_start, *v_end;
        while (fgets(line, sizeof(line), file))
        {
            int start, end;
            if (sscanf(line, "%d %d", &start, &end) == 2)
            {
                if (start != end)
                { // esegui la computazione solo se l'arco non è un loop
                    c++;
                    if (start > len_list_vertex)
                    {
                        len_list_vertex = start;
                    }
                    if (end > len_list_vertex)
                    {
                        len_list_vertex = end;
                    }

                    v_start = &(grafo->vertices[start]);
                    v_end = &(grafo->vertices[end]);

                    if (v_start->id != 0)
                    {
                        v_start->outdegree++;
                    }
                    else
                    {
                        v_start->id = start;
                        v_start->outdegree=1;
                    }

                    if (v_end->id != 0)
                    {
                        v_end->indegree++;
                    }
                    else
                    {
                        v_end->id = end;
                        v_end->indegree=1;
                    }
                    //printf("il nodo %d ha indegree %d\n", v_end->id,v_end->indegree);
                    vicino *vic_out = (vicino *)malloc(sizeof(vicino));
                    vicino *vic_in = (vicino *)malloc(sizeof(vicino));
                    if (vic_out ==NULL || vic_in == NULL)
                    {
                        printf("malloc vicino fallito\n");
                    }

                    vic_out->id = v_end->id;
                    vic_out->next = v_start->primo_vicino_out;
                    v_start->primo_vicino_out = vic_out;

                    vic_in->id = v_start->id;
                    vic_in->next = v_end->primo_vicino_in;
                    v_end->primo_vicino_in = vic_in;

                    grafo->edges[len_list_edges].a = v_start;
                    grafo->edges[len_list_edges].b = v_end;

                    len_list_edges++;

                }
            }
        }
        fclose(file);
        grafo->num_edges = len_list_edges;
        grafo->num_vertices = len_list_vertex;
        for(int i=1; i<=grafo->num_vertices;i++){
            grafo->vertices[i].h_in=grafo->vertices[i].indegree;
            grafo->vertices[i].h_out=grafo->vertices[i].outdegree;
        }
        return grafo;
    }
    else
    {
        printf("il file non esiste\n");
        exit(1);
    }
}
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];  
    int i = (low - 1);      

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] >= pivot) {
            i++; 
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quicksortDesc(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksortDesc(arr, low, pi - 1);
        quicksortDesc(arr, pi + 1, high);
    }
}
int update_h_out(graph* grafo,int k){
    vicino* current = grafo->vertices[k].primo_vicino_out;
    int num_neighbors=1;
    int* lista= (int*)malloc(sizeof(int) * (grafo->vertices[k].outdegree));
    while(current!=NULL){
        lista[num_neighbors++]=grafo->vertices[current->id].h_in;
        current=current->next;
    }
    int temp;
    lista[0]=-1;
    quicksortDesc(lista, 1, num_neighbors - 1);
    printf("lista_h_out:[%d", lista[1]);
    for(int i=2;i<num_neighbors;i++){
        printf(", %d", lista[i]);
    }
    printf("]\n");
    int max_i = 0;
    for (int i = 1; i < num_neighbors; i++) {
        if (lista[i] >= i) {
            max_i = i;
        }
    }
    printf("current h_out: %d\n", max_i);
    free(lista);
    free(current);
    return max_i;
}
int update_h_in(graph* grafo,int k){
    vicino* current = grafo->vertices[k].primo_vicino_in;
    int num_neighbors=1;
    int* lista= (int*)malloc(sizeof(int) * (grafo->vertices[k].indegree));
    while(current!=NULL){
        lista[num_neighbors++]=grafo->vertices[current->id].h_out;
        current=current->next;
    }
    int temp;
    lista[0]=-1;
    quicksortDesc(lista, 1, num_neighbors - 1); 
    printf("lista_h_in:[%d", lista[1]);
    for(int i=2;i<num_neighbors;i++){
        printf(", %d", lista[i]);
    }
    printf("]\n");
    int max_i = 0;
    for (int i = 1; i < num_neighbors; i++) {
        if (lista[i] >= i) {
            max_i = i;
        }
    }
    printf("current h_in: %d\n", max_i);
    free(lista);
    free(current);
    return max_i;
}
void compute_H_indexes(graph *grafo){
    int converged=0;
    int count=1;
    while (converged==0){
        printf("\n-------iterazione %d-------\n",count);
        count++;
        for(int i=1; i<=grafo->num_vertices;i++){
            printf("\nVertice %d\n", i);
            printf("vecchio h_in: %d\n",grafo->vertices[i].h_in);
            grafo->vertices[i].new_h_in = update_h_in(grafo, i);
            printf("vecchio h_out: %d\n",grafo->vertices[i].h_out);
            grafo->vertices[i].new_h_out = update_h_out(grafo, i);
            
        }
        converged=1;
        
        for(int i=1; i<=grafo->num_vertices;i++){
            if(grafo->vertices[i].h_in!=grafo->vertices[i].new_h_in && grafo->vertices[i].h_out!=grafo->vertices[i].new_h_out){
                converged=0;
                printf("il vertice %d non rispetta la condizione di convergenza\n",i);
            }
            grafo->vertices[i].h_in=grafo->vertices[i].new_h_in;
            grafo->vertices[i].h_out=grafo->vertices[i].new_h_out;
        } 
    }
    printf("\ntutti i vertici rispettano la condizione di convergenza\n");
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Please provide a string as a command-line argument.\n");
        return 1; // Exit the program with an error code
    }

    char *input_file = argv[1];


    graph *graph_input = set_input(input_file);
    for (int i=1;i<=graph_input->num_vertices;i++){
        printf("il vertice %d ha h_in di ordine 0 = %d\n", i,  graph_input->vertices[i].h_in);
        printf("il vertice %d ha h_out di ordine 0 = %d\n", i,  graph_input->vertices[i].h_out);
        printf("\n");
    }
    compute_H_indexes(graph_input);

    printf("\nRISULTATO FINALE\n");
    for (int i=1;i<=graph_input->num_vertices;i++){
        printf("il vertice %d ha coreness_in %d\n", i,  graph_input->vertices[i].new_h_in);
        printf("il vertice %d ha coreness_out %d\n", i,  graph_input->vertices[i].new_h_out);
        printf("\n");
    }

  
    return 0;
}
