#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main()
{
int n=10;
char vector[]={"vector"};
double local_b[10];
for (int i = 0; i < n; i++)
local_b[i]=i;
int local_n =10;
double b[10];
int i;

int my_rank, comm_sz;
MPI_Init(NULL, NULL);
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

if (my_rank == 0)
{
//b = malloc(n∗sizeof(double));
MPI_Gather(local_b, local_n, MPI_DOUBLE, b, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
printf("%s\n", vector);
for (i = 0; i < n; i++)
cout << local_b[i] << " ";
cout << endl;
//free(b);
}
else
{
MPI_Gather(local_b, local_n, MPI_DOUBLE, b, local_n, MPI_DOUBLE, 0,MPI_COMM_WORLD );
}

for (int i = 0; i < n; i++)
cout << b[i] << " ";
cout << endl;

MPI_Finalize();
return 0;
}
