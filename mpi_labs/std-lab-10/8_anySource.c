#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int rank;
	int nProcesses;
	MPI_Status status;


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	
	if(rank == nProcesses - 1)
	{
		for(int i = 0; i < nProcesses - 1; i++)
		{
			int data;
			MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("Received %d with status: %d\n", data, status.MPI_SOURCE);
		}
	}
	else
	{
		int trim=rank+1;
		MPI_Send(&trim, 1, MPI_INT, nProcesses - 1, 0, MPI_COMM_WORLD);
	}


	MPI_Finalize();

	return 0;
}