#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define LOOP 128
#define FILA "/fila"
int main(void) {
  mqd_t fila;
  char msg;
  if ((fila = mq_open(FILA, O_RDWR)) < 0) {
    perror("mq_open");
    exit(1);
  }
  srand(time(NULL));
  for (int c = 0; c< LOOP; c++) {
    msg = (rand() % (26)) + 65;
    if (mq_send(fila, (void *)&msg, sizeof(msg), 0) < 0) {
      perror("mq_send");
      exit(1);
    }
    printf("Valor enviado %c\n", msg);
    sleep(1);
  }
  mq_close(fila);
  return 0;
}
