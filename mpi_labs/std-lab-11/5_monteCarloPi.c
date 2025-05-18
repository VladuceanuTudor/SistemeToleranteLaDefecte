#include <math.h>
#include <mpi.h>              // **ADDED MPI**
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int T;           // total number of tosses
int printLevel;  // 0 = no print, 1 = print result

void getArgs(int argc, char **argv) {
    if (argc < 3) {
        if (argc >= 1) {
            printf("Usage: %s T printLevel\n", argv[0]);
        } else {
            printf("Usage: ./program T printLevel\n");
        }
        printf("  T          = total number of random tosses (must be divisible by number of processes)\n");
        printf("  printLevel = 0 (quiet) or 1 (print π estimate)\n");
        exit(1);
    }
    T = atoi(argv[1]);
    printLevel = atoi(argv[2]);
}

int main(int argc, char **argv) {
    int rank, size;              // **ADDED MPI**
    MPI_Init(&argc, &argv);      // **ADDED MPI**
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // **ADDED MPI**
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // **ADDED MPI**

    if (rank == 0) {
        getArgs(argc, argv);
        if (T % size != 0) {
            fprintf(stderr, "Error: T (%d) must be divisible by #processes (%d)\n", T, size);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Broadcast T and printLevel to all ranks
    MPI_Bcast(&T,          1, MPI_INT, 0, MPI_COMM_WORLD);   // **ADDED MPI**
    MPI_Bcast(&printLevel, 1, MPI_INT, 0, MPI_COMM_WORLD);   // **ADDED MPI**

    // Each process gets T_local tosses
    int T_local = T / size;                                   // **ADDED MPI**

    // Seed RNG differently per rank
    unsigned int seed = (unsigned int)time(NULL) ^ rank;
    srand(seed);

    // Perform local Monte Carlo
    int inCircle_local = 0;
    for (int i = 0; i < T_local; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x*x + y*y <= 1.0) {
            inCircle_local++;
        }
    }

    // Gather all local inCircle counts at root
    int *all_inCircle = NULL;
    if (rank == 0) {
        all_inCircle = malloc(size * sizeof(int));           // **ADDED MPI**
    }
    MPI_Gather(&inCircle_local, 1, MPI_INT,                // **ADDED MPI**
               all_inCircle,       1, MPI_INT,
               0, MPI_COMM_WORLD);

    // Root computes final π estimate
    if (rank == 0) {
        long long total_inCircle = 0;
        for (int i = 0; i < size; i++) {
            total_inCircle += all_inCircle[i];
        }
        double pi_est = 4.0 * (double)total_inCircle / (double)T;
        if (printLevel == 1) {
            printf("Estimated π = %f (using %d tosses, %d processes)\n",
                   pi_est, T, size);
        }
        free(all_inCircle);
    }

    MPI_Finalize();             // **ADDED MPI**
    return 0;
}
