#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// clang-format off

// Run: mpirun -np 5 ./a.out
// DO NOT USE DIRECTLY. USE getNeighbors() INSTEAD
int graph[][2] = { { 0, 1 }, { 1, 0 }, 
                   { 2, 1 }, { 1, 2 },
                   { 3, 1 }, { 1, 3 },
				   { 3, 4 }, { 4, 3 } };

//Run: mpirun -np 12 ./a.out
// DO NOT USE DIRECTLY. USE getNeighbors() INSTEAD
// int graph[][2] = { { 0, 1 }, { 1, 2 }, { 2, 3 }, 
//                    { 3, 4 }, { 4,11 }, {11, 5 }, 
//                    { 5, 6 }, { 6, 7 }, { 7, 8 },
//                    { 8, 9 }, { 9,10 }, {10, 9 },
//                    { 9, 8 }, { 8, 7 }, { 7, 6 },
//                    { 6, 5 }, { 5,11 }, {11, 4 },
//                    { 4, 3 }, { 3, 2 }, { 2, 1 },
//                    { 1, 0 }, { 9, 5 }, { 5, 9 },
//                    { 5, 3 }, { 3, 5 }, { 0, 2 },
//                    { 2, 0 }, { 9, 7 }, { 7, 9 } };

// clang-format on

// getNeighbors returns a 100 int vector.
// position 0 represents the number of neighbors
// positions 1+ represent the ranks of the neighbors
// your program should only send messages to processes on this list
int *getNeighbors(int myRank)
{
	int *neigh = malloc(sizeof(int) * 100);
	int i;
	int j = 1;
	int sizeOfGraph = sizeof(graph) / sizeof(int) / 2;
	for (i = 0; i < sizeOfGraph; i++) {
		if (graph[i][0] == myRank) {
			neigh[j] = graph[i][1];
			j++;
		}
	}
	neigh[0] = j;
	return neigh;
}

void printNeighbors(int *neigh, int rank)
{
	char aux[1000];
	aux[0] = 0;
	sprintf(aux + strlen(aux), "Rank %2i neighbors: ", rank);
	for (int i = 1; i < neigh[0]; i++) {
		sprintf(aux + strlen(aux), " %2i,", neigh[i]);
	}
	sprintf(aux + strlen(aux) - 1, "\n");  // also deletes the last comma
	printf("%s", aux);
}

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <unistd.h>

// Assume getNeighbors and printNeighbors are defined as before
int* getNeighbors(int rank);
void printNeighbors(int* neigh, int rank);

int main(int argc, char *argv[])
{
    int rank, nProcesses;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

    int *neigh = getNeighbors(rank);
    printNeighbors(neigh, rank);

    // Get initiator rank from command-line argument
    if (argc != 2) {
        if (rank == 0) {
            printf("Error: Please provide initiator rank as a command-line argument.\n");
        }
        MPI_Finalize();
        free(neigh);
        return 1;
    }
    int initiator = atoi(argv[1]);

    // Initialize variables
    int parent = -1; // -1 means no parent yet
    int parents[100] = {-1}; // Local parent array
    int visited[100] = {0}; // Global visited array
    int message[100] = {0}; // Message buffer: [0] = sender, [1] = hop count
    MPI_Status status;

    // Initiator logic
    if (rank == initiator) {
        parent = rank;
        parents[rank] = parent;
        visited[rank] = 1;
        printf("Rank %d: I am the initiator, sending initial message to neighbors.\n", rank);
        for (int i = 1; i < neigh[0]; i++) {
            message[0] = rank; // Sender rank
            message[1] = 1; // Hop count
            printf("Rank %d: Sending initial message to neighbor %d.\n", rank, neigh[i]);
            MPI_Send(message, 100, MPI_INT, neigh[i], 0, MPI_COMM_WORLD);
        }
    }

    // Broadcast visited array to synchronize state
    MPI_Bcast(visited, 100, MPI_INT, initiator, MPI_COMM_WORLD);

    // Non-initiator processes
    if (rank != initiator) {
        double start_time = MPI_Wtime();
        int message_received = 0;
        while (MPI_Wtime() - start_time < 15.0 && !message_received) {
            for (int i = 1; i < neigh[0]; i++) {
                if (!visited[rank]) {
                    int flag;
                    MPI_Iprobe(neigh[i], 0, MPI_COMM_WORLD, &flag, &status);
                    if (flag) {
                        MPI_Recv(message, 100, MPI_INT, neigh[i], 0, MPI_COMM_WORLD, &status);
                        printf("Rank %d: Received message from %d (hop count %d).\n", rank, message[0], message[1]);
                        if (parent == -1) { // Set parent if not already set
                            parent = message[0]; // Set parent to sender
                            parents[rank] = parent;
                            visited[rank] = 1;
                            printf("Rank %d: Set parent to %d.\n", rank, parent);
                            // Broadcast updated visited array
                            MPI_Bcast(visited, 100, MPI_INT, rank, MPI_COMM_WORLD);
                            // Forward message to unvisited neighbors
                            for (int j = 1; j < neigh[0]; j++) {
                                if (neigh[j] != parent && !visited[neigh[j]]) {
                                    message[0] = rank; // Update sender
                                    message[1]++; // Increment hop count
                                    printf("Rank %d: Forwarding message to neighbor %d.\n", rank, neigh[j]);
                                    MPI_Send(message, 100, MPI_INT, neigh[j], 0, MPI_COMM_WORLD);
                                }
                            }
                            message_received = 1;
                        }
                    }
                }
            }
            sleep(1); // Check every second
        }
    }

    // Barrier to ensure all processes complete message forwarding
    MPI_Barrier(MPI_COMM_WORLD);

    // Gather parent information at initiator
    int all_parents[100];
    MPI_Reduce(parents, all_parents, 100, MPI_INT, MPI_MAX, initiator, MPI_COMM_WORLD);

    // Initiator prints complete parent list
    if (rank == initiator) {
        printf("Rank %d: Collecting complete parent list.\n", rank);
        printf("Complete parent list for all processes:\n");
        for (int i = 0; i < nProcesses; i++) {
            if (all_parents[i] != -1) {
                printf("Rank %d: Parent of process %d is %d.\n", rank, i, all_parents[i]);
            } else {
                printf("Rank %d: Process %d has no parent.\n", rank, i);
            }
        }
    }

    // Clean up
    free(neigh);
    MPI_Finalize();
    return 0;
}