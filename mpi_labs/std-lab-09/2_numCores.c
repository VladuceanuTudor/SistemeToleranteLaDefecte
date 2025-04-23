#include <stdio.h>
#include <unistd.h>

int main()
{
	// Obține numărul de core-uri disponibile (core-uri logice)
	long num_cores = sysconf(_SC_NPROCESSORS_ONLN);

	if (num_cores == -1)
	{
		perror("sysconf(_SC_NPROCESSORS_ONLN) a eșuat");
		return 1;
	}

	printf("Numărul de core-uri disponibile: %ld\n", num_cores);
	return 0;
}