#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int rank;
	int nProcesses;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	// printf("Hello from %i/%i \n", rank, nProcesses);
	if (rank == 0)
	{
		int a[100];
		for (int i = 0; i < 100; i++)
		{
			a[i] = i;
		}
		MPI_Send(a, 100, MPI_INT, 1, 0, MPI_COMM_WORLD);
	}
	if (rank == 1)
	{
		int b[100];
		MPI_Recv(b, 100, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Received array: ");
		for (int i = 0; i < 100; i++)
		{
			printf("%d ", b[i]);
		}
		printf("\n");
	}
	MPI_Finalize();
	return 0;
}