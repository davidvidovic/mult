#include <stdio.h>
#include <iostream>
#include <chrono> 

using namespace std;
using namespace std::chrono;

int returnSize(char* fname)
{
	FILE* f = fopen(fname, "r");
	int dim = 0;
	double tmp ;
	while (fscanf(f, "%lf", &tmp) != EOF)
		dim++;
	fclose(f);
	return dim;
}

void logRes(const char* fname, double* res, int n)
{
	FILE* f = fopen(fname, "w");
	for (int i = 0; i != n; ++i)
		fprintf(f, "%.1lf ", res[i]);
	fclose(f);
	
}


int main(int argc, char* argv[]) 
{
	int dim = returnSize(argv[1]);
	
	double mat[dim][dim];
	double vec[dim];
	double res[dim];
	double temp;
	int cnt = 0, cnt_row = 0;
	
	high_resolution_clock::time_point start = high_resolution_clock::now();
	
	FILE *f;
	f = fopen(argv[1], "r");
	
	while(fscanf(f, "%lf", &temp) != EOF)
	{
		vec[cnt] = temp;
		cnt++;
	}
	fclose(f);
	
	cnt = 0;
	cnt_row = 0;
	f = fopen(argv[2], "r");
	
	while(fscanf(f, "%lf", &temp) != EOF)
	{
		mat[cnt_row][cnt++] = temp;
		if(cnt == dim) 
		{
			cnt = 0;
			cnt_row++;
		}
	}
	fclose(f);
	
	for(int i = 0; i < dim; i++)
	{
		res[i] = 0;
		for(int j = 0; j < dim; j++)
		{
			res[i] += vec[j] * mat[i][j];
		}
	}
	
	logRes("seq_res.txt", res, dim);
	
	high_resolution_clock::time_point end = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(end - start);
	std::cout << "[SEQ] Time spent is " << time_span.count() << " s\n";
	
	cnt = 0;
	f = fopen("res.txt", "r");
	char flag = 1;
	if(f != NULL)
	{
		while(fscanf(f, "%lf", &temp) != EOF)
		{
			if(res[cnt++] != temp) 
			{
				flag = 0;
				break;
			}
		}
		
		flag ? printf("Results match!\n") : printf("Results DO NOT match!\n");
		
		fclose(f);
	}
	else printf("Not res.txt file!\n");
}
