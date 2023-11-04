#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>


int getInput()
{
	int res;
	printf("Input vector dimension: ");
	fflush(stdout);
	scanf("%d", &res);
	return (int)(res);
}

int main(int argc,char* argv[])
{
	int n;
	int sum = 0;
	int csize, prank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &csize);
	MPI_Comm_rank(MPI_COMM_WORLD, &prank);
	
	if(prank == 0)
	{
		n = getInput();
	}
	//MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	int vec1[n];
	int vec2[n];
	
	if(prank == 0)
	{
		// Set random seed for core 0
		srand(time(NULL) + prank);
		
		// Generate vectors
		for(int i = 0; i < n; i++)
		{
			// Generate a random number beetwen -50 and 49
			// First generate random number in range 0 - 99 and substract 50
			// In order to expend range to negative numbers as well
			vec1[i] = rand() % 100 - 50;
			vec2[i] = rand() % 100 - 50;
			
			//printf("[PROC %d] Iteration %d generated: vec1[%d] = %d and vec1[%d] = %d\n", prank, i, i, vec1[i], i, vec2[i]);
		}
		
		printf("[PROC %d] vec1 = ", prank);
		for(int i = 0; i < n; i++)
		{
			printf("%d ", vec1[i]);
		}
		printf("\n");
		
		printf("[PROC %d] vec2 = ", prank);
		for(int i = 0; i < n; i++)
		{
			printf("%d ", vec2[i]);
		}
		printf("\n");
	}
	
	
	MPI_Bcast(vec1, n, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(vec2, n, MPI_INT, 0, MPI_COMM_WORLD);
	double s = MPI_Wtime();
	
	int i = prank;
	
	while(i < n)
	{
		sum += vec1[i] * vec2[i];
		printf("[PROC %d] %d * %d and local sum is %d\n", prank, vec1[i], vec2[i], sum);
		i += csize;
	}
	
	int tsum;
	MPI_Reduce(&sum, &tsum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	double e = MPI_Wtime();
	double d = e - s;
	double mind;
	MPI_Reduce(&d, &mind, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	if(prank == 0)
	{
		printf("Vector dot product is %d\n", tsum);
		printf("Elapsed time: %f\n", d);
	}
	MPI_Finalize();
	return 0;
}
