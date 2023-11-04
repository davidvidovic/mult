#include <stdio.h>
#include <math.h>
#include <mpi.h>

double getInput()
{
	double res;
	printf("Number: ");
	fflush(stdout);
	scanf("%lf", &res);
	return (double)(res);
}

int main(int argc,char* argv[])
{
	double n;
	double sum = 0;
	int csize, prank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &csize);
	MPI_Comm_rank(MPI_COMM_WORLD, &prank);
	
	if(prank == 0)
	{
		n = getInput();
	}
	//MPI_Barrier(MPI_COMM_WORLD);
	MPI_Bcast(&n, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	double s = MPI_Wtime();
	
	// Block size - size of a block od data
	// Example: csize = 4, n = 6
	// Cores should work like:
	// core0 - 1 + 2
	// core1 - 3 + 4
	// core2 - 5
	// core3 - 6
	// Block_size is 2, so worker cores (busier cores) should get 2 jobs, all the others get 1
	int block_size = ceil(n*(1.0) / csize);
	
	// Workers defines how many cores will be busier than others
	// If workers == 0, all the cores are equaly busy
	int workers = (int)n % csize;
	int start, end;

	// Assign all the cores below workers treshold to be busier than others
	// Or in case where global job can be spread equaly among all cores (it divisible)
	if(prank < workers || workers == 0)
	{
		start = prank * block_size + 1;
		end = start + block_size;
	}	
	else
	{
		start = workers * block_size + (prank - workers) * (block_size - 1) + 1;
		end = start + block_size - 1;
	}
	
	for(int i = start; i < end; i++)
	{
		printf("[PROC %d] Sum is %.1f. Adding %d. ", prank, sum, i);
		sum += i;
		printf("Sum is now %.1f\n", sum);
	}
	
	double tsum;
	MPI_Reduce(&sum, &tsum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	double e = MPI_Wtime();
	double d = e - s;
	double mind;
	MPI_Reduce(&d, &mind, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	if(prank == 0)
	{
		printf("Sum first %f integer is %f\n",n,tsum);
		printf("Elapsed time: %f\n",d);
	}
	MPI_Finalize();
	return 0;
}
