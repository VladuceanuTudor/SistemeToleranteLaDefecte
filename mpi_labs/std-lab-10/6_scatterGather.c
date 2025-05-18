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
	int local[25];
	if(rank == 0)
		for(int i = 0; i < 100; i++)
			a[i] = i;
		

		MPI_Scatter(a, 25, MPI_INT, local, 25, MPI_INT, 0, MPI_COMM_WORLD);

		for(int i = 0; i < 25; i++)
		{
			local[i] += 45;
		}
		MPI_Gather(local, 25, MPI_INT, a, 25, MPI_INT, 0, MPI_COMM_WORLD);
		

	if(rank == 0)
		for(int i = 0; i < 100; i++)
			printf("%d ", a[i]);
	

	MPI_Finalize();
	return 0;
}