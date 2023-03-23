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
  sem_init(mutex, 1, 1);
  sem_init(item, 1, 0);
  sem_init(vaga, 1, BUFFER);
  aux_count = shm_open("count", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  erro = ftruncate(aux_count, sizeof(int));
  if (erro != 0){
    printf("Erro!\n");
  }
  count = mmap(NULL, sizeof(int), PROT_WRITE, MAP_SHARED, aux_count, 0);
  *count = 0;
  aux_buff = shm_open("buff", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  erro = ftruncate(aux_buff, 32 * sizeof(char));
  if (erro != 0){
    printf("Erro!\n");
  }
  buffer = mmap(NULL, sizeof(int), PROT_WRITE, MAP_SHARED, aux_buff, 0);
  sem_init(mutex, 1, 1);
  sem_init(item, 1, BUFFER);
  sem_init(vaga, 1, BUFFER);
  srand(time(NULL));
  pid_t pid2;
  int status;
  if ((pid2 = fork()) < 0) {
    printf("Erro\n");
    exit(-1);
  }
  if (pid2 == 0) {
    char aux;
    for (int c = 0; c < NUM_RODADAS; c++) {
      sleep(1);
      sem_wait(vaga);
      sem_wait(mutex);
      aux = (rand() % (26)) + 65;
      buffer[*count] = aux;
      *count += 1;
      printf("Produtor produziu %c (%d)\n", aux, c);
      sem_post(mutex);
      sem_post(item);
    }
    exit(1);
  }

  wait(&status);

  printf("Main: Programa completado. Saindo.\n");
  return 0;
}
