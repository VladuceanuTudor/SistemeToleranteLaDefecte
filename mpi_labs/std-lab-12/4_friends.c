#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Verificăm dacă sunt exact 5 procese
    if (size != 5) {
        if (rank == 0) {
            printf("Acest program necesită exact 5 procese.\n");
        }
        MPI_Finalize();
        return 1;
    }

    // Determinăm vecinii
    int neighbors[2];
    int num_neighbors;
    if (rank == 0) {
        neighbors[0] = 1;
        num_neighbors = 1;
    } else if (rank == 4) {
        neighbors[0] = 3;
        num_neighbors = 1;
    } else {
        neighbors[0] = rank - 1;
        neighbors[1] = rank + 1;
        num_neighbors = 2;
    }

    // Inițializăm generatorul de numere aleatoare
    srand(time(NULL) + rank);

    // Procesul 0 trimite mesajul inițial către procesul 1
    if (rank == 0) {
        int message = 1;
        MPI_Request request;
        printf("Proces %d: Inițiez trimiterea mesajului către %d\n", rank, 1);
        MPI_Isend(&message, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, MPI_STATUS_IGNORE);
        printf("Proces %d: Trimitere finalizată către %d\n", rank, 1);
    }

    // Toate procesele intră în bucla de recepție
    int max_messages = 5; // Limităm la 5 mesaje per proces
    int messages_processed = 0;

    // Inițializăm cererile de recepție
    int buffers[2];
    MPI_Request requests[2];
    int active_requests[2] = {0, 0}; // Urmărim cererile active
    for (int i = 0; i < num_neighbors; i++) {
        MPI_Irecv(&buffers[i], 1, MPI_INT, neighbors[i], 0, MPI_COMM_WORLD, &requests[i]);
        active_requests[i] = 1;
        printf("Proces %d: Inițiez primirea de la vecinul %d\n", rank, neighbors[i]);
    }

    while (messages_processed < max_messages) {
        int message_received = 0;
        for (int i = 0; i < 5 && !message_received; i++) {
            sleep(1);
            int index;
            int flag;
            MPI_Testany(num_neighbors, requests, &index, &flag, MPI_STATUS_IGNORE);
            if (flag && active_requests[index]) {
                message_received = 1;
                int wait_time = rand() % 6; // Timp aleatoriu între 0 și 5 secunde
                printf("Proces %d a primit mesaj de la %d, așteaptă %d secunde\n", rank, neighbors[index], wait_time);
                sleep(wait_time);
                // Trimitem mesajul tuturor vecinilor
                MPI_Request send_requests[2];
                for (int j = 0; j < num_neighbors; j++) {
                    int neighbor = neighbors[j];
                    int send_message = 1;
                    printf("Proces %d: Inițiez trimiterea mesajului către %d\n", rank, neighbor);
                    MPI_Isend(&send_message, 1, MPI_INT, neighbor, 0, MPI_COMM_WORLD, &send_requests[j]);
                }
                // Așteptăm finalizarea tuturor trimiterilor
                for (int j = 0; j < num_neighbors; j++) {
                    MPI_Wait(&send_requests[j], MPI_STATUS_IGNORE);
                    printf("Proces %d: Trimitere finalizată către %d\n", rank, neighbors[j]);
                }
                // Repostăm cererea pentru vecinul care a trimis mesajul
                MPI_Irecv(&buffers[index], 1, MPI_INT, neighbors[index], 0, MPI_COMM_WORLD, &requests[index]);
                printf("Proces %d: Inițiez primirea de la vecinul %d\n", rank, neighbors[index]);
                active_requests[index] = 1;
                messages_processed++;
            }
        }
        if (!message_received) {
            printf("Proces %d: Niciun mesaj primit în 5 secunde\n", rank);
        }
    }

    // Anulăm cererile active la final
    for (int i = 0; i < num_neighbors; i++) {
        if (active_requests[i]) {
            MPI_Cancel(&requests[i]);
            MPI_Wait(&requests[i], MPI_STATUS_IGNORE);
        }
    }

    MPI_Finalize();
    return 0;
}