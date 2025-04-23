#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int rank;
	int nProcesses;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	switch (rank)
	{
	case 0:
		printf("Hello World from %i/%i\n", rank, nProcesses);
		break;
	case 1:
		printf("SMTH else from %i/%i\n", rank, nProcesses);
		break;
	}
	MPI_Finalize();
	return 0;
}
