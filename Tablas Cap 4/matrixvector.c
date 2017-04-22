#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

int     num_hilos;
int     m, n;
double* A;
double* x;
double* y;
void Usage(char* prog_name);
void Generar_matrix(double A[], int m, int n);
void Read_matrix(char* prompt, double A[], int m, int n);
void Generar_vector(double x[], int n);
void *Pth_mat_vect(void* rank);

int main(int argc, char* argv[])
{
long       thread;
pthread_t* thread_handles;
double start, finish;
if (argc != 4) Usage(argv[0]);
num_hilos = strtol(argv[1], NULL, 10);
m = strtol(argv[2], NULL, 10);
n = strtol(argv[3], NULL, 10);

printf("num_hilos =  %d, m = %d, n = %d\n", num_hilos, m, n);

thread_handles = malloc(num_hilos*sizeof(pthread_t));
A = malloc(m*n*sizeof(double));
x = malloc(n*sizeof(double));
y = malloc(m*sizeof(double));

/*int k;
printf("ingresa el vector de tamanho %i\n", n);
  for (k = 0; k < n; k++) 
     scanf("%lf", &x[k]);

int p, q;
printf("Ingresa la matriz de tamaño %i y %i\n",m,n);
for (p = 0; p < m; p++) 
  for (q = 0; q < n; q++)
    scanf("%lf", &A[p*n+q]);*/
   
Generar_matrix(A, m, n);
Generar_vector(x, n);

   GET_TIME(start);
   for (thread = 0; thread < num_hilos; thread++)
      pthread_create(&thread_handles[thread], NULL,
         Pth_mat_vect, (void*) thread);

   for (thread = 0; thread < num_hilos; thread++)
      pthread_join(thread_handles[thread], NULL);
   GET_TIME(finish);

int l;
printf("El resultado es: ");
/*for(l=0;l<n;l++)
  printf("%6.3f ",y[l]);
printf("\n");*/

   printf("Tiempo de ejecución = %e segundos\n", finish - start);

   free(A);
   free(x);
   free(y);
   free(thread_handles);

   return 0;
}


void Usage (char* prog_name)
{
fprintf(stderr, "usage: %s <num_hilos> <m> <n>\n", prog_name);
exit(0);
}

void Generar_matrix(double A[], int m, int n) {
   int i, j;
   for (i = 0; i < m; i++)
      for (j = 0; j < n; j++)
         A[i*n+j] = random()/((double) RAND_MAX);
}

void Generar_vector(double x[], int n)
{
int i;
for (i = 0; i < n; i++)
   x[i] = random()/((double) RAND_MAX);
}

void *Pth_mat_vect(void* rank)
{
long my_rank = (long) rank;
int i, j;
int local_m = m/num_hilos; 
register int sub = my_rank*local_m*n;
int my_first_row = my_rank*local_m;
int my_last_row = (my_rank+1)*local_m - 1;
double tmp;

printf("Hilo %ld > mi_primera_fila = %d, mi_última_columna = %d\n",my_rank, my_first_row, my_last_row);
printf("Hilo %ld > dirección en memoria = %p\n",my_rank, &tmp);

for (i = my_first_row; i <= my_last_row; i++)
   {
    tmp = 0.0;
    for (j = 0; j < n; j++)
      tmp += A[sub++]*x[j];
    y[i] = tmp;
   }
return NULL;
}
