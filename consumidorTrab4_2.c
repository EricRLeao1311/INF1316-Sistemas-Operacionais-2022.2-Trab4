#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_RODADAS 128
#define BUFFER 32

void produtor();
void consumidor();
char deposita();
char retira();

int main(void) {
  sem_t *mutex;
  sem_t *item;
  sem_t *vaga;
  char *buffer;
  char aux_buff;
  int aux_count;
  int *count;
  int erro;
  mutex = sem_open("mutex", O_CREAT, 0600, 0);
  item = sem_open("item", O_CREAT, 0600, 0);
  vaga = sem_open("vaga", O_CREAT, 0600, 0);
  aux_count = shm_open("count", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  erro = ftruncate(aux_count, sizeof(int));
  if (erro != 0){
    printf("Erro!\n");
  }
  count = mmap(NULL, sizeof(int), PROT_WRITE, MAP_SHARED, aux_count, 0);
  aux_buff = shm_open("buff", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  erro = ftruncate(aux_buff, 32 * sizeof(char));
  if (erro != 0){
    printf("Erro!\n");
  }
  buffer = mmap(NULL, sizeof(int), PROT_WRITE, MAP_SHARED, aux_buff, 0);
  srand(time(NULL));
  pid_t pid1;
  int status;
  if ((pid1 = fork()) < 0) {
    printf("Erro\n");
    exit(-1);
  }
  if (pid1 == 0) {
    char aux;
    for (int c = 0; c < NUM_RODADAS; c++) {
      sleep(1);
      sem_wait(item);
      sem_wait(mutex);
      *count -= 1;
      aux = buffer[*count];
      printf("Consumidor consumiu %c (%d)\n", aux, c);
      sem_post(mutex);
      sem_post(vaga);
    }
    exit(1);
  }

  wait(&status);

  printf("Main: Programa completado. Saindo.\n");
  return 0;
}
