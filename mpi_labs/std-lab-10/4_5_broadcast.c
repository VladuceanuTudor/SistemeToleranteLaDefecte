#include <mpi.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int rank;
	int nProcesses;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	
	int a[100];
	if (rank == 1){
		for(int i = 0; i < 100; i++)
		{
			a[i] = i;
		}
	}
	MPI_Bcast(a, 100, MPI_INT, 1, MPI_COMM_WORLD);
	if(rank != 1) {
		printf("%i Received array: ", rank);
		for (int i = 0; i < 100; i++)
		{
			printf("%d ", a[i]);
		}
		printf("\n");
	}
	MPI_Finalize();
	return 0;
}