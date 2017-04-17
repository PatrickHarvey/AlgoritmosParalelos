#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "my_rand.h"
#include "timer.h"

const int MAX_KEY = 100000000;

struct list_node_s {
   int    data;
   struct list_node_s* next;
};

struct list_node_s* head = NULL;  
int num_hilos;
int total_ops;
double insert_percent;
double search_percent;
double delete_percent;
pthread_mutex_t mutex;
pthread_mutex_t count_mutex;
int total_miembro=0, total_insercion=0, total_borrado=0;

void Usage(char* prog_name);
void Get_input(int* inserts_in_main_p);

void* Thread_work(void* rank);

int Insert(int value);
void Print(void);
int Member(int value);
int Delete(int value);
void Free_list(void);
int Is_empty(void);

int main(int argc, char* argv[]) {
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
   while ( i < inserts_in_main && attempts < 2*inserts_in_main ) {
      key = my_rand(&seed) % MAX_KEY;
      success = Insert(key);
      attempts++;
      if (success) i++;
   }
   printf("%ld claves insertadas\n", i);

#  ifdef OUTPUT
   printf("Antes de crear hilos, list = \n");
   Print();
   printf("\n");
#  endif

   thread_handles = malloc(num_hilos*sizeof(pthread_t));
   pthread_mutex_init(&mutex, NULL);
   pthread_mutex_init(&count_mutex, NULL);

   GET_TIME(inicio);
   for (i = 0; i < num_hilos; i++)
      pthread_create(&thread_handles[i], NULL, Thread_work, (void*) i);

   for (i = 0; i < num_hilos; i++)
      pthread_join(thread_handles[i], NULL);
   GET_TIME(fin);
   printf("Tiempo de ejecución = %e segundos\n", fin - inicio);
   printf("Total operaciones = %d\n", total_ops);
   printf("Operaciones miembro = %d\n", total_miembro);
   printf("Operaciones insercion= %d\n", total_insercion);
   printf("Operaciones borrado= %d\n", total_borrado);

#  ifdef OUTPUT
   printf("Despues que los hilos finalizan , lista = \n");
   Print();
   printf("\n");
#  endif

   Free_list();
   pthread_mutex_destroy(&mutex);
   pthread_mutex_destroy(&count_mutex);
   free(thread_handles);

   return 0;
}

void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <num_hilos>\n", prog_name);
   exit(0);
}

void Get_input(int* inserts_in_main_p) {

   printf("Número de claves a insertar:\n");
   scanf("%d", inserts_in_main_p);
   printf("Número total de operaciones:\n");
   scanf("%d", &total_ops);
   printf("Porcentaje de operaciones de búsqueda:\n");
   scanf("%lf", &search_percent);
   printf("Porcentaje de operaciones de inserción:\n");
   scanf("%lf", &insert_percent);
   delete_percent = 1.0 - (search_percent + insert_percent);
}

int Insert(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   struct list_node_s* temp;
   int rv = 1;
   
   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }

   if (curr == NULL || curr->data > value) {
      temp = malloc(sizeof(struct list_node_s));
      temp->data = value;
      temp->next = curr;
      if (pred == NULL)
         head = temp;
      else
         pred->next = temp;
   } else {
      rv = 0;
   }

   return rv;
}

void Print(void) {
   struct list_node_s* temp;

   printf("lista = ");

   temp = head;
   while (temp != (struct list_node_s*) NULL) {
      printf("%d ", temp->data);
      temp = temp->next;
   }
   printf("\n");
}


int  Member(int value) {
   struct list_node_s* temp;

   temp = head;
   while (temp != NULL && temp->data < value)
      temp = temp->next;

   if (temp == NULL || temp->data > value) {
#     ifdef DEBUG
      printf("%d NO está en la lista\n", value);
#     endif
      return 0;
   } else {
#     ifdef DEBUG
      printf("%d está en la lista\n", value);
#     endif
      return 1;
   }
}

int Delete(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   int rv = 1;

   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }
   
   if (curr != NULL && curr->data == value) {
      if (pred == NULL) {
         head = curr->next;
#        ifdef DEBUG
         printf("Liberando %d\n", value);
#        endif
         free(curr);
      } else { 
         pred->next = curr->next;
#        ifdef DEBUG
         printf("Liberando %d\n", value);
#        endif
         free(curr);
      }
   } else {
      rv = 0;
   }

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

int  Is_empty(void) {
   if (head == NULL)
      return 1;
   else
      return 0;
}

void* Thread_work(void* rank) {
   long my_rank = (long) rank;
   int i, val;
   double which_op;
   unsigned seed = my_rank + 1;
   int my_member=0, my_insert=0, my_delete=0;
   int ops_per_thread = total_ops/num_hilos;

   for (i = 0; i < ops_per_thread; i++) {
      which_op = my_drand(&seed);
      val = my_rand(&seed) % MAX_KEY;
      if (which_op < search_percent) {
         pthread_mutex_lock(&mutex);
         Member(val);
         pthread_mutex_unlock(&mutex);
         my_member++;
      } else if (which_op < search_percent + insert_percent) {
         pthread_mutex_lock(&mutex);
         Insert(val);
         pthread_mutex_unlock(&mutex);
         my_insert++;
      } else {
         pthread_mutex_lock(&mutex);
         Delete(val);
         pthread_mutex_unlock(&mutex);
         my_delete++;
      }
   }

   pthread_mutex_lock(&count_mutex);
   total_miembro += my_member;
   total_insercion += my_insert;
   total_borrado += my_delete;
   pthread_mutex_unlock(&count_mutex);

   return NULL;
}
