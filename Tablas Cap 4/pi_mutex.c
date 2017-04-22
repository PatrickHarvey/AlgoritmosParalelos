#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "timer.h"

long num_hilos;
long long n;
double sum;
pthread_mutex_t mutex;

void* Thread_sum(void* rank);
void Get_args(int argc, char* argv[]);
void Usage(char* prog_name);
double Serial_pi(long long n);

int main(int argc, char* argv[])
{
long thread;
pthread_t* thread_handles;
double inicio, fin;

Get_args(argc, argv);

thread_handles = (pthread_t*) malloc (num_hilos*sizeof(pthread_t)); 
pthread_mutex_init(&mutex, NULL);

GET_TIME(inicio);
sum = 0.0;
for (thread = 0; thread < num_hilos; thread++)  
   pthread_create(&thread_handles[thread], NULL,Thread_sum, (void*)thread);  

for (thread = 0; thread < num_hilos; thread++) 
   pthread_join(thread_handles[thread], NULL); 
sum = 4.0*sum;
GET_TIME(fin);

printf("n = %lld terminos,\n", n);
printf("Resultado = %.15f\n", sum);
printf("Tiempo de ejecución: %e segundos\n", fin - inicio);
GET_TIME(inicio);
sum = Serial_pi(n);
GET_TIME(fin);
printf("Con un solo hilo  = %.15f\n", sum);
printf("Tiempo de ejecución: %e segundos\n", fin - inicio);
printf("pi = %.15f\n", 4.0*atan(1.0));
   
pthread_mutex_destroy(&mutex);
free(thread_handles);
return 0;
}

void* Thread_sum(void* rank)
{
long my_rank = (long) rank;
double factor;
long long i;
long long my_n = n/num_hilos;
long long my_first_i = my_n*my_rank;
long long my_last_i = my_first_i + my_n;
double my_sum = 0.0;

if (my_first_i % 2 == 0)
   factor = 1.0;
else
   factor = -1.0;

for (i = my_first_i; i < my_last_i; i++, factor = -factor)
   my_sum += factor/(2*i+1);

pthread_mutex_lock(&mutex);
printf("Hilo %ld > mutex\n", my_rank);
fflush(stdout);
sum += my_sum;
pthread_mutex_unlock(&mutex);

return NULL;
}

double Serial_pi(long long n)
{
double sum = 0.0;
long long i;
double factor = 1.0;

for (i = 0; i < n; i++, factor = -factor)
  sum += factor/(2*i+1);

return 4.0*sum;
}

void Get_args(int argc, char* argv[])
{
if (argc != 3) Usage(argv[0]);
   num_hilos = strtol(argv[1], NULL, 10);  
if (num_hilos <= 0) Usage(argv[0]);
   n = strtoll(argv[2], NULL, 10);
if (n <= 0) Usage(argv[0]);
}

void Usage(char* prog_name)
{
   fprintf(stderr, "usage: %s <number of threads> <n>\n", prog_name);
   exit(0);
}
