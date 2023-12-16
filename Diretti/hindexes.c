// nodi e archi sono due array, grafo ha un puntatore a ciascun array
// malloc e realloc su archi e nodi ma se V e E sono min della dimensione effettiva di nodi e archi non funziona
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define V 4000000
#define E 8000000
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
    int degree;
    int indegree;
    int outdegree;
    int coreness;
    int coreness_in;
    int coreness_out;
    int is_in;
    int is_out;
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
                        v_start->degree++;
                    }
                    else
                    {
                        v_start->id = start;
                        v_start->outdegree=1;
                        v_start->degree=1;
                    }

                    if (v_end->id != 0)
                    {
                        v_end->indegree++;
                        v_end->degree++;
                    }
                    else
                    {
                        v_end->id = end;
                        v_end->indegree=1;
                        v_end->degree=1;
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

/*-------------CALCOLO DELLA CORENESS---------------*/
void compute_coreness(graph *grafo)
{
    int n = grafo->num_vertices;
    int md = 0;
    int d,d_in,d_out, i, start, num;
    int v, u, w, du, pu, pw;
    int *deg = malloc(sizeof(int) * (2*n + 1));
    // int* bin = malloc(sizeof(int) * (n+1));
    int *bin = calloc(n + 1, sizeof(int));
    int *pos = malloc(sizeof(int) * (2*n + 1));
    int *vert = malloc(sizeof(int) * (2*n + 1));
    if (deg == NULL || bin == NULL || pos == NULL || vert == NULL)
    {
        printf("malloc compute_coreness fallito\n");
        exit(1);
    }
    for (v = 1; v <= n; v++)
    {
        vertex vertice = grafo->vertices[v];
        d_in = vertice.indegree;
        d_out = vertice.outdegree;

        deg[v] = d_in;
        deg[v+n] = d_out;

        if (d_in > md)
            md = d_in;
        if (d_out > md)
            md = d_out;
        bin[deg[v]]++;
        bin[deg[v+n]]++;
    }
    start = 1;
    for (d = 0; d <= md; d++)
    {
        num = bin[d];
        bin[d] = start;
        start += num;
    }

    for (v = 1; v <= 2*n; v++)
    {
        pos[v] = bin[deg[v]];
        vert[pos[v]] = v;
        bin[deg[v]]++;
    }
    for (d = md; d >= 0; d--)
    {
        bin[d] = bin[d - 1];
    }
    bin[0] = 1;
    int num_vert = 0;
    int a=0,b=0;
    for (i = 1; i <= 2*n; i++)
    {
        a=0;
        b=0;
        num_vert++;
        v = vert[i];
        if(v<=n){
            if(grafo->vertices[v].id != 0) a=1;
        }
        else{
            if(grafo->vertices[v-n].id != 0) b=1;

        }
        if (a==1 || b==1)
        {
            vicino *vi;
             if(v<=n)
                vi = grafo->vertices[v].primo_vicino_in;
            else
                vi = grafo->vertices[v-n].primo_vicino_out;
               
            while (vi!=NULL)
            {
                vertex vic = grafo->vertices[vi->id];
                if(v<=n)
                    u = vic.id+n;
                else 
                    u = vic.id;
                // printf("\t vicino %d\n", vic->id);

                if (deg[u] > deg[v])
                {
                    du = deg[u];
                    pu = pos[u];
                    pw = bin[du];
                    w = vert[pw];

                    if (u != w)
                    {
                        pos[u] = pw;
                        vert[pu] = w;
                        pos[w] = pu;
                        vert[pw] = u;
                    }
                    bin[du]++;
                    deg[u]--;
                }

                vi = vi->next;
            }
        }
        if (v<=n){
            grafo->vertices[v].coreness_in = deg[v];
        }else{
            grafo->vertices[v-n].coreness_out = deg[v];
        }
    }
    for(int l=1; l<=grafo->num_vertices; l++){
        if(grafo->vertices[l].coreness_in>grafo->vertices[l].coreness_out){
            grafo->vertices[l].coreness=grafo->vertices[l].coreness_in;
            grafo->vertices[l].is_in=1;
        }
        if(grafo->vertices[l].coreness_in<grafo->vertices[l].coreness_out){
            grafo->vertices[l].coreness=grafo->vertices[l].coreness_out;
            grafo->vertices[i].is_out=1;
        }
        if(grafo->vertices[l].coreness_in==grafo->vertices[l].coreness_out){
            grafo->vertices[l].coreness=grafo->vertices[l].coreness_out;
            grafo->vertices[l].is_out=1;
            grafo->vertices[l].is_in=1;
        }
    }
    free(vert);
    free(pos);
    free(bin);
    free(deg);
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
    printf("fatto set_input\n");
   
    //compute_H_indexes(graph_input);
    compute_coreness(graph_input);
    printf("\nRISULTATO FINALE\n");
    
    for (int i=1;i<=graph_input->num_vertices;i++){
        printf("il vertice %d ha coreness in %d\n", i,  graph_input->vertices[i].coreness_in);
        printf("il vertice %d ha coreness out %d\n", i,  graph_input->vertices[i].coreness_out);
        printf("il vertice %d ha coreness %d\n", i,  graph_input->vertices[i].coreness);
        printf("\n");
    }   
    return 0;
}
