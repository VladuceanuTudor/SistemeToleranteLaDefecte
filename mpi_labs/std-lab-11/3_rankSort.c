#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>                      // **MPI MOD**

int N;
int printLevel;
int *v;
int *vQSort;

void getArgs(int argc, char **argv)
{
    if (argc < 3) {
        printf("Not enough paramters: ./program N printLevel\nprintLevel: 0=no, 1=verbose, 2=partial\n");
        exit(1);
    }
    N = atoi(argv[1]);
    printLevel = atoi(argv[2]);
}

void compareVectors(int *a, int *b)
{
    // DO NOT MODIFY
    int i;
    for (i = 0; i < N; i++) {
        if (a[i] != b[i]) {
            printf("Sorted incorrectly\n");
            return;
        }
    }
    printf("Sorted correctly\n");
}

void displayVector(int *v)
{
    // DO NOT MODIFY
    int i;
    int displayWidth = 2 + log10(N);
    for (i = 0; i < N; i++) {
        printf("%*i", displayWidth, v[i]);
    }
    printf("\n");
}

int cmp(const void *a, const void *b)
{
    // DO NOT MODIFY
    int A = *(int *)a;
    int B = *(int *)b;
    return A - B;
}

void printPartial()
{
    compareVectors(v, vQSort);
}

void printAll()
{
    displayVector(v);
    displayVector(vQSort);
    compareVectors(v, vQSort);
}

void printResults()
{
    if (printLevel == 0)
        return;
    else if (printLevel == 2)
        printPartial();
    else
        printAll();
}

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    getArgs(argc, argv);

    // Verificăm divizibilitatea pentru Scatter/Gather simplu
    if (N % size != 0) {
        if (rank == 0) {
            fprintf(stderr, "Error: N (%d) must be divisible by number of processes (%d)\n", N, size);
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int localN      = N / size;
    int *localVals  = malloc(localN * sizeof(int));
    int *localRanks = malloc(localN * sizeof(int));

    // Pe root: generăm v și vQSort
    if (rank == 0) {
        v      = malloc(N * sizeof(int));
        vQSort = malloc(N * sizeof(int));

        srand(42);
        // inițial 0..N-1
        for (int i = 0; i < N; i++) 
            v[i] = i;
        // shuffle
        for (int i = 0; i < 5 * N; i++) {
            int a = rand() % N, b = rand() % N, t = v[a];
            v[a] = v[b];
            v[b] = t;
        }
        // copia pentru verificare
        memcpy(vQSort, v, N * sizeof(int));
        qsort(vQSort, N, sizeof(int), cmp);
    }

    // *** Asigurăm buffer-ul v pe toate rank-urile înainte de Bcast ***
    if (rank != 0) {
        v = malloc(N * sizeof(int));
    }

    // Trimitem vectorul complet pentru calcul rank
    MPI_Bcast(v, N, MPI_INT, 0, MPI_COMM_WORLD);

    // Acum putem face Scatter în localVals
    MPI_Scatter(v,      localN, MPI_INT,
                localVals, localN, MPI_INT,
                0, MPI_COMM_WORLD);

    // Calculăm rank-ul fiecărui element local
    for (int i = 0; i < localN; i++) {
        int val = localVals[i];
        int r   = 0;
        int global_index = rank * localN + i;
        for (int j = 0; j < N; j++) {
            if (v[j] < val || (v[j] == val && j < global_index)) {
                r++;
            }
        }
        localRanks[i] = r;
    }

    // Adunăm rank-urile pe root
    int *allRanks = NULL;
    if (rank == 0) {
        allRanks = malloc(N * sizeof(int));
    }
    MPI_Gather(localRanks, localN, MPI_INT,
               allRanks,  localN, MPI_INT,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        // Construim vectorul sortat
        int *out = malloc(N * sizeof(int));
        for (int i = 0; i < N; i++) {
            out[ allRanks[i] ] = v[i];
        }
        // Pregătim v pentru printResults()
        free(v);
        v = out;
        printResults();

        free(v);
        free(vQSort);
        free(allRanks);
    }

    // Cleanup


    MPI_Finalize();
    return 0;
}


