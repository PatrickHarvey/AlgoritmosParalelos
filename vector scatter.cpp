#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main()
{
int n=10;
char vector[]={"vector"};
double local_a[10];
for (int i = 0; i < n; i++)
local_a[i]=i;
int local_n =10;

int my_rank, comm_sz;
MPI_Init(NULL, NULL);
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

double a[10];
int i;

if (my_rank == 0) 
{
//a = malloc(n*sizeof(double));
printf("Enter the vector %s\n", vector);
for (i = 0; i < n; i++)
scanf("%lf", &a[i]);
MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
//free(a);
}
else
{
MPI_Scatter(a, local_n, MPI_DOUBLE, local_a, local_n,MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

for (int i = 0; i < n; i++)
cout << local_a[i] << " ";
cout << endl;
MPI_Finalize();
return 0;
}
