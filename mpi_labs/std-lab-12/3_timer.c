#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    
    if (argc < 2) {
        if (rank == 0) {
            printf("Usage: %s X\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    int X = atoi(argv[1]); 

    if (rank == 0) {
        
        sleep(X);
        int message = 42;
        MPI_Request request;
        MPI_Isend(&message, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, MPI_STATUS_IGNORE); // Așteptăm finalizarea trimiterii
    } else if (rank == 1) {
        int received_message;
        MPI_Request request;
        MPI_Irecv(&received_message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
        int flag = 0;
        for (int i = 0; i < 5; i++) {
            sleep(1);
            MPI_Test(&request, &flag, MPI_STATUS_IGNORE);
            if (flag) {
                printf("Mesaj primit după %d secunde: %d\n", i+1, received_message);
                break;
            } else {
                printf("Verificat la %d secunde, mesajul nu a fost încă primit.\n", i+1);
            }
        }
        if (!flag) {
            printf("Mesajul nu a fost primit în 5 secunde.\n");
        }
    }

    MPI_Finalize();
    return 0;
}