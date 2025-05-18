#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int rank;
	int nProcesses;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	
	char *rasp = malloc(100);
	if (rank == 0)
		{
			int nrAles=50;
			
			while(1){
				printf("A fost testat numarul: %d\n", nrAles);
				// scanf("%d", &nrAles);
				
				MPI_Send(&nrAles, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
				MPI_Recv(rasp, 100, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				printf("%s\n", rasp);
				if(strcmp(rasp, "Corect") == 0)
				break;
				if(strcmp(rasp, "Mai mare") == 0)
					nrAles+=nrAles/2;
				else if(strcmp(rasp, "Mai mic") == 0)
					nrAles-=nrAles/2;

			}
		}
	if (rank == 1)
		{
			srand(time(NULL));
			int nr = rand()%100;
			int nrAles;
			while(1){
				MPI_Recv(&nrAles, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				if (nrAles < nr)
					strcpy(rasp, "Mai mare");
				else if (nrAles > nr)
					strcpy(rasp, "Mai mic");
				else
					strcpy(rasp, "Corect");
				MPI_Send(rasp, 100, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
				if(strcmp(rasp, "Corect") == 0)
					break;
			}

		}
	MPI_Finalize();
	return 0;
}