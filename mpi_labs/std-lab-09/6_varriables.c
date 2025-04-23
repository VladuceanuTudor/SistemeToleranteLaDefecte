#include <mpi.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char **argv)
{
	int rank;
	int nProcesses;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	// printf("Hello World from %i/%i \n", rank, nProcesses);
	int a = 2;
	int b;
	b = 0;
	if (rank == 0)
	{
		b = 100;
	}
	if (rank == nProcesses - 1)
	{
		b = 1000;
	}

	printf("%d Hello World %g  \n", rank, pow(a, rank) + b);
	MPI_Finalize();
	return 0;
}