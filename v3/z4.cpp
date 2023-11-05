#include <stdio.h>
#include <mpi.h>

using namespace std;

int returnSize(char* fname)
{
	FILE* f = fopen(fname, "r");
	int dim = 0;
	double tmp;
	while (fscanf(f, "%lf", &tmp) != EOF)
	{
		dim++;
	}
	fclose(f);
	return dim;
}

double* loadVec(char* fname, int n)
{
	FILE* f = fopen(fname , "r");
	double *res = new double [n];
	double *it = res;
	while (fscanf(f, "%lf", it++) != EOF);
	fclose(f);
	return res;
}

double* loadMat(char* fname, int n, int dim_true)
{
	FILE* f = fopen(fname, "r");
	double* res = new double [n*n];
	double* it = res;
	int cnt = 0;
	
	while(fscanf(f, "%lf", it++) != EOF)
	{
		cnt++;
		while(cnt >= dim_true && cnt < n) 
		{
			it++;
			cnt++;
			if(cnt == n) cnt = 0; 
		}
		
	}

	/*
	for(int i = 0; i < n*n; i++)
	{
		if(i % n == 0) printf("\n");
		printf("%.0f ", res[i]);
	}
	*/
	
	fclose(f);
	return res;
}

void logRes(const char* fname, double* res, int n)
{
	FILE* f = fopen(fname, "w");
	for (int i = 0; i != n; ++i)
		fprintf(f, "%lf ", res[i]);
	fclose(f);
	
}

int main(int argc, char* argv[]) 
{
	
	int csize, prank;
	int dim_true, temp_dim;
		
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &csize);
	MPI_Comm_rank(MPI_COMM_WORLD, &prank);

	char* vfname = argv [1];
	char* mfname = argv [2];

	int dim;
	double* mat;
	double* vec;
	double* tmat;
	double* lres;
	double* res;

	double start = MPI_Wtime();
	
	if (prank == 0)
	{
		dim_true = returnSize(vfname);
		dim = dim_true;
		temp_dim = dim_true;
		
		while(dim % csize != 0)
		{
			dim++;
		}
	}
	
	MPI_Bcast(&dim, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	
	if (prank == 0)
	{
		vec = loadVec(vfname, dim);
		
		while(temp_dim % csize != 0)
		{
			vec[temp_dim] = 0;
			temp_dim++;	
		}
		
	}
	else
		vec = new double[dim];
	
	MPI_Bcast(vec, dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
	if (prank == 0)
	{	
		tmat = loadMat(mfname, dim, dim_true);
	}

	int msize = dim * dim / csize;
	mat = new double[msize];
	
	MPI_Scatter (tmat, msize, MPI_DOUBLE, mat, msize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	int to = dim / csize;
	printf("[PROC %d] Working on %d batches\n", prank, to);
	lres = new double [to];
	
	for(int i = 0; i != to; ++i)
	{
		double s = 0;
		for(int j = 0; j != dim; ++j)
			s += mat[i * dim + j] * vec[j];
		lres [i] = s;
	}

	if (prank == 0)
		res = new double[dim];

	MPI_Gather(lres, to, MPI_DOUBLE, res, to, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	
	if (prank == 0)
	{
		logRes("res.txt", res, dim_true);
	}
	
	if (prank == 0)
	{
		delete [] tmat ;
		delete [] res ;
	}
	
	delete [] vec ;
	delete [] mat ;
	delete [] lres ;

	double end = MPI_Wtime();
	double duration = end - start;
	double max_duration;
	MPI_Reduce(&duration, &max_duration, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
	
	if (prank == 0)
	{
		printf("[MPI] Time spent is %f s\n", max_duration);
	}
	
	MPI_Finalize();

	return 0;
}
