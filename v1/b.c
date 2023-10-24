#include <stdio.h>
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

	double i = (double)((prank % csize) * (int)((int)n / csize) + (prank % (int)n)); 

	printf("[PROC %d] %d * %d + %d \n", prank, prank%csize, (int)((int)n/(int)csize), prank%(int)n);	
	// c0 0 0
	// c1 1 2
	// c2 2 3
	// c3 3 4
	//
	// n = 6	
	// (6 % 4) - (6 / 4) = 1
	
	double limit = (double)(((int)n % csize) - (int)((int)n % csize));
	
	if((int)n % csize > (double)prank)
	{
		limit++;
	}

	limit = i + limit;

	while(i <= limit)
	{
		printf("[PROC %d] Element %lf to sum %lf\n", prank, i+1, sum);
		sum += i+1;
		i++;
	}

	/*
	while(i < prank % csize * (int)(n / csize) + (int)(n / csize))
	{
		printf("[PROC %d] Element %lf to sum %lf\n", prank, i+1, sum);
		sum += i+1;
		i++;
	}

	if(prank == (csize - 1) && ((int)n % 2 == 1))
	{
		printf("[PROC %d] Element %lf to sum %lf\n", prank, i+1, sum);
		sum += i+1;
		i++;	
	}
	*/
	
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
