#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define VECTOR_SIZE 100000  // Dimensiunea vectorului pentru exemplificare

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        int *vector = malloc(VECTOR_SIZE * sizeof(int));
        for (int i = 0; i < 10; i++) {
            vector[i] = i;
        }
        MPI_Request request;
        MPI_Isend(vector, VECTOR_SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
        printf("Procesul 0 a inițiat trimiterea vectorului.\n");

        // Modificăm vectorul după trimitere
        //vector[0] = 100;  // Modificare la o poziție mică
		vector[99999] = 100;  // Modificare la o poziție mare

        // Așteptăm finalizarea trimiterii
        MPI_Wait(&request, MPI_STATUS_IGNORE);
        printf("Procesul 0 a finalizat trimiterea vectorului.\n");

        free(vector);
    } else if (rank == 1) {
        int *received_vector = malloc(VECTOR_SIZE * sizeof(int));
       MPI_Request request;
        MPI_Irecv(received_vector, VECTOR_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        printf("Procesul 1 a inițiat primirea vectorului.\n");

        // Așteptăm finalizarea primirii
        MPI_Wait(&request, MPI_STATUS_IGNORE);
        printf("Procesul 1 a finalizat primirea vectorului.\n");

        // Afișăm vectorul primit
        printf("Vector primit de procesul 1: ");
        for (int i = 0; i < 10; i++) {
            printf("%d ", received_vector[i]);
        }
        printf("%d\n", received_vector[99999]);  // Afișăm și modificarea de la o poziție mare

        free(received_vector);
    }

    MPI_Finalize();
    return 0;
}