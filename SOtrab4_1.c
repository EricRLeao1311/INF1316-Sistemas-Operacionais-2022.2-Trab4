// Nome: Eric Leão Matrícula: 2110694
// Nome: Pedro Machado Peçanha Matrícula: 2110535

#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
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
  int segmento1, segmento2;
  sem_t *mutex;
  sem_t *item;
  sem_t *vaga;
  char *buffer;
  int *count;

  segmento1 = shmget(IPC_PRIVATE, BUFFER * sizeof(char),
                     IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  segmento2 = shmget(IPC_PRIVATE, sizeof(int),
                     IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  mutex = sem_open("mutex", O_CREAT, 0666, 0);
  item = sem_open("item", O_CREAT, 0666, 0);
  vaga = sem_open("vaga", O_CREAT, 0666, 0);
  buffer = (char *)shmat(segmento1, 0, 0);
  count = (int *)shmat(segmento2, 0, 0);
  *count = 0;
  sem_init(mutex, 1, 1);
  sem_init(item, 1, 0);
  sem_init(vaga, 1, BUFFER);
  srand(time(NULL));
  pid_t pid1, pid2;
  int status;
  if ((pid2 = fork()) < 0) {
    printf("Erro\n");
    exit(-1);
  }
  if (pid2 == 0) {
    char aux;
    for (int c = 0; c < NUM_RODADAS; c++) {
      sem_wait(vaga);
      sem_wait(mutex);
      aux = (rand() % (26)) + 65;
      buffer[*count] = aux;
      *count += 1;
      printf("Produtor produziu %c (%d)\n", aux, c);
      sem_post(mutex);
      sem_post(item);
      sleep(1);
    }
    exit(1);
  }
  if ((pid1 = fork()) < 0) {
    printf("Erro\n");
    exit(-1);
  }
  sleep(10);
  if (pid1 == 0) {
    char aux;
    for (int c = 0; c < NUM_RODADAS; c++) {
      sem_wait(item);
      sem_wait(mutex);
      *count -= 1;
      aux = buffer[*count];
      printf("Consumidor consumiu %c (%d)\n", aux, c);
      sem_post(mutex);
      sem_post(vaga);
      sleep(2);
    }
    exit(1);
  }

  for (int i = 0; i < 2; i++)
    wait(&status);

  printf("Main: Programa coompletado. Saindo.\n");
  return 0;
}
