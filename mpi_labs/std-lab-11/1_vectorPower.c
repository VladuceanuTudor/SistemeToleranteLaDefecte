#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

int N;
int printLevel;
int *v;
int *outV;

void getArgs(int argc, char **argv)
{
	if (argc < 3) {
		printf("Not enough paramters: ./program N printLevel\nprintLevel: 0=no, 1=verbouse\n");
		exit(1);
	}
	N = atoi(argv[1]);
	printLevel = atoi(argv[2]);
}

void displayVectors(int *v, int *outV)
{
	int i;
	int max = 1;
	for (i = 0; i < N; i++) {
		if (max < log10(v[i]))
			max = log10(v[i]);
		if (max < log10(outV[i]))
			max = log10(outV[i]);
	}
	int displayWidth = 2 + max;

	printf("Original vector:\n");
	for (i = 0; i < N; i++) {
		printf("%*i", displayWidth, v[i]);
		if (!((i + 1) % 20))
			printf("\n");
	}

	printf("Power vector:\n");
	for (i = 0; i < N; i++) {
		printf("%*i", displayWidth, outV[i]);
		if (!((i + 1) % 20))
			printf("\n");
	}
	printf("\n");
}

void printAll()
{
	displayVectors(v, outV);
}

void print()
{
	if (printLevel == 0)
		return;
	else
		printAll();
}

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    getArgs(argc, argv);

    // Allocate full arrays on root
    if (rank == 0) {
        v    = malloc(sizeof(int) * N);
        outV = malloc(sizeof(int) * N);
        if (!v || !outV) { fprintf(stderr, "Allocation failed\n"); MPI_Abort(MPI_COMM_WORLD, 1); }
        srand(42);
        for (int i = 0; i < N; i++) v[i] = rand() % 1000;
    }

    // Determine local sizes
    int base = N / size;
    int rem  = N % size;
    int *counts = malloc(sizeof(int) * size);
    int *displs = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++) {
        counts[i] = base + (i < rem ? 1 : 0);
        displs[i] = (i == 0 ? 0 : displs[i-1] + counts[i-1]);
    }
    int localN = counts[rank];

    // Allocate local buffers
    int *localV    = malloc(sizeof(int) * localN);
    int *localOutV = malloc(sizeof(int) * localN);
    if (!localV || !localOutV) { fprintf(stderr, "Local allocation failed\n"); MPI_Abort(MPI_COMM_WORLD, 2); }

    // Scatter v
    MPI_Scatterv(v, counts, displs, MPI_INT,
                 localV, localN, MPI_INT,
                 0, MPI_COMM_WORLD);

    // Compute power for local segment
    for (int i = 0; i < localN; i++) {
        int rez = 1;
        for (int j = 0; j < localV[i]; j++)
            rez = (rez * 42) % 1000;
        localOutV[i] = rez;
    }

    // Gather results
    MPI_Gatherv(localOutV, localN, MPI_INT,
                outV, counts, displs, MPI_INT,
                0, MPI_COMM_WORLD);

    // Print on root
    if (rank == 0) {
        print();
    }

    // Cleanup
    free(localV);
    free(localOutV);
    free(counts);
    free(displs);
    if (rank == 0) {
        free(v);
        free(outV);
    }

    MPI_Finalize();
    return 0;
}
