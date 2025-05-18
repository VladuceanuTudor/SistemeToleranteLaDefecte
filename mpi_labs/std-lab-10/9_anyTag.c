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
	
	if(rank == 0){
		int trim = 0;
		for(int i=1; i<=3; i++){
			MPI_Send(&trim, 1, MPI_INT, 1, i, MPI_COMM_WORLD);
			trim++;
		}
	}
	else if(rank == 1){
		int trim = 0;
		for(int i=1; i<=3; i++){
			MPI_Recv(&trim, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			printf("Received %d from process %d with tag %d\n", trim, status.MPI_SOURCE, status.MPI_TAG);
		}
	}


	MPI_Finalize();
	return 0;
}