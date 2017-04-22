#include <mpi.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <time.h>

using namespace std;

int main(int argc, char **argv)
{
//double MPI_Wtime(void);
//double start, finish;
//start = MPI_Wtime();
clock_t t;
int rank, size;
MPI_Init( &argc, &argv );
MPI_Comm_rank( MPI_COMM_WORLD, &rank );
MPI_Comm_size( MPI_COMM_WORLD, &size );
double * x;
int i, j;
int local_m = 3; //3
int local_n = 4; //4
int n=4; //4
double local_y[12]; //12
MPI_Comm comm;

t = clock();

double A[12]; //12
for(int i=0;i<12;i++) //12
A[i]=i;

/*for (i = 0; i < 12; i++)
cout << A[i] << " ";
cout << endl;*/

double local_x[12]; //12
for(int i=0;i<4;i++) //4
local_x[i]=i;

/*for (i = 0; i < 4; i++)
cout << local_x[i] << " ";
cout << endl;*/


x = (double*)malloc(n*sizeof(double));
MPI_Allgather(local_x, local_n, MPI_DOUBLE, x, local_n, MPI_DOUBLE, MPI_COMM_WORLD);
for (i = 0; i < local_m; i++)
    {
    local_y[i] = 0;
    for (j = 0; j < n; j++)
	local_y[i] += A[i*n+j]*x[j];
    }
//finish = MPI_Wtime();
//free(x);
/*for (i = 0; i < 3; i++)
cout << local_y[i] << " ";
cout << endl;*/
t = clock() - t;
cout << (t/1000000.0) << endl;
MPI_Finalize();

return 0;
}
