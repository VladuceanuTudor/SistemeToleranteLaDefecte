#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SMALL_SIZE 5         // Dimensiune vector mic
#define LARGE_SIZE 1000000   // Dimensiune vector mare

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Test 1: Vector mic
    if (rank == 0) {
        int *small_vector = malloc(SMALL_SIZE * sizeof(int));
        for (int i = 0; i < SMALL_SIZE; i++) {
            small_vector[i] = i;
        }
        double start_time = MPI_Wtime();
        printf("Procesul 0: Inițiez trimiterea vectorului mic.\n");
        MPI_Send(small_vector, SMALL_SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD);
        double end_time = MPI_Wtime();
        printf("Procesul 0: Am finalizat trimiterea vectorului mic. Timp: %f secunde.\n", end_time - start_time);
        free(small_vector);
    } else if (rank == 1) {
        int *small_vector = malloc(SMALL_SIZE * sizeof(int));
        printf("Procesul 1: Inițiez primirea vectorului mic.\n");
        MPI_Recv(small_vector, SMALL_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Procesul 1: Am primit vectorul mic: ");
        for (int i = 0; i < SMALL_SIZE; i++) {
            printf("%d ", small_vector[i]);
        }
        printf("\n");
        free(small_vector);
    }

    // Barieră pentru sincronizare între teste
    MPI_Barrier(MPI_COMM_WORLD);

    // Test 2: Vector mare
    if (rank == 0) {
        int *large_vector = malloc(LARGE_SIZE * sizeof(int));
        for (int i = 0; i < LARGE_SIZE; i++) {
            large_vector[i] = i;
        }
        double start_time = MPI_Wtime();
        printf("Procesul 0: Inițiez trimiterea vectorului mare.\n");
        MPI_Send(large_vector, LARGE_SIZE, MPI_INT, 1, 1, MPI_COMM_WORLD);
        double end_time = MPI_Wtime();
        printf("Procesul 0: Am finalizat trimiterea vectorului mare. Timp: %f secunde.\n", end_time - start_time);
        free(large_vector);
    } else if (rank == 1) {
        int *large_vector = malloc(LARGE_SIZE * sizeof(int));
        printf("Procesul 1: Inițiez primirea vectorului mare.\n");
        MPI_Recv(large_vector, LARGE_SIZE, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Procesul 1: Am primit vectorul mare.\n");
        free(large_vector);
    }

    MPI_Finalize();
    return 0;
}