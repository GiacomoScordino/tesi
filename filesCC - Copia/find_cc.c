// nodi e archi sono due array, grafo ha un puntatore a ciascun array
// malloc e realloc su archi e nodi ma se V e E sono min della dimensione effettiva di nodi e archi non funziona
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define V 1000000   // 150000000
#define E 10000000 // 2000000000
// 68,349,466 &  1,811,849,342

// struttura di output
typedef struct node_tree_struct
{
    int id;
    int coreness;
    int range_c[2];
    int n_numerousness;
    int n_numerousnessL;
    int n_numerousnessR;
    int e_numerousness;
    int n_tm_numerousness;
    int n_tm_numerousnessL;
    int n_tm_numerousnessR;
    int e_tm_numerousness;
    int depth;
    struct node_tree_struct *child;
    struct node_tree_struct *last_child;
    struct node_tree_struct *sibling;
    struct node_tree_struct *prev_sibling;
    struct node_tree_struct *parent;
    struct node_ds_struct *node_set;
} node_tree;

// struttura di supporto
typedef struct node_ds_struct
{
    int id;
    int rank;
    int coreness;
    struct node_ds_struct *parent;
    node_tree *root_tree;
} node_ds;

// struttura di input
typedef struct vicino_struct
{
    int id;
    struct vicino_struct *next;
} vicino;

typedef struct vertex_struct
{
    int id;
    int coreness;
    int degree;
    node_tree *v_tree;
    vicino *primo_vicino;
    char class;
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
    int num_verticesL;
    int num_verticesR;
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
        int len_list_vertexL = 0;
        int len_list_vertexR = 0;
        int c = 0;
        // int n = V;
        // int m = E;

        char line[1000];

        vertex *v_start, *v_end;

       int contaLettura=0;
       int verticiSinistra=0; 
        while (fgets(line, sizeof(line), file))
        {
            if(contaLettura==1){
                char x;
                int y,h;
                sscanf(line,"%c %d %d %d",&x,&y,&verticiSinistra,&h);
            }
            contaLettura++;
            int start, end;
            if (sscanf(line, "%d %d", &start, &end) == 2)
            { 
              end=end+verticiSinistra;
              c++;
              if (start > len_list_vertexL)
              {
                 len_list_vertexL = start;
              }
              if (end > len_list_vertexR)
             {
                  len_list_vertexR = end;
             }

              v_start = &(grafo->vertices[start]);
              v_end = &(grafo->vertices[end]);

            if (v_start->id !=0)
            {
                v_start->degree++;
            }
            else
            {
                 v_start->id = start;
                 v_start->degree = 1;
                 v_start->class='l';
            }

            if (v_end->id != 0)
            {
                 v_end->degree++;
            }
            else
            {
                v_end->id = end;
                v_end->degree = 1;
                v_end->class='r';
             }

            vicino *vic = (vicino *)malloc(sizeof(vicino));
            vicino *vic2 = (vicino *)malloc(sizeof(vicino));
            if (vic == NULL || vic2 == NULL)
            {
                printf("malloc vicino fallito\n");
            }

            vic->id = v_end->id;
            vic->next = v_start->primo_vicino;
            v_start->primo_vicino = vic;

            vic2->id = v_start->id;
            vic2->next = v_end->primo_vicino;
            v_end->primo_vicino = vic2;

            grafo->edges[len_list_edges].a = v_start;
            grafo->edges[len_list_edges].b = v_end;

                len_list_edges++;

                
            }
        }
        fclose(file);
        grafo->num_edges = len_list_edges;
        grafo->num_verticesL = len_list_vertexL;
        grafo->num_verticesR = len_list_vertexR;
        grafo->num_vertices = len_list_vertexR+len_list_vertexL;
        return grafo;
    }
    else
    {
        printf("il file non esiste\n");
        exit(1);
    }
}

//*************************************************************CALCOLO CORENESS*************************************************************//

void compute_coreness(graph *grafo)
{

    int n = grafo->num_vertices;
    int md = 0;
    int d, i, start, num;
    int v, u, w, du, pu, pw;

    int *deg = malloc(sizeof(int) * (n + 1));
    // int* bin = malloc(sizeof(int) * (n+1));
    int *bin = calloc(n + 1, sizeof(int));
    int *pos = malloc(sizeof(int) * (n + 1));
    int *vert = malloc(sizeof(int) * (n + 1));

    if (deg == NULL || bin == NULL || pos == NULL || vert == NULL)
    {
        printf("malloc compute_coreness fallito\n");
        exit(1);
    }

    for (v = 1; v <= n; v++)
    {
        // printf("%d\n", v);
        vertex vertice = grafo->vertices[v];
        d = vertice.degree;

        deg[v] = d;
        if (d > md)
            md = d;
        bin[deg[v]]++;
    }
    /*
        for(d=0; d<=md; d++){
            bin[d]=0;
        }
    */
    /*    for(v=1; v<=n; v++){
            bin[deg[v]]++;
        }
    */
    start = 1;
    for (d = 0; d <= md; d++)
    {
        num = bin[d];
        bin[d] = start;
        start += num;
    }

    for (v = 1; v <= n; v++)
    {
        pos[v] = bin[deg[v]];
        vert[pos[v]] = v;
        bin[deg[v]]++;
        deg[v] = grafo->vertices[v].degree;
    }

    for (d = md; d >= 0; d--)
    {
        bin[d] = bin[d - 1];
    }
    bin[0] = 1;

    // printf("\tFinita precomputazione\n");
    int num_vert = 0;

    // LARGE_INTEGER frequency;        // ticks per second
    // LARGE_INTEGER t1, t2;           // ticks
    // double diff_sec;

    // get ticks per second
    // QueryPerformanceFrequency(&frequency);
    // start timer
    // QueryPerformanceCounter(&t1);

    for (i = 1; i <= n; i++)
    {
        num_vert++;
        v = vert[i];

        if (grafo->vertices[v].id != 0)
        {

            vicino *vi = grafo->vertices[v].primo_vicino;
            for (int j = 0; j < grafo->vertices[v].degree; j++)
            {

                vertex vic = grafo->vertices[vi->id];
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
        // if(num_vert < 7000000){
        /*if(num_vert%1000000==0){
          QueryPerformanceCounter(&t2);
          diff_sec = (t2.QuadPart-t1.QuadPart)/(double)frequency.QuadPart;
          printf("\telaborato vertice %d in %f secondi\n", num_vert, diff_sec);
          QueryPerformanceCounter(&t1);
        }*/
        /*}
        else{
          if(num_vert%1000==0){
            QueryPerformanceCounter(&t2);
            diff_sec = (t2.QuadPart-t1.QuadPart)/(double)frequency.QuadPart;
            printf("\telaborato vertice %d in %f secondi\n", num_vert, diff_sec);
            QueryPerformanceCounter(&t1);
          }
        }*/
        /*printf("Nodo: %d \t", v);
        printf("Coreness: %d\n", deg[v]);*/
        grafo->vertices[v].coreness = deg[v];
    }
    free(vert);
    free(pos);
    free(bin);
    free(deg);
}

//**************************************************ORDINA ARCHI PER CORENESS DESCRESCENTE**************************************************//

int find_min(int a, int b)
{
    if (a <= b)
        return a;
    else
        return b;
}

void ordina_archi(graph *grafo)
{
    int max_c = 0;

    for (int l = 1; l <= grafo->num_vertices; l++)
    {
        if (grafo->vertices[l].coreness > max_c)
            max_c = grafo->vertices[l].coreness;
    }
    /*
        int* aux = (int*)malloc(sizeof(int) * (max_c+1));
        int* bucket = (int*)malloc(sizeof(int) * (max_c+1));
    */
    int *aux = (int *)calloc(max_c + 1, sizeof(int));
    int *bucket = (int *)calloc(max_c + 1, sizeof(int));

    if (aux == NULL || bucket == NULL)
    {
        printf("malloc ordina_archi fallito\n");
    }
    /*
        for(int j=1; j<=max_c; j++){
          aux[j]=0;
        }
    */
    for (int i = 0; i < grafo->num_edges; i++)
    {
        grafo->edges[i].coreness = find_min(grafo->edges[i].a->coreness, grafo->edges[i].b->coreness);
        aux[grafo->edges[i].coreness]++;
    }

    /*for(int j=1; j<=max_c; j++){
      printf("%d %d\n", j, aux[j]);
    }
    printf("\n");*/

    int size = grafo->num_edges;
    /*
        for(int j=max_c; j>=1; j--){
          if(j==max_c){
            bucket[j] = 0;
          }else{
            bucket[j]=bucket[j+1]+aux[j+1];
          }
        }
    */

    for (int j = max_c - 1; j >= 1; j--)
    {
        bucket[j] = bucket[j + 1] + aux[j + 1];
    }
    free(aux);

    /*for(int j=1; j<=max_c; j++){
      printf("%d %d\n", j, bucket[j]);
    }*/

    // printf("Fine assegnazione coreness agli archi\n");

    edge *edges = (edge *)malloc(size * sizeof(edge));

    for (int i = 0; i < size; i++)
    {
        int a = grafo->edges[i].a->id;
        int b = grafo->edges[i].b->id;
        int core_e = grafo->edges[i].coreness;
        int posix = bucket[core_e];

        // printf("arco %d (%d %d), coreness %d\n", i+1, a, b, core_e);

        edges[posix].a = &(grafo->vertices[a]);
        edges[posix].b = &(grafo->vertices[b]);
        edges[posix].coreness = core_e;

        bucket[core_e]++;
    }

    free(grafo->edges);
    free(bucket);
    grafo->edges = edges;

    /*printf("lista archi:\n");
    for(int i=0; i<grafo->num_edges; i++){
        printf("arco %d: (%d,%d), coreness: %d\n", i+1, grafo->edges[i].a->id, grafo->edges[i].b->id, grafo->edges[i].coreness);
    }*/
}

//********************************************************CREA FOGLIE OUTPUTTREE E DS********************************************************//

void makeSetLeaf(vertex *v)
{

    // crea set Disjoint-set: makeset(vrt)
    // node_ds* n_ds = (node_ds*)malloc(sizeof(node_ds));
    node_ds *n_ds = (node_ds *)calloc(1, sizeof(node_ds));
    n_ds->id = v->id;
    // n_ds->rank = 0;
    n_ds->coreness = v->coreness;
    n_ds->parent = n_ds;
    // n_ds->root_tree = NULL;
    // printf("-----------nodo disjoint-set: %d-----------------\n", n_ds->id);

    // crea foglie albero
    // node_tree* n_tree = (node_tree*)malloc(sizeof(node_tree));
    node_tree *n_tree = (node_tree *)calloc(1, sizeof(node_tree));
    n_tree->id = v->id;
    n_tree->node_set = n_ds;
    // n_tree->child = NULL;
    // n_tree->last_child = NULL;
    // n_tree->sibling = NULL;
    // n_tree->prev_sibling = NULL;
    // n_tree->parent = NULL;
    // n_tree->n_numerousness = 0;
    n_tree->coreness = v->coreness;
    // printf("-----------nodo albero: %d-----------------\n", n_tree->id);

    // crea un riferimento tra il vertice e la foglia dell'albero (serve per la find?)
    v->v_tree = n_tree;
    // printf("-----------riferimento vertice: %d-----------------\n", v->v_tree->id);
}

void init(graph *g)
{
    // setta il grafo dato come input: per ogni nodo del grafo costruisci l'albero di output e la struttura di supporto
    for (int i = 1; i <= g->num_vertices; i++)
    {
        makeSetLeaf(&(g->vertices[i]));
    }
}

//**********************************************************COSTRUZIONE OUTPUT TREE**********************************************************//

node_tree *find_min_coreness(node_tree *a, node_tree *b)
{
    if (a->coreness <= b->coreness)
        return a;
    else
        return b;
}

node_tree *find_max_coreness(node_tree *a, node_tree *b)
{
    if (a->coreness <= b->coreness)
        return b;
    else
        return a;
}

node_ds *find(node_ds *n_set)
{
    while (n_set != n_set->parent)
    {
        n_set = n_set->parent;
    }
    return n_set;
}

void merge_ds(node_ds *root_a, node_ds *root_b, node_tree *k_tree)
{
    int rank_a = root_a->rank;
    int rank_b = root_b->rank;

    if (rank_a > rank_b)
    {
        root_b->parent = root_a;
        root_a->root_tree = k_tree;
        /*printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_a->id, root_b->id);
        printf("Disjoint set: %d punta a %d\n", root_a->id, root_a->root_tree->id);*/
    }
    else
    {
        root_a->parent = root_b;
        root_b->root_tree = k_tree;
        /*printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_b->id, root_a->id);
        printf("Disjoint set: %d punta a %d\n", root_b->id, root_b->root_tree->id);*/

        if (rank_a == rank_b)
            root_b->rank = root_b->rank + 1;
    }
}

node_tree *build(graph *g)
{

    // node_tree* root = (node_tree*)malloc(sizeof(node_tree));
    node_tree *root = (node_tree *)calloc(1, sizeof(node_tree));
    // root->id=0;
    // root->coreness=0;
    // root->parent=NULL;

    int size = g->num_vertices;

    // node_tree** array_ninterni = malloc(sizeof(node_tree) * size);
    node_tree **array_ninterni = calloc(size, sizeof(node_tree));
    int num_ninterni = 0;

    int j = 1;
    int c = 0;
    int prevCoreness = g->edges->coreness;

    // printf("-----------------COSTRUZIONE ALBERO OUTPUT----------------\n");
    for (int i = 0; i < g->num_edges; i++)
    {

        c++;
        /*if(c%10000000==0)
          printf("\teseguito arco %d\n", c);
*/
        edge *edg_elm = &(g->edges[i]);

        vertex *a = edg_elm->a;
        vertex *b = edg_elm->b;

        node_tree *a_tree = a->v_tree;
        node_tree *b_tree = b->v_tree;

        node_ds *a_ds = a_tree->node_set;
        node_ds *b_ds = b_tree->node_set;

        node_ds *root_a_ds = find(a_ds);
        node_ds *root_b_ds = find(b_ds);

        node_tree *root_a_tree = root_a_ds->root_tree;
        node_tree *root_b_tree = root_b_ds->root_tree;

        int core_e = edg_elm->coreness;

        if (prevCoreness > core_e)
        {
            j = 1;
        }
        prevCoreness = core_e;

        // printf("\n------------------- (%d,%d) --- %d azione -------------------\n", a->id, b->id, action);

        if (root_a_tree == NULL && root_b_tree == NULL)
        { // situazione iniziale
            /*printf("caso 1\n\n");
            printf("situazione iniziale:\n");
            printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_a_ds->id, a_tree->id);
            printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_b_ds->id, b_tree->id);
            printf("Output tree: %d e %d sono due foglie\n\n", a_tree->id, b_tree->id);*/

            // modifica albero output
            // node_tree* k_tree = (node_tree*)malloc(sizeof(node_tree));
            node_tree *k_tree = (node_tree *)calloc(1, sizeof(node_tree));
            k_tree->id = j + (core_e * 1000);
            k_tree->coreness = core_e;
            // k_tree->sibling = NULL;
            // k_tree->prev_sibling = NULL;
            // k_tree->parent = NULL;

            k_tree->child = a_tree;
            k_tree->last_child = b_tree;
            a_tree->sibling = b_tree;
            b_tree->prev_sibling = a_tree;

            a_tree->parent = k_tree;
            b_tree->parent = k_tree;

            // aggiorna disjoint_set: union() + aggiornamento root_a_tree che ora punta a k_tree
            // printf("Dopo le modifiche:\n");
            merge_ds(root_a_ds, root_b_ds, k_tree);
            // printf("Output tree: %d e' parent di %d e %d\n", k_tree->id, k_tree->child->id, k_tree->child->sibling->id);

            array_ninterni[num_ninterni] = k_tree;
            num_ninterni++;
            j++;
        }

        else if (root_a_tree == NULL && !(root_b_tree == NULL))
        {
            if (core_e < root_b_tree->coreness)
            {
                /*printf("caso 2\n\n");
                printf("situazione iniziale:\n");
                printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_a_ds->id, a_tree->id);
                printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_b_ds->id, b_tree->id);
                printf("Output tree: %d e' una foglia e %d e' la radice temp di %d\n\n", a_tree->id, root_b_tree->id, b_tree->id);*/

                // crea nodo K' parent di a e K
                // node_tree* k_tree = (node_tree*)malloc(sizeof(node_tree));
                node_tree *k_tree = (node_tree *)calloc(1, sizeof(node_tree));
                k_tree->id = j + (core_e * 1000);
                k_tree->coreness = core_e;
                // k_tree->sibling = NULL;
                // k_tree->prev_sibling = NULL;
                // k_tree->parent = NULL;

                k_tree->child = root_b_tree;
                k_tree->last_child = a_tree;
                root_b_tree->sibling = a_tree;
                a_tree->prev_sibling = root_b_tree;

                root_b_tree->parent = k_tree;
                a_tree->parent = k_tree;

                // printf("Dopo le modifiche:\n");
                merge_ds(root_a_ds, root_b_ds, k_tree);
                // printf("Output tree: %d e' parent di %d e %d\n", k_tree->id, k_tree->child->id, k_tree->child->sibling->id);

                array_ninterni[num_ninterni] = k_tree;
                num_ninterni++;
                j++;
            }
            else
            {
                // nodo K è parent di a
                /*printf("caso 3\n\n");
                printf("situazione iniziale:\n");
                printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_a_ds->id, a_tree->id);
                printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_b_ds->id, b_tree->id);
                printf("Output tree: %d e' una foglia e %d e' la radice temp di %d\n\n", a_tree->id, root_b_tree->id, b_tree->id);

                printf("Dopo le modifiche:\n");
                printf("Output tree: %d e' parent di: ", root_b_tree->id);*/

                node_tree *last_child = root_b_tree->last_child;
                last_child->sibling = a_tree;
                a_tree->prev_sibling = last_child;
                root_b_tree->last_child = a_tree;

                a_tree->parent = root_b_tree;

                // printf("e di %d\n", last_child->sibling->id);
                merge_ds(root_a_ds, root_b_ds, root_b_tree);
            }
        }

        else if (!(root_a_tree == NULL) && root_b_tree == NULL)
        {
            if (core_e < root_a_tree->coreness)
            {
                /*printf("caso 4\n\n");
                printf("situazione iniziale:\n");
                printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_a_ds->id, a_tree->id);
                printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_b_ds->id, b_tree->id);
                printf("Output tree: %d e' la radice temp di %d e %d e' una foglia\n\n", root_a_tree->id, a_tree->id, b_tree->id);*/

                // crea nodo K' parent di b e K
                // node_tree* k_tree = (node_tree*)malloc(sizeof(node_tree));
                node_tree *k_tree = (node_tree *)calloc(1, sizeof(node_tree));
                k_tree->id = j + (core_e * 1000);
                k_tree->coreness = core_e;
                // k_tree->sibling = NULL;
                // k_tree->prev_sibling = NULL;
                // k_tree->parent = NULL;

                k_tree->child = root_a_tree;
                k_tree->last_child = b_tree;
                root_a_tree->sibling = b_tree;
                b_tree->prev_sibling = root_a_tree;

                root_a_tree->parent = k_tree;
                b_tree->parent = k_tree;

                // printf("Dopo le modifiche:\n");
                merge_ds(root_a_ds, root_b_ds, k_tree);
                // printf("Output tree: %d e' parent di %d e %d\n", k_tree->id, k_tree->child->id, k_tree->child->sibling->id);

                array_ninterni[num_ninterni] = k_tree;
                num_ninterni++;
                j++;
            }
            else
            {
                // nodo K è parent di b
                /*printf("caso 5\n\n");
                printf("situazione iniziale:\n");
                printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_a_ds->id, a_tree->id);
                printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_b_ds->id, b_tree->id);
                printf("Output tree: %d e' la radice temp di %d e %d e' una foglia\n\n", root_a_tree->id, a_tree->id, b_tree->id);

                printf("Dopo le modifiche:\n");
                printf("Output tree: %d e' parent di: ", root_a_tree->id);*/

                node_tree *last_child = root_a_tree->last_child;
                last_child->sibling = b_tree;
                b_tree->prev_sibling = last_child;
                root_a_tree->last_child = b_tree;

                b_tree->parent = root_a_tree;

                // printf("e di %d\n", last_child->sibling->id);
                merge_ds(root_a_ds, root_b_ds, root_a_tree);
            }
        }

        else
        {
            if (root_a_tree == root_b_tree)
            {
                /*printf("caso 6\n\n");
                //non fare nulla
                printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_a_ds->id, a_tree->id);
                printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_b_ds->id, b_tree->id);
                printf("Output tree: %d e' la radice temp di %d e di %d\n\n", root_a_tree->id, a_tree->id, b_tree->id);*/
            }
            else
            {
                node_tree *root_min_tree = find_min_coreness(root_a_tree, root_b_tree);
                node_tree *root_max_tree = find_max_coreness(root_a_tree, root_b_tree);

                /*printf("caso 7\n\n");
                printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_a_ds->id, a_tree->id);
                printf("Disjoint set: %d e' il nodo di riferimento per %d\n", root_b_ds->id, b_tree->id);
                printf("Output tree: %d e' la radice temp di %d e %d e' la radice temp di %d\n\n", root_a_tree->id, a_tree->id, root_b_tree->id, b_tree->id);

                printf("Dopo le modifiche:\n");
                printf("Output tree: %d e' parent di: ", root_min_tree->id);*/

                node_tree *last_child = root_min_tree->last_child;
                last_child->sibling = root_max_tree;
                root_max_tree->prev_sibling = last_child;
                root_min_tree->last_child = root_max_tree;

                root_max_tree->parent = root_min_tree;

                // printf("e di %d\n", last_child->sibling->id);
                merge_ds(root_a_ds, root_b_ds, root_min_tree);
            }
        }
        // action++;
    }

    // collega chi non ha un parent alla radice
    int primo = 0;
    node_tree *fratello;
    for (int i = 0; i < num_ninterni; i++)
    {
        if (array_ninterni[i]->parent == NULL)
        {
            if (primo == 0)
            {
                root->child = array_ninterni[i];
                root->child->prev_sibling = NULL;
                array_ninterni[i]->parent = root;
                primo = 1;
                fratello = array_ninterni[i];
            }
            else
            {
                array_ninterni[i]->parent = root;
                fratello->sibling = array_ninterni[i];
                array_ninterni[i]->prev_sibling = fratello;
                fratello = array_ninterni[i];
            }
        }
        else
        {
        }
    }
    free(array_ninterni);
    // printf("----------------------FINE COSTRUZIONE------------------------\n\n");

    return root;
}

//****************************************************************SISTEMA OUTPUT TREE****************************************************************//

void action(node_tree *nodo /*, node_tree* parent, node_tree* first_child*/)
{

    node_tree *n_child = nodo->child;
    node_tree *n_parent = nodo->parent;

    // 1. eliminare nodo dai figli di parent
    if (nodo->prev_sibling == NULL)
    {
        n_parent->child = nodo->sibling;
        nodo->sibling->prev_sibling = NULL;
    }
    else if (nodo->sibling == NULL)
    {
        n_parent->last_child = nodo->prev_sibling;
        nodo->prev_sibling->sibling = NULL;
    }
    else
    {
        node_tree *prev_sibling = nodo->prev_sibling;
        node_tree *next_sibling = nodo->sibling;
        prev_sibling->sibling = next_sibling;
        next_sibling->prev_sibling = prev_sibling;
    }

    /*//1. eliminare nodo dai figli di parent
    if(first_child == nodo){
      if(first_child->sibling==NULL){
        parent->child=NULL;
        parent->last_child=NULL;
      }else{
        parent->child = first_child->sibling;
      }
    }else{
      node_tree* temp = first_child;
      while(temp->sibling != NULL){
        if(temp->sibling==nodo){
          if(temp->sibling->sibling == NULL){
            temp->sibling=NULL;
            parent->last_child=temp;
          }else{
            temp->sibling = temp->sibling->sibling;
          }
          break;
        }
        temp = temp->sibling;
      }
    }*/

    // 2. assegnare ai figli di nodo il parent parent
    while (n_child != NULL)
    {
        n_child->parent = n_parent;
        n_child = n_child->sibling;
    }

    // unire i figli
    if (n_parent->child != NULL)
    {
        n_parent->last_child->sibling = nodo->child;
        n_parent->last_child = nodo->last_child;
    }
    else
    {
        n_parent->child = nodo->child;
        n_parent->last_child = nodo->last_child;
    }
}

void sistema(node_tree *root, int len)
{

    node_tree *nodo = root->child;
    int size = len * 2;
    node_tree **temp = malloc(sizeof(node_tree) * size);
    node_tree **array = malloc(sizeof(node_tree) * len);

    if (temp == NULL || array == NULL)
    {
        printf("malloc sistema fallito\n");
    }

    int len_t = 0;
    int len_a = 0;
    int i = 0;

    while (1)
    {
        /*
        if(nodo->coreness == 6){
            node_tree* figlio = nodo->child;
            int count = 0;
            while(figlio != NULL){
                count++;
                figlio = figlio->sibling;
            }
            printf("nodo %d, figli %d\n", nodo->id, count);
            figlio = nodo->child;
            while(figlio != NULL){
                printf("\t\tnodo %d\n", figlio->id);
                figlio = figlio->sibling;
            }
        }*/

        if (nodo->sibling != NULL)
        {
            temp[len_t] = nodo->sibling;
            len_t++;
        }
        if (nodo->child != NULL)
        {
            nodo->range_c[0] = nodo->coreness;
            nodo->range_c[1] = nodo->parent->coreness + 1;
            temp[len_t] = nodo->child;
            len_t++;
        }
        else
        {
            free(nodo->node_set);
        }

        if (nodo->child != NULL && nodo->coreness == nodo->parent->coreness)
        {
            array[len_a] = nodo;
            len_a++;
        }

        if (len_t > i)
        {
            nodo = temp[i];
            i++;
        }
        else
            break;
    }
    free(temp);

    printf("\tnodi da sistemare %d\n", len_a);

    for (int j = 0; j < len_a; j++)
    {
        // if(j%100000==0 && j!=0)
        // printf("\t%d\n", j);
        // action(array[j], array[j]->parent, array[j]->parent->child);
        action(array[j]);
    }
    free(array);
}

//****************************************************************PRIMITIVE****************************************************************//

// grafo input

void num_vertici_archi(graph *grafo)
{
    printf("Il numero di archi e' %d\n", grafo->num_edges);
    printf("Il numero di nodi e' %d\n", grafo->num_vertices);
}

void stampa_nodi(graph *grafo)
{
    int len_list_vertex = grafo->num_vertices;
    printf("\nLista nodi:\n");
    for (int i = 1; i <= len_list_vertex; i++)
    {
        printf("vertice: %d\t", grafo->vertices[i].id);
        printf("coreness: %d\t", grafo->vertices[i].coreness);
        printf("grado: %d\t", grafo->vertices[i].degree);
        printf("vicini: ");
        vicino *vic = grafo->vertices[i].primo_vicino;
        for (int j = 0; j < grafo->vertices[i].degree; j++)
        {
            printf("%d ", vic->id);
            vic = vic->next;
        }
        printf("\n");
    }
}

void stampa_archi(graph *grafo)
{
    printf("Lista archi:\n");
    edge *temp_e = grafo->edges;
    int len_list_edges = grafo->num_edges;
    for (int i = 0; i < len_list_edges; i++)
    {
        printf("arco: %d, %d\tcoreness: %d\n", temp_e[i].a->id, temp_e[i].b->id, temp_e[i].coreness);
    }
}

// albero output

void printTabs(int count)
{
    for (int i = 0; i < count; i++)
    {
        putchar('\t');
    }
}

void print(node_tree *node, int level)
{
    while (node != NULL)
    {

        if (node->child == NULL)
        {
            printTabs(level);
            printf("Node: %d\n", node->id);
        }
        else
        {
            printTabs(level);
            printf("Node: %d, [%d,%d], [n:%d,%d - e:%d,%d]\n", node->id, node->range_c[0], node->range_c[1], node->n_numerousness, node->n_tm_numerousness, node->e_numerousness, node->e_tm_numerousness);

            printTabs(level);
            printf("Children:\n");
            print(node->child, level + 1);
        }
        node = node->sibling;
    }
}

void print_cc(node_tree *node, int level)
{
    while (node != NULL)
    {
        if (node->child != NULL)
        {
            printTabs(level);
            printf("Node: %d, [%d,%d], [n:%d,%d - e:%d,%d]\n", node->id, node->range_c[0], node->range_c[1], node->n_numerousness, node->n_tm_numerousness, node->e_numerousness, node->e_tm_numerousness);

            printTabs(level);
            printf("Children:\n");
            print_cc(node->child, level + 1);
        }
        node = node->sibling;
    }
}

void print_tree_recursive(node_tree *root, int level)
{
    printf("Root: %d, [%d,%d], [n:%d,%d - e:%d,%d]\n", root->id, root->range_c[0], root->range_c[1], root->n_numerousness, root->n_tm_numerousness, root->e_numerousness, root->e_tm_numerousness);
    printf("Children:\n");
    node_tree *node = root->child;
    print(node, level + 1);
}

void print_tree_cc_recursive(node_tree *root, int level)
{
    printf("Root: %d, [%d,%d], [n:%d,%d - e:%d,%d]\n", root->id, root->range_c[0], root->range_c[1], root->n_numerousness, root->n_tm_numerousness, root->e_numerousness, root->e_tm_numerousness);
    printf("Children:\n");
    node_tree *node = root->child;
    print_cc(node, level + 1);
}

void lv_coreness_max_min(graph *grafo)
{
    int val = grafo->edges->coreness;
    printf("Il livello massimo di coreness e' %d\n", val);

    for (int i = 1; i <= grafo->num_vertices; i++)
    {
        if (grafo->vertices[i].coreness < val)
            val = grafo->vertices[i].coreness;
    }
    printf("Il livello minimo di coreness e' %d\n", val);
}

int *quante_componenti(graph *grafo, node_tree *root, int level)
{

    int num_comp = 0;
    int num_vert = 0;
    int *result = malloc(sizeof(int) * 2);

    node_tree *nodo = root->child;

    node_tree **temp = malloc(sizeof(node_tree) * V);
    int len = 0;

    // printf("\nCoreness: %d\n", level);

    int i = 0;

    int coreness_max = grafo->edges->coreness;

    if (coreness_max < level)
    {
        printf("Il livello di coreness e' troppo alto\n");
        printf("0 componenti connesse\n");
        return 0;
    }

    while (1)
    {

        if (nodo->sibling != NULL)
        {
            temp[len] = nodo->sibling;
            len++;
        }
        if (nodo->child != NULL)
        {
            temp[len] = nodo->child;
            len++;

            if (nodo->parent->coreness != nodo->coreness)
            {
                if (nodo->coreness == level)
                {
                    num_comp++;
                    // printf("\nId %d componente: %d\n", num_comp, nodo->id);
                    // printf("Coreness: %d\n", nodo->coreness);
                    // printf("Nodi presenti:\n");
                    // print(nodo->child, 1);
                    // printf("\n");
                }

                else if (nodo->parent->coreness < level && nodo->coreness > level)
                {
                    num_comp++;
                    // printf("\nId %d componente: %d\n", num_comp, nodo->id);
                    // printf("Coreness: %d\n", nodo->coreness);
                    // printf("Nodi presenti:\n");
                    // print(nodo->child, 1);
                    // printf("\n");
                }
            }
        }
        else
        {
            if (nodo->coreness >= level)
            {
                num_vert++;
            }
        }

        if (len > i)
        {
            nodo = temp[i];
            i++;
        }
        else
        {
            result[0] = num_comp;
            result[1] = num_vert;
            free(temp);
            return result;
        }
    }
}

void quante_componenti_ogni_val(graph *grafo, node_tree *root)
{
    /*int max_k = grafo->edges->coreness;

    for(int i=max_k; i>=0; i--){
        quante_componenti(grafo, root, i);
        printf("----------------------------------------------\n");
        printf("\n");
    }*/
    int max_k = grafo->edges->coreness;
    int *result = malloc(sizeof(int) * 2);
    int num_comp = 0;
    int num_vert = 0;
    int temp = 0;

    for (int i = max_k; i > 0; i--)
    {
        result = quante_componenti(grafo, root, i);
        num_comp = result[0];
        num_vert = result[1];
        // printf("%d-%d\n", temp, num_comp);
        if (num_comp != temp)
        {
            printf("coreness: %d, numero componenti connesse: %d, numero vertici: %d\n", i, num_comp, num_vert);
            printf("----------------------------------------------\n");
            printf("\n");
            temp = num_comp;
        }
    }
}

void trova_id(graph *grafo, node_tree *root, int id, int level)
{

    node_tree *nodo = grafo->vertices[id].v_tree;
    node_tree *parent = nodo->parent;
    int coreness_max = grafo->edges->coreness;

    if (level > coreness_max)
    {
        printf("Livello coreness troppo alto\n");
        return;
    }
    if (level > nodo->coreness)
    {
        printf("il nodo %d non esiste per il livello di coreness %d\n", nodo->id, level);
        return;
    }

    while (1)
    {
        if ((parent->coreness == level) || ((parent->coreness > level) && (parent->parent->coreness < level)))
        {
            printf("L'id della componente connessa a cui appartiene il vertice %d al livello di coreness %d e' %d\n", id, level, parent->id);
            break;
        }
        if (parent == root)
        {
            printf("errore");
            break;
        }
        parent = parent->parent;
    }
}

node_tree *trova_componente(node_tree *root, int id)
{

    node_tree *nodo = root->child;

    node_tree **temp = malloc(sizeof(node_tree) * V);
    int len = 0;
    int i = 0;

    while (1)
    {

        if ((nodo->id == id) && (nodo->child != NULL))
        {
            return nodo;
        }

        if (nodo->sibling != NULL)
        {
            temp[len] = nodo->sibling;
            len++;
        }
        if (nodo->child != NULL)
        {
            temp[len] = nodo->child;
            len++;
        }

        if (len > i)
        {
            nodo = temp[i];
            i++;
        }
        else
        {
            printf("componente non trovata\n");
            return root;
        }
    }
    free(temp);
}

void trova_nodi(node_tree *root, int id)
{

    node_tree *nodo = trova_componente(root, id);
    if (nodo != root)
    {
        printf("Nodi appartenenti alla componente connessa %d sono:\n", id);
        print(nodo->child, 1);
    }
}

void componenti_k_kp1(graph *grafo, node_tree *root, int id)
{

    node_tree *nodo = trova_componente(root, id);

    if (nodo == root)
        return;

    int k = nodo->coreness;

    printf("Livello di coreness %d\n", k);
    printf("\tId componente: %d\n", id);
    // printf("\tNodi presenti:\n");
    // print(nodo->child,2);
    printf("\n");

    if (nodo->coreness == grafo->edges->coreness)
    {
        printf("Al livello di coreness %d non esiste nessuna componente connessa\n", k + 1);
        return;
    }

    node_tree **figli = malloc(sizeof(node_tree) * V);
    int len = 0;

    node_tree *figlio = nodo->child;

    while (figlio->sibling != NULL)
    {
        if (figlio->child != NULL && figlio->coreness != figlio->parent->coreness)
        {
            figli[len] = figlio;
            len++;
        }
        figlio = figlio->sibling;
    }
    if (figlio->child != NULL && figlio->coreness != figlio->parent->coreness)
    {
        figli[len] = figlio;
        len++;
    }

    printf("len: %d\n", len);

    if (len == 0)
    {
        printf("Livello di coreness %d\n", k + 1);
        printf("Componente %d scompare\n", id);
    }
    else if (len == 1)
    {
        printf("Livello di coreness %d\n", k + 1);
        if (figli[0]->coreness == k + 1)
        {
            printf("Diminuiscono i nodi presenti\n");
            printf("\tId componente: %d\n", figli[0]->id);
            // printf("\tNodi presenti:\n");
            // print(figli[0]->child,2);
            printf("\n");
        }
        else
        {
            printf("Componente %d al livello di coreness %d resta la stessa\n", id, k + 1);
        }
    }
    else if (len >= 2)
    {
        printf("Livello di coreness %d\n", k + 1);
        printf("Componente %d si divide nelle seguenti componenti:\n", id);
        for (int i = 0; i < len; i++)
        {
            printf("\tComponente: %d\n", figli[i]->id);
            // printf("\tNodi presenti:\n");
            // print(figli[i]->child,2);
        }
    }
    free(figli);
}

void componenti_k_km1(node_tree *root, int id)
{

    node_tree *nodo = trova_componente(root, id);
    int k = nodo->coreness;

    if (nodo == root)
        return;

    printf("Livello di coreness %d\n", nodo->coreness);
    printf("\tId componente: %d\n", id);
    // printf("\tNodi presenti:\n");
    // print(nodo->child,2);
    printf("\n");

    node_tree *parent = nodo->parent;

    if (parent->coreness == k - 1 && parent->coreness != 0)
    {

        node_tree **figli = malloc(sizeof(node_tree) * V);
        int len = 0;

        node_tree *figlio = parent->child;

        while (figlio->sibling != NULL)
        {
            if (figlio->child != NULL && figlio->coreness != figlio->parent->coreness)
            {
                figli[len] = figlio;
                len++;
            }
            figlio = figlio->sibling;
        }
        if (figlio->child != NULL && figlio->coreness != figlio->parent->coreness)
        {
            figli[len] = figlio;
            len++;
        }

        if (len == 1)
        {
            printf("Livello di coreness %d:\n", k - 1);
            printf("Aumentano i nodi presenti\n");
            printf("\tId componente: %d\n", parent->id);
            // printf("\tNodi presenti:\n");
            // print(parent->child,2);
            printf("\n");
        }
        else if (len >= 2)
        {
            printf("Livello di coreness %d:\n", k - 1);
            printf("Componente %d si fonde con le seguenti componenti di livello %d e forma la componente %d:\n", id, k, parent->id);
            for (int i = 0; i < len; i++)
            {
                if (figli[i]->id != id)
                {
                    printf("\tId componente: %d\n", figli[i]->id);
                    // printf("\tNodi presenti:\n");
                    // print(figli[i]->child,2);
                }
            }
            printf("\n");
            // printf("\tNodi presenti nella componente %d a livello %d:\n", parent->id, k-1);
            // print(parent->child,2);
        }
        free(figli);
    }
    else
    {
        printf("Livello di coreness %d\n", k - 1);
        printf("Componente %d al livello di coreness %d resta la stessa\n", id, k - 1);
    }
}

node_tree *max_coreness_comune(graph *grafo, int id_a, int id_b)
{

    int found_a = 0;
    int found_b = 0;

    vertex a;
    vertex b;

    node_tree *lca = NULL;

    if (grafo->vertices[id_a].id == id_a)
    {
        a = grafo->vertices[id_a];
        found_a = 1;
    }

    if (grafo->vertices[id_b].id == id_b)
    {
        b = grafo->vertices[id_b];
        found_b = 1;
    }

    if (found_a == 0 || found_b == 0)
    {
        printf("id non valido\n");
        exit(3);
    }

    node_tree *nodo_a = a.v_tree;
    node_tree *nodo_b = b.v_tree;

    node_tree *nodo_max_coreness = find_max_coreness(nodo_a, nodo_b);

    for (int j = 0; j <= nodo_max_coreness->coreness; j++)
    {
        if (nodo_a->parent->id == nodo_b->parent->id)
        {
            lca = nodo_a->parent;
            // printf("\nil massimo livello di coreness per cui i due vertici appartengono alla stessa componente connessa e' %d\n", nodo_a->parent->coreness);
            break;
        }
        if (nodo_a->parent->coreness < nodo_b->parent->coreness)
            nodo_b = nodo_b->parent;

        else if (nodo_b->parent->coreness < nodo_a->parent->coreness)
            nodo_a = nodo_a->parent;

        else
        {
            nodo_a = nodo_a->parent;
            nodo_b = nodo_b->parent;
        }
    }
    return lca;
}

void trova_coreness(graph *grafo, int id_a)
{
    printf("La coreness max del vertice %d e' %d\n", id_a, grafo->vertices[id_a].coreness);
}

void writeTabs(FILE *fp, int count)
{
    for (int i = 0; i < count; i++)
    {
        fprintf(fp, "\t");
    }
}

void write(node_tree *node, int level, FILE *fp)
{
    while (node && node->child != NULL)
    {
        writeTabs(fp, level);
        fprintf(fp, "{\"Node\": %d,\n", node->id);
        writeTabs(fp, level);
        fprintf(fp, " \"Coreness\": %d,\n", node->coreness);
        writeTabs(fp, level);
        fprintf(fp, " \"minCoreness\":  %d,\n", node->range_c[1]);
        writeTabs(fp, level);
        fprintf(fp, " \"maxCoreness\": %d,\n", node->range_c[0]);
        writeTabs(fp, level);
        fprintf(fp, " \"NumeroNodi\": %d,\n", node->n_tm_numerousness);
        writeTabs(fp, level);
        fprintf(fp, " \"NumeroArchi\": %d,\n", node->e_tm_numerousness);

        if (node->child != NULL)
        {
            writeTabs(fp, level);
            fprintf(fp, " \"Children\": [\n");
            write(node->child, level + 1, fp);
            writeTabs(fp, level);
            fprintf(fp, " ]\n");
            writeTabs(fp, level);
        }

        if (node->sibling && node->sibling->child)
            fprintf(fp, "},\n");
        else
            fprintf(fp, "}\n");
        node = node->sibling;
    }
}

void json_tree_recursive(node_tree *root, int level, FILE *fp)
{
    fprintf(fp, "\"Root\": %d,\n", root->id);
    fprintf(fp, " \"NumeroNodi\": %d,\n", 0);  // oppure grafo->num_vertices
    fprintf(fp, " \"NumeroArchi\": %d,\n", 0); // oppure grafo->num_edges
    fprintf(fp, "\"Children\": [\n");
    node_tree *child = root->child;
    int hoStampatoUnFiglio = 0;
    while (child != NULL)
    {
        if (child->child != NULL)
        {
            write(child, level + 1, fp);
            hoStampatoUnFiglio = 1;
        }
        child = child->sibling;
        if (child != NULL && child->child != NULL && hoStampatoUnFiglio)
        {
            fprintf(fp, ",\n");
        }
    }
    /*
    node_tree* node = root->child;
    write(node, level+1, fp);
    */
    fprintf(fp, " ]\n");
}

void output_json(node_tree *root, int level)
{
    FILE *fp;
    fp = fopen("output_complete.json", "w");
    fprintf(fp, "{");
    json_tree_recursive(root, level, fp);
    fprintf(fp, "}");
    fclose(fp);
}

void write_cc(node_tree *node, int level, FILE *fp)
{
    //if (node->child != NULL)
    //{
        writeTabs(fp, level);
        fprintf(fp, "{\"Node\": %d,\n", node->id);
        writeTabs(fp, level);
        fprintf(fp, " \"Coreness\": %d,\n", node->coreness);
        writeTabs(fp, level);
        fprintf(fp, " \"minCoreness\":  %d,\n", node->range_c[1]);
        writeTabs(fp, level);
        fprintf(fp, " \"maxCoreness\": %d,\n", node->range_c[0]);
        writeTabs(fp, level);
        fprintf(fp, " \"NumeroNodi\": %d,\n", node->n_tm_numerousness);
        writeTabs(fp, level);
        fprintf(fp, " \"NumeroNodiR\": %d,\n", node->n_tm_numerousnessR);
        writeTabs(fp, level);
        fprintf(fp, " \"NumeroNodiL\": %d,\n", node->n_tm_numerousnessL);
        // if(node->n_tm_numerousness < 0) printf("%d\n",node->id);
        writeTabs(fp, level);
        fprintf(fp, " \"NumeroArchi\": %d,\n", node->e_tm_numerousness);
        writeTabs(fp, level);
        fprintf(fp, " \"children\": [\n");
        node_tree *child = node->child;
        int hoStampatoUnFiglio = 0;
        while (child != NULL)
        {
            if (child->child != NULL)
            {
                write_cc(child, level + 1, fp);
                hoStampatoUnFiglio = 1;
            }
            child = child->sibling;
            if (child != NULL && child->child != NULL && hoStampatoUnFiglio)
            {
                fprintf(fp, ",\n");
            }
        }
        writeTabs(fp, level);
        fprintf(fp, " ]}");
    //}
}

void json_tree_recursive_cc(node_tree *root, int level, FILE *fp)
{
    fprintf(fp, "\"Node\": %d,\n", root->id);
    fprintf(fp, "\"Coreness\": %d,\n", 0);
    // fprintf(fp," \"NumeroNodi\": %d,\n", grafo->num_vertices);
    // fprintf(fp," \"NumeroArchi\": %d,\n", grafo->num_edges);
    fprintf(fp, "\"NumeroNodi\": %d,\n", root->n_tm_numerousness);
    fprintf(fp, "\"NumeroNodiL\": %d,\n", root->n_tm_numerousnessL);
    fprintf(fp, "\"NumeroNodiR\": %d,\n", root->n_tm_numerousnessR);
    // if(root->n_tm_numerousness < 0) printf("%d\n", root->id);
    fprintf(fp, "\"NumeroArchi\": 0,\n");
    fprintf(fp, "\"minCoreness\": 0,\n");
    fprintf(fp, "\"maxCoreness\": 0,\n");
    fprintf(fp, "\"children\": [\n");
    node_tree *child = root->child;
    int hoStampatoUnFiglio = 0;
    while (child != NULL)
    {
        if (child->child != NULL)
        {
            write_cc(child, level + 1, fp);
            hoStampatoUnFiglio = 1;
        }
        child = child->sibling;
        if (child != NULL && child->child != NULL && hoStampatoUnFiglio)
        {
            fprintf(fp, ",\n");
        }
    }
    /*
    node_tree* node = root->child;
    write_cc(node, level+1, fp);
    */
    fprintf(fp, " ]\n");
}

void output_json_cc(node_tree *root, int level, char *input_file)
{
    char *output_path = malloc(sizeof(char) * 300);
    strncpy(output_path, input_file, 299);
    // find pointer to last '/' in string
    char *lastslash = strrchr(output_path, '\\');
    if (lastslash)            // if found
        *(lastslash + 1) = 0; //   terminate the string right after the '/'

    output_path = strncat(output_path, "\\output.json", 280);
    printf("Sto salvando in %s", output_path);
    FILE *fp;
    fp = fopen(output_path, "w");
    fprintf(fp, "{");
    json_tree_recursive_cc(root, level, fp);
    fprintf(fp, "}");
    fclose(fp);
}

int calcola_num_n_interni(node_tree *root, int len)
{
    node_tree *nodo = root->child;
    int size = len * 2;
    node_tree **temp = malloc(sizeof(node_tree) * size);

    if (temp == NULL)
    {
        printf("malloc sistema fallito\n");
    }

    int len_t = 0;
    int i = 0;
    int result = 0;

    while (1)
    {

        if (nodo->sibling != NULL)
        {
            temp[len_t] = nodo->sibling;
            len_t++;
        }
        if (nodo->child != NULL)
        {
            result++;
            temp[len_t] = nodo->child;
            len_t++;
        }

        if (len_t > i)
        {
            nodo = temp[i];
            i++;
        }
        else
            break;
    }
    free(temp);

    return result;
}

int calcola_grado_massimo(graph *grafo)
{
    int max = 0;
    int deg;
    for (int i = 0; i < grafo->num_vertices; i++)
    {
        deg = grafo->vertices[i].degree;
        if (deg > max)
        {
            max = deg;
        }
    }
    return max;
}

// Definizione per evitare warnings
void compatta(node_tree *root, int len);

void primitive(graph *grafo, node_tree *root, char *input_file)
{
    while (1)
    {
        int var;

        printf("\n------------------------------------------------------------PRIMITIVE-----------------------------------------------------------------\n\n");
        printf("Per il grafo di input:\n");
        printf("\t1 - Stampa il numero di vertici e il numero di archi del grafo di input\n");
        printf("\t2 - Stampa la lista di nodi (con info: vicini e coreness) del grafo di input\n");
        printf("\t3 - Stampa la lista di archi (con info: coreness) del grafo di input\n");
        printf("\t4 - Dato l'id di un vertice stampa la sua coreness\n");
        printf("\t5 - Stampa il valore di coreness piu' alto e piu' basso\n");
        printf("\nPer l'albero di output:\n");
        printf("\t6 - Stampa l'albero di output\n");              // ricorsiva
        printf("\t7 - Stampa l'albero di output senza foglie\n"); // ricorsiva
        printf("\t8 - Inserire un livello di coreness per determinare il numero di componenti connesse e il numero dei vertici\n");
        printf("\t9 - Stampa per ogni valore di coreness il numero delle componenti connesse e il numero dei vertici\n");
        printf("\t10 - Inserire un vertice e un livello di coreness per trovare l'id della componente connessa a cui appartiene\n");
        printf("\t11 - Inserire l'id di una componente connessa per stampare tutti i nodi che appartengono ad essa\n");
        printf("\t12 - Inserire l'id di una componente connessa, al livello k+1...\n");
        printf("\t\t resta la stessa?\n");
        printf("\t\t diminuiscono i nodi?\n");
        printf("\t\t si divide in diverse componenti connesse?\n");
        printf("\t\t scompare?\n");
        printf("\t13 - Inserire l'id di una componente connessa, al livello k-1...\n");
        printf("\t\t resta la stessa?\n");
        printf("\t\t si aggiungono nodi?\n");
        printf("\t\t si fonde con altre componenti connesse?\n");
        printf("\t14 - Calcolare il livello di coreness per il quale 2 vertici appartengono alla stessa componente connessa\n");
        printf("\n\t15 - Compatta l'albero di output\n");
        printf("\t16 - Esporta il json dell'albero di output\n");              // ricorsiva
        printf("\t17 - Esporta il json dell'albero di output senza foglie\n"); // ricorsiva
        printf("\t18 - Calcola il numero di nodi interni\n");                  // ricorsiva
        printf("\t19 - Calcola il grado massimo\n");
        printf("\nPer terminare:\n");
        printf("\t20 - Exit\n");
        printf("\nDigitare un valore:\t");
        scanf("%d", &var);
        printf("\n");

        if (var == 1)
        {
            num_vertici_archi(grafo);
        }
        else if (var == 2)
        {
            stampa_nodi(grafo);
        }
        else if (var == 3)
        {
            stampa_archi(grafo);
        }
        else if (var == 4)
        {
            int id_a;
            printf("Inserisci l'id del vertice per stampare il suo livello max di coreness:\n");
            scanf("%d", &id_a);
            trova_coreness(grafo, id_a);
        }
        else if (var == 5)
        {
            lv_coreness_max_min(grafo);
        }
        else if (var == 6)
        {
            print_tree_recursive(root, 0);
        }
        else if (var == 7)
        {
            print_tree_cc_recursive(root, 0);
        }
        else if (var == 8)
        {
            int level;
            int *result = malloc(sizeof(int) * 2);
            printf("Inserisci il livello di coreness: ");
            scanf("%d", &level);
            printf("\n");
            result = quante_componenti(grafo, root, level);
            printf("Il numero delle componenti connesse e' %d\n", result[0]);
            printf("Il numero totale dei vertici coinvolti e' %d\n", result[1]);
        }
        else if (var == 9)
        {
            quante_componenti_ogni_val(grafo, root);
        }
        else if (var == 10)
        {
            int id;
            int level;
            printf("Inserisci l'id del vertice: ");
            scanf("%d", &id);
            printf("Inserisci il livello di coreness: ");
            scanf("%d", &level);
            printf("\n");
            trova_id(grafo, root, id, level);
        }
        else if (var == 11)
        {
            int id;
            printf("Inserisci l'id della componente connessa: ");
            scanf("%d", &id);
            printf("\n");
            trova_nodi(root, id);
        }
        else if (var == 12)
        {
            int id;
            printf("Inserisci l'id della componente connessa: ");
            scanf("%d", &id);
            printf("\n");
            componenti_k_kp1(grafo, root, id);
        }
        else if (var == 13)
        {
            int id;
            printf("Inserisci l'id della componente connessa: ");
            scanf("%d", &id);
            printf("\n");
            componenti_k_km1(root, id);
        }
        else if (var == 14)
        {
            int id_a;
            int id_b;
            printf("\nFunzione per trovare il massimo livello di coreness per il quale due veritici appartengolo al sottografo\n");
            printf("Inserisci l'id del primo vertice:\t");
            scanf("%d", &id_a);
            printf("Inserisci l'id del secondo vertice:\t");
            scanf("%d", &id_b);
            node_tree *lca = max_coreness_comune(grafo, id_a, id_b);
            printf("\nil massimo livello di coreness per cui i due vertici appartengono alla stessa componente connessa e' %d\n", lca->coreness);
        }
        else if (var == 15)
        {
            compatta(root, grafo->num_vertices);
        }
        else if (var == 16)
        {
            output_json(root, 0);
        }
        else if (var == 17)
        {
            output_json_cc(root, 0, input_file);
        }
        else if (var == 18)
        {
            int result = calcola_num_n_interni(root, grafo->num_vertices);
            printf("Il numero di nodi interni e': %d\n", result);
        }
        else if (var == 19)
        {
            int result = calcola_grado_massimo(grafo);
            printf("Il grado massimo e': %d\n", result);
        }
        else if (var == 20)
        {
            free(grafo->edges);
            free(grafo->vertices);
            free(grafo);
            exit(0);
        }
        else
        {
            printf("Valore non valido\n");
        }
        printf("--------------------------------------------------------------------------------------------------------------------------------------\n\n");
    }
}

//****************************************************************MAIN****************************************************************//

void action_compact(node_tree *nodo, node_tree *parent, node_tree *first_child)
{

    node_tree *n_child = nodo->child;

    // parent->e_numerousness += nodo->e_numerousness;
    // parent->n_numerousness += nodo->n_numerousness;
    parent->e_tm_numerousness += nodo->e_tm_numerousness;
    parent->n_tm_numerousness += nodo->n_tm_numerousness;
    parent->range_c[0] = nodo->range_c[0];

    // 1. eliminare nodo dai figli di parent
    if (first_child == nodo)
    {
        if (first_child->sibling == NULL)
        {
            parent->child = NULL;
            parent->last_child = NULL;
        }
        else
        {
            parent->child = first_child->sibling;
        }
    }
    else
    {
        node_tree *temp = first_child;
        while (temp->sibling != NULL)
        {
            if (temp->sibling == nodo)
            {
                if (temp->sibling->sibling == NULL)
                {
                    temp->sibling = NULL;
                    parent->last_child = temp;
                }
                else
                {
                    temp->sibling = temp->sibling->sibling;
                }
                break;
            }
            temp = temp->sibling;
        }
    }

    // 2. assegnare ai figli di nodo il parent parent
    while (n_child != NULL)
    {
        n_child->parent = parent;
        n_child = n_child->sibling;
    }

    // unire i figli
    if (parent->child != NULL)
    {
        parent->last_child->sibling = nodo->child;
        parent->last_child = nodo->last_child;
    }
    else
    {
        parent->child = nodo->child;
        parent->last_child = nodo->last_child;
    }
}

void compatta(node_tree *root, int len)
{

    node_tree *nodo = root->child;
    int size = len * 2;
    node_tree **temp = malloc(sizeof(node_tree) * size);
    node_tree **array = malloc(sizeof(node_tree) * len);

    if (temp == NULL || array == NULL)
    {
        printf("malloc sistema fallito\n");
    }

    int len_t = 0;
    int len_a = 0;
    int i = 0;

    root->depth = 0;
    root->coreness = 0;

    while (1)
    {

        if (nodo->sibling != NULL)
        {
            nodo->depth = nodo->parent->depth + 1;
            temp[len_t] = nodo->sibling;
            len_t++;
        }
        if (nodo->child != NULL)
        {
            nodo->depth = nodo->parent->depth + 1;
            temp[len_t] = nodo->child;
            len_t++;
        }

        if (nodo->depth % 2 == 0 && nodo->child != NULL)
        {
            printf("nodo: %d\n", nodo->id);
            array[len_a] = nodo;
            len_a++;
        }

        if (len_t > i)
        {
            nodo = temp[i];
            i++;
        }
        else
            break;
    }
    free(temp);

    printf("\tnodi da compattare %d\n", len_a);

    for (int j = 0; j < len_a; j++)
    {
        if (j % 100 == 0 && j != 0)
            printf("\t%d\n", j);
        action_compact(array[j], array[j]->parent, array[j]->parent->child);
    }
    free(array);
}
/*
void action_compact_coreness(node_tree* nodo, node_tree* parent, node_tree* first_child){

    node_tree* n_child = nodo->child;

    parent->e_numerousness += nodo->e_numerousness;
    parent->n_numerousness += nodo->n_numerousness;
    parent->e_tm_numerousness += nodo->e_tm_numerousness;
    parent->n_tm_numerousness += nodo->n_tm_numerousness;
    parent->range_c[0] = nodo->range_c[0];

    //1. eliminare nodo dai figli di parent
    if(first_child == nodo){
      if(first_child->sibling==NULL){
        parent->child=NULL;
        parent->last_child=NULL;
      }else{
        parent->child = first_child->sibling;
      }
    }else{
      node_tree* temp = first_child;
      while(temp->sibling != NULL){
        if(temp->sibling==nodo){
          if(temp->sibling->sibling == NULL){
            temp->sibling=NULL;
            parent->last_child=temp;
          }else{
            temp->sibling = temp->sibling->sibling;
          }
          break;
        }
        temp = temp->sibling;
      }
    }

    //2. assegnare ai figli di nodo il parent parent
    while(n_child!=NULL){
      n_child->parent = parent;
      n_child=n_child->sibling;
    }

    //unire i figli
    if(parent->child!=NULL){
      parent->last_child->sibling = nodo->child;
      parent->last_child = nodo->last_child;
    }else{
      parent->child=nodo->child;
      parent->last_child = nodo->last_child;
    }
}

void compatta_coreness(node_tree* root, int len, int dist){

    node_tree* nodo = root->child;
    int size = len*2;
    node_tree** temp = malloc(sizeof(node_tree) * size);
    node_tree** array = malloc(sizeof(node_tree) * len);

    if(temp==NULL || array==NULL){
      printf("malloc sistema fallito\n");
    }

    int len_t=0;
    int len_a=0;
    int i=0;

    root->depth = 0;
    root->coreness = 0;

    while(1){

        if(nodo->sibling != NULL){
            nodo->depth = nodo->parent->depth + 1;
            temp[len_t] = nodo->sibling;
            len_t++;
        }
        if(nodo->child != NULL){
          nodo->depth = nodo->parent->depth + 1;
            temp[len_t] = nodo->child;
            len_t++;
        }

        if(nodo->depth%2==0 && nodo->child!=NULL){
            printf("nodo: %d\n", nodo->id);
            array[len_a]=nodo;
            len_a++;
        }

        if(len_t>i){
            nodo=temp[i];
            i++;
        }
        else
            break;
    }
    free(temp);

    printf("\tnodi da compattare %d\n", len_a);

    for(int j=0; j<len_a; j++){
        if(j%100==0 && j!=0)
          printf("\t%d\n", j);
        action_compact(array[j], array[j]->parent, array[j]->parent->child);
    }
    free(array);

}

void compatta_coreness_rec(node_tree* node, int len, int dist){
    int child_len = 0;
    if(node->child)
    child_len = 1;
    //node_tree* child =
}
*/

void calculate_node_numerousness(graph *grafo)
{

    for (int i = 1; i <= grafo->num_vertices; i++)
    {
        node_tree *n = grafo->vertices[i].v_tree;
        char class=grafo->vertices[i].class;
        if (n->id != 0)
        {
            n->parent->n_tm_numerousness += 1;
            if(class=='l')
                n->parent->n_tm_numerousnessL += 1;
            else
                n->parent->n_tm_numerousnessR += 1;
           
            // if(n->parent->n_tm_numerousness < 0 ) printf("id padre: %d,     id nodo: %d\n", n->parent->id, n->id);
            while (n->parent != NULL)
            {
                n->parent->n_numerousness += 1;
                if(class=='l')
                    n->parent->n_numerousnessL += 1;
                else
                    n->parent->n_numerousnessR += 1;
                n = n->parent;
            }
        }
    }
}

void calculate_edge_numerousness(graph *grafo)
{
    for (int i = 0; i < grafo->num_edges; i++)
    {
        vertex *a = grafo->edges[i].a;
        vertex *b = grafo->edges[i].b;
        node_tree *lca = max_coreness_comune(grafo, a->id, b->id);

        lca->e_tm_numerousness += 1;

        while (lca != NULL)
        {
            lca->e_numerousness += 1;
            lca = lca->parent;
        }
    }
}

void calculate_depth(graph *grafo)
{
    for (int i = 1; i <= grafo->num_vertices; i++)
    {
        node_tree *n = grafo->vertices[i].v_tree;
        n->depth = 0;

        while (n->parent != NULL)
        {
            n->parent->depth = n->depth + 1;
            n = n->parent;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Please provide a string as a command-line argument.\n");
        return 1; // Exit the program with an error code
    }

    char *input_file = argv[1];

    LARGE_INTEGER frequency;                          // ticks per second
    LARGE_INTEGER t1, t9; //t2, t3, t4, t5, t6, t7, t8, t9; // ticks
    double diff_sec;

    // get ticks per second
    QueryPerformanceFrequency(&frequency);

    // start timer
    QueryPerformanceCounter(&t1);

    // leggi i dati da file e setta il grafo di input
    //printf("Inizio setting input\n");
    graph *graph_input = set_input(input_file);
    //QueryPerformanceCounter(&t2);
    //diff_sec = (t2.QuadPart - t1.QuadPart) / (double)frequency.QuadPart;
    //printf("Fine setting input in %f s\n", diff_sec);

    // funzione per calcolare la coreness dei nodi
    //printf("Inizio calcolo coreness\n");
    compute_coreness(graph_input);
    //QueryPerformanceCounter(&t3);
    //diff_sec = (t3.QuadPart - t2.QuadPart) / (double)frequency.QuadPart;
    //printf("Fine calcolo coreness in %f s\n", diff_sec);

    // funzione per modificare   la lista degli archi del grafo in ordine decrescente di coreness
    ordina_archi(graph_input);
    //QueryPerformanceCounter(&t4);
    //diff_sec = (t4.QuadPart - t3.QuadPart) / (double)frequency.QuadPart;
    //printf("Fine ordinamento archi in %f s\n", diff_sec);

    // funzione per creare le foglie dell'albero di output e le foglie della struttura di supporto
    init(graph_input);
    //QueryPerformanceCounter(&t5);
    //diff_sec = (t5.QuadPart - t4.QuadPart) / (double)frequency.QuadPart;
    //printf("Fine creazione foglie output tree e disjoint set in %f s\n", diff_sec);

    // costruzione dell'albero di output
    node_tree *root = build(graph_input);
    //QueryPerformanceCounter(&t6);
    //printf("Inizio costruzione albero di output in %f s\n", diff_sec);
    //diff_sec = (t6.QuadPart - t5.QuadPart) / (double)frequency.QuadPart;
    //printf("Fine costruzione albero di output in %f s\n", diff_sec);

    // sistema output tree per eliminare parentele "padre-figlio" con stessa coreness
    sistema(root, graph_input->num_vertices);
    //QueryPerformanceCounter(&t7);
    //diff_sec = (t7.QuadPart - t6.QuadPart) / (double)frequency.QuadPart;
    //printf("Fine sistemazione output tree in %f s\n", diff_sec);

    calculate_node_numerousness(graph_input);
    //QueryPerformanceCounter(&t8);
    //diff_sec = (t8.QuadPart - t7.QuadPart) / (double)frequency.QuadPart;
    //printf("Fine calcolo numerosita' nodi in %f s\n", diff_sec);

    // calculate_edge_numerousness(graph_input);
    //QueryPerformanceCounter(&t9);
    //diff_sec = (t9.QuadPart - t8.QuadPart) / (double)frequency.QuadPart;
    //printf("Fine calcolo numerosita' archi in %f s\n", diff_sec);

    diff_sec = (t9.QuadPart - t1.QuadPart) / (double)frequency.QuadPart;
    printf("\nTempo totale: %f s\n", diff_sec);

    primitive(graph_input, root, input_file);

    return 0;
}
