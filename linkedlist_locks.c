#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "my_rand.h"
#include "timer.h"


const int MAX_KEY = 100000000;

struct list_node_s
{
   int    data;
   struct list_node_s* next;
};

struct list_node_s* head = NULL;  
int num_hilos;
int total_ops;
double porcentaje_insertar;
double porcentaje_busqueda;
double porcentaje_borrado;
pthread_rwlock_t rwlock;
pthread_mutex_t count_mutex;
int member_count = 0, insert_count = 0, delete_count = 0;

void Usage(char* prog_name);
void Get_input(int* inserts_in_main_p);

void* Thread_work(void* rank);

int Insert(int value);
void Print(void);
int Member(int value);
int Delete(int value);
void Free_list(void);
int Is_empty(void);


int main(int argc, char* argv[])
{
long i; 
int key, success, attempts;
pthread_t* thread_handles;
int inserts_in_main;
unsigned seed = 1;
double inicio, fin;

if (argc != 2) Usage(argv[0]);
num_hilos = strtol(argv[1],NULL,10);

Get_input(&inserts_in_main);

i = attempts = 0;
while ( i < inserts_in_main && attempts < 2*inserts_in_main )
{
      key = my_rand(&seed) % MAX_KEY;
      success = Insert(key);
      attempts++;
      if (success) i++;
}
   printf("%ld keys insertadas\n", i);

#  ifdef OUTPUT
   printf("Antes de crear hilos, y lista = \n");
   Print();
   printf("\n");
#  endif

thread_handles = malloc(num_hilos*sizeof(pthread_t));
pthread_mutex_init(&count_mutex, NULL);
pthread_rwlock_init(&rwlock, NULL);
GET_TIME(inicio);
for (i = 0; i < num_hilos; i++)
   pthread_create(&thread_handles[i], NULL, Thread_work, (void*) i);

for (i = 0; i < num_hilos; i++)
   pthread_join(thread_handles[i], NULL);
GET_TIME(fin);
printf("Tiempo de ejecución = %e segundos\n", fin - inicio);
printf("Total operaciones = %d\n", total_ops);
printf("Operaciones miembro = %d\n", member_count);
printf("Operaciones insertar = %d\n", insert_count);
printf("Operaciones borrado = %d\n", delete_count);

#  ifdef OUTPUT
printf("Despues de finalizar los hilos, lista = \n");
Print();
printf("\n");
#  endif

Free_list();
pthread_rwlock_destroy(&rwlock);
pthread_mutex_destroy(&count_mutex);
free(thread_handles);
return 0;
}

void Usage(char* prog_name)
{
   fprintf(stderr, "usage: %s <num_hilos>\n", prog_name);
   exit(0);
}

void Get_input(int* inserts_in_main_p)
{
printf("Número de claves a insertar:\n");
scanf("%d", inserts_in_main_p);
printf("Número total de operaciones:\n");
scanf("%d", &total_ops);
printf("Porcentaje de operaciones de búsqueda:\n");
scanf("%lf", &porcentaje_busqueda);
printf("Porcentaje de operaciones de inserción:\n");
scanf("%lf", &porcentaje_insertar);
porcentaje_borrado = 1.0 - (porcentaje_busqueda + porcentaje_insertar);
}

int Insert(int value)
{
struct list_node_s* curr = head;
struct list_node_s* pred = NULL;
struct list_node_s* temp;
int rv = 1;

while (curr != NULL && curr->data < value)
{
   pred = curr;
   curr = curr->next;
}

if (curr == NULL || curr->data > value)
{
temp = malloc(sizeof(struct list_node_s));
temp->data = value;
temp->next = curr;
if (pred == NULL)
      head = temp;
   else
      pred->next = temp;
} 
else 
   rv = 0;

return rv;
}

void Print(void)
{
struct list_node_s* temp;

printf("lista = ");
temp = head;
while (temp != (struct list_node_s*) NULL)
  {
   printf("%d ", temp->data);
   temp = temp->next;
  }
printf("\n");
}

int  Member(int value)
{
struct list_node_s* temp;

temp = head;
while (temp != NULL && temp->data < value)
   temp = temp->next;

if (temp == NULL || temp->data > value)
{
#     ifdef DEBUG
   printf("%d NO está en la lista\n", value);
#     endif
   return 0;
}
else
{
#     ifdef DEBUG
   printf("%d está en la lista\n", value);
#     endif
   return 1;
}
}

int Delete(int value)
{
struct list_node_s* curr = head;
struct list_node_s* pred = NULL;
int rv = 1;

while (curr != NULL && curr->data < value)
{
  pred = curr;
  curr = curr->next;
}
   
if (curr != NULL && curr->data == value)
{
   if (pred == NULL)
   {
      head = curr->next;
#     ifdef DEBUG
      printf("Liberando %d\n", value);
#     endif
      free(curr);
   }
   else
   { 
      pred->next = curr->next;
#     ifdef DEBUG
      printf("Liberando %d\n", value);
#     endif
      free(curr);
   }
}
else
  rv = 0;
return rv;
}

void Free_list(void) {
   struct list_node_s* current;
   struct list_node_s* following;

   if (Is_empty()) return;
   current = head; 
   following = current->next;
   while (following != NULL) {
#     ifdef DEBUG
      printf("Liberando %d\n", current->data);
#     endif
      free(current);
      current = following;
      following = current->next;
   }
#  ifdef DEBUG
   printf("Liberando %d\n", current->data);
#  endif
   free(current);
}

int  Is_empty(void)
{
if (head == NULL)
   return 1;
else
   return 0;
}
void* Thread_work(void* rank)
{
long my_rank = (long) rank;
int i, val;
double which_op;
unsigned seed = my_rank + 1;
int my_member_count = 0, my_insert_count=0, my_delete_count=0;
int ops_per_thread = total_ops/num_hilos;

for (i = 0; i < ops_per_thread; i++)
{
   which_op = my_drand(&seed);
   val = my_rand(&seed) % MAX_KEY;
   if (which_op < porcentaje_busqueda)
   {
      pthread_rwlock_rdlock(&rwlock);
      Member(val);
      pthread_rwlock_unlock(&rwlock);
      my_member_count++;
   }
   else if (which_op < porcentaje_busqueda + porcentaje_insertar)
   {
      pthread_rwlock_wrlock(&rwlock);
      Insert(val);
      pthread_rwlock_unlock(&rwlock);
      my_insert_count++;
   }
   else
   {
      pthread_rwlock_wrlock(&rwlock);
      Delete(val);
      pthread_rwlock_unlock(&rwlock);
      my_delete_count++;
   }
}

pthread_mutex_lock(&count_mutex);
member_count += my_member_count;
insert_count += my_insert_count;
delete_count += my_delete_count;
pthread_mutex_unlock(&count_mutex);
return NULL;
}
