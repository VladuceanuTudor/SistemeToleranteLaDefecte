#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int N;         // Dimensiunea matricei
int printLevel; // Nivelul de afișare

// Funcție pentru citirea argumentelor din linia de comandă
void getArgs(int argc, char **argv)
{
    if (argc < 3) {
        printf("Not enough parameters: ./program N printLevel\nprintLevel: 0=no, 1=verbose\n");
        exit(1);
    }
    N = atoi(argv[1]);
    printLevel = atoi(argv[2]);
}

// Funcție pentru afișarea matricei c
void printAll(int *c)
{
    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            printf("%i\t", c[i * N + j]);
        }
        printf("\n");
    }
}

// Funcție pentru gestionarea afișării bazate pe printLevel
void print(int *c)
{
    if (printLevel == 1) {
        printAll(c);
    }
}

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Procesul root citește argumentele
    if (rank == 0) {
        getArgs(argc, argv);
    }

    // Difuzarea dimensiunii N către toate procesele
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calcularea numărului de rânduri per proces
    int local_N = N / size; // Presupunem că N este divizibil cu size

    // Alocarea memoriei pentru tablourile locale în toate procesele
    int *local_a = malloc(sizeof(int) * local_N * N); // Porțiunea locală din a
    int *local_c = malloc(sizeof(int) * local_N * N); // Porțiunea locală din c
    int *b = malloc(sizeof(int) * N * N);             // Matricea b completă
    if (local_a == NULL || local_c == NULL || b == NULL) {
        printf("malloc failed!\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Alocarea și inițializarea matricelor a și c doar pe root
    int *a = NULL;
    int *c = NULL;
    if (rank == 0) {
        a = malloc(sizeof(int) * N * N);
        c = malloc(sizeof(int) * N * N);
        if (a == NULL || c == NULL) {
            printf("malloc failed!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        // Inițializarea matricelor a și b
        int i, j;
        for (i = 0; i < N; i++) {
            for (j = 0; j < N; j++) {
                if (i <= j) {
                    a[i * N + j] = 1;
                    b[i * N + j] = 1;
                } else {
                    a[i * N + j] = 0;
                    b[i * N + j] = 0;
                }
            }
        }
    }

    // Distribuirea rândurilor din a către local_a în fiecare proces
    MPI_Scatter(
        (rank == 0) ? a : NULL, // Buffer de trimitere (doar root trimite)
        local_N * N,            // Numărul de elemente trimise per proces
        MPI_INT,                // Tipul de date
        local_a,                // Buffer de recepție
        local_N * N,            // Numărul de elemente recepționate
        MPI_INT,                // Tipul de date
        0,                      // Procesul root
        MPI_COMM_WORLD          // Comunicator
    );

    // Difuzarea matricei b către toate procesele
    MPI_Bcast(b, N * N, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculul local al porțiunii din c
    int i, j, k;
    for (i = 0; i < local_N; i++) {
        for (j = 0; j < N; j++) {
            local_c[i * N + j] = 0;
            for (k = 0; k < N; k++) {
                local_c[i * N + j] += local_a[i * N + k] * b[k * N + j];
            }
        }
    }

    // Adunarea porțiunilor din local_c în c pe root
    MPI_Gather(
        local_c,                // Buffer de trimitere
        local_N * N,            // Numărul de elemente trimise
        MPI_INT,                // Tipul de date
        (rank == 0) ? c : NULL, // Buffer de recepție (doar root primește)
        local_N * N,            // Numărul de elemente recepționate per proces
        MPI_INT,                // Tipul de date
        0,                      // Procesul root
        MPI_COMM_WORLD          // Comunicator
    );

    // Afișarea rezultatului doar pe root
    if (rank == 0) {
        print(c);
    }

    // Eliberarea memoriei
    free(local_a);
    free(local_c);
    free(b);
    if (rank == 0) {
        free(a);
        free(c);
    }

    // Finalizarea MPI
    MPI_Finalize();
    return 0;
}