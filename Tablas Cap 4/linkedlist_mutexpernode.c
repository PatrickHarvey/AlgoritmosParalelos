#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "my_rand.h"
#include "timer.h"

const int MAX_KEY = 100000000;

const int IN_LIST = 1;
const int EMPTY_LIST = -1;
const int END_OF_LIST = 0;

struct list_node_s
{
int    data;
pthread_mutex_t mutex;
struct list_node_s* next;
};

struct list_node_s* head = NULL;  
pthread_mutex_t head_mutex;
int thread_count;
int total_ops;
double porcentaje_insercion;
double porcentaje_busqueda;
double porcentaje_borrado;
pthread_mutex_t count_mutex;
int total_miembro=0, total_insertar=0, total_borrado=0;

void Usage(char* prog_name);
void Get_input(int* inserts_in_main_p);

void* Thread_work(void* rank);
void Init_ptrs(struct list_node_s** curr_pp, struct list_node_s** pred_pp);
int Advance_ptrs(struct list_node_s** curr_pp,struct list_node_s** pred_pp);
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
thread_count = strtol(argv[1], NULL, 10);
Get_input(&inserts_in_main);

i = attempts = 0;
pthread_mutex_init(&head_mutex, NULL);
while ( i < inserts_in_main && attempts < 2*inserts_in_main )
{
   key = my_rand(&seed) % MAX_KEY;
   success = Insert(key);
   attempts++;
   if (success) i++;
}
printf("%ld keys insertadas\n", i);

#  ifdef OUTPUT
   printf("Antes de crear hilos, lista = \n");
   Print();
   printf("\n");
#  endif

thread_handles = malloc(thread_count*sizeof(pthread_t));
pthread_mutex_init(&count_mutex, NULL);

GET_TIME(inicio);
for (i = 0; i < thread_count; i++)
   pthread_create(&thread_handles[i], NULL, Thread_work, (void*) i);

for (i = 0; i < thread_count; i++)
   pthread_join(thread_handles[i], NULL);
GET_TIME(fin);
printf("Tiempo de ejecución = %e segundos\n", fin - inicio);
printf("Total operaciones = %d\n", total_ops);
printf("Operaciones miembro = %d\n", total_miembro);
printf("Operaciones insertar = %d\n", total_insertar);
printf("Operaciones borrado = %d\n", total_borrado);

#  ifdef OUTPUT
   printf("Despues de terminar los hilos, lista = \n");
   Print();
   printf("\n");
#  endif

Free_list();
pthread_mutex_destroy(&head_mutex);
pthread_mutex_destroy(&count_mutex);
free(thread_handles);

return 0;
}

void Usage(char* prog_name)
{
   fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
   exit(0);
}

/*-----------------------------------------------------------------*/
void Get_input(int* inserts_in_main_p) {

   printf("Número de claves a insertar:\n");
   scanf("%d", inserts_in_main_p);
   printf("Número total de operaciones:\n");
   scanf("%d", &total_ops);
   printf("Porcentaje de operaciones de búsqueda:\n");
   scanf("%lf", &porcentaje_busqueda);
   printf("Porcentaje de operaciones de inserción:\n");
   scanf("%lf", &porcentaje_insercion);
   porcentaje_borrado = 1.0 - (porcentaje_busqueda + porcentaje_insercion);
}

void Init_ptrs(struct list_node_s** curr_pp, struct list_node_s** pred_pp)
{
*pred_pp = NULL;
pthread_mutex_lock(&head_mutex);
*curr_pp = head;
if (head != NULL)
   pthread_mutex_lock(&(head->mutex));
}

int Advance_ptrs(struct list_node_s** curr_pp, struct list_node_s** pred_pp) {
   int rv = IN_LIST;
   struct list_node_s* curr_p = *curr_pp;
   struct list_node_s* pred_p = *pred_pp;

   if (curr_p == NULL)
      if (pred_p == NULL)
         return EMPTY_LIST;
      else 
         return END_OF_LIST;
   else {
      if (curr_p->next != NULL)
         pthread_mutex_lock(&(curr_p->next->mutex));
      else
         rv = END_OF_LIST;
      if (pred_p != NULL)
         pthread_mutex_unlock(&(pred_p->mutex));
      else
         pthread_mutex_unlock(&head_mutex);
      *pred_pp = curr_p;
      *curr_pp = curr_p->next;
      return rv;
   }
} 

int Insert(int value){
struct list_node_s* curr;
struct list_node_s* pred;
struct list_node_s* temp;
int rv = 1;
Init_ptrs(&curr, &pred);
   
while (curr != NULL && curr->data < value)
   Advance_ptrs(&curr, &pred);

if (curr == NULL || curr->data > value) {
#     ifdef DEBUG
      printf("Insertando %d\n", value);
#     endif
      temp = malloc(sizeof(struct list_node_s));
      pthread_mutex_init(&(temp->mutex), NULL);
      temp->data = value;
      temp->next = curr;
      if (curr != NULL) 
         pthread_mutex_unlock(&(curr->mutex));
      if (pred == NULL) {
         head = temp;
         pthread_mutex_unlock(&head_mutex);
      } else {
         pred->next = temp;
         pthread_mutex_unlock(&(pred->mutex));
      }
   } else {
      if (curr != NULL) 
         pthread_mutex_unlock(&(curr->mutex));
      if (pred != NULL)
         pthread_mutex_unlock(&(pred->mutex));
      else
         pthread_mutex_unlock(&head_mutex);
      rv = 0;
   }

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

   pthread_mutex_lock(&head_mutex);
   temp = head;
   while (temp != NULL && temp->data < value) {
      if (temp->next != NULL) 
         pthread_mutex_lock(&(temp->next->mutex));
      if (temp == head)
         pthread_mutex_unlock(&head_mutex);
      pthread_mutex_unlock(&(temp->mutex));
      temp = temp->next;
   }

   if (temp == NULL || temp->data > value) {
#     ifdef DEBUG
      printf("%d NO está en la lista\n", value);
#     endif
      if (temp == head)
         pthread_mutex_unlock(&head_mutex);
      if (temp != NULL) 
         pthread_mutex_unlock(&(temp->mutex));
      return 0;
   } else {
#     ifdef DEBUG
      printf("%d está en la lista\n", value);
#     endif
      if (temp == head)
         pthread_mutex_unlock(&head_mutex);
      pthread_mutex_unlock(&(temp->mutex));
      return 1;
   }
}

int Delete(int value)
{
struct list_node_s* curr;
struct list_node_s* pred;
int rv = 1;

Init_ptrs(&curr, &pred);

while (curr != NULL && curr->data < value)
   Advance_ptrs(&curr, &pred);
   
   if (curr != NULL && curr->data == value) {
      if (pred == NULL) {
         head = curr->next;
#        ifdef DEBUG
         printf("Freeing %d\n", value);
#        endif
         pthread_mutex_unlock(&head_mutex);
         pthread_mutex_unlock(&(curr->mutex));
         pthread_mutex_destroy(&(curr->mutex));
         free(curr);
      } else { 
         pred->next = curr->next;
         pthread_mutex_unlock(&(pred->mutex));
#        ifdef DEBUG
         printf("Liberando %d\n", value);
#        endif
         pthread_mutex_unlock(&(curr->mutex));
         pthread_mutex_destroy(&(curr->mutex));
         free(curr);
      }
   } else {
      if (pred != NULL)
         pthread_mutex_unlock(&(pred->mutex));
      if (curr != NULL)
         pthread_mutex_unlock(&(curr->mutex));
      if (curr == head)
         pthread_mutex_unlock(&head_mutex);
      rv = 0;
   }

   return rv;
}

void Free_list(void)
{
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
int my_member=0, my_insert=0, my_delete=0;
int ops_per_thread = total_ops/thread_count;
for (i = 0; i < ops_per_thread; i++)
   {
   which_op = my_drand(&seed);
   val = my_rand(&seed) % MAX_KEY;
   if (which_op < porcentaje_busqueda)
   {
#     ifdef DEBUG
      printf("Hilo %ld > Buscando %d\n", my_rank, val);
#     endif
      Member(val);
      my_member++;
   }
   else if (which_op < porcentaje_busqueda + porcentaje_insercion)
   {
#     ifdef DEBUG
      printf("Hilo %ld > Insertando %d\n", my_rank, val);
#     endif
      Insert(val);
      my_insert++;
   }
   else 
   {
#     ifdef DEBUG
      printf("Hilo %ld > Borrando %d\n", my_rank, val);
#     endif
      Delete(val);
      my_delete++;
   }
}

   pthread_mutex_lock(&count_mutex);
   total_miembro += my_member;
   total_insertar += my_insert;
   total_borrado += my_delete;
   pthread_mutex_unlock(&count_mutex);

   return NULL;
}
