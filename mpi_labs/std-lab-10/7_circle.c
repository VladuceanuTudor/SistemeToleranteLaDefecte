#include <mpi.h>
#include <stdio.h>



int main(int argc, char **argv)
{
	int rank;
	int nProcesses;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

	int a;
	
	
		if(rank ==0){
			a=1;
			MPI_Send(&a, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
		}else if(rank == nProcesses-1){
			MPI_Recv(&a, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("Process %d received %d from process %d\n", rank, a, rank-1);
			a+=2;
			MPI_Send(&a, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		} else{
			MPI_Recv(&a, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("Process %d received %d from process %d\n", rank, a, rank-1);
			a+=2;
			MPI_Send(&a, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
		}
	

	if(rank == 0){
		MPI_Recv(&a, 1, MPI_INT, nProcesses-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Process %d received %d from process %d\n", rank, a, nProcesses-1);
	}
	
	MPI_Finalize();
	return 0;
}