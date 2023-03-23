#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#define LOOP 128
#define FILA "/fila"
int main(void) {
  mqd_t fila;
  struct mq_attr attr;
  char msg;
  attr.mq_maxmsg = 32;
  attr.mq_msgsize = sizeof(msg);
  attr.mq_flags = 0;
  if ((fila = mq_open(FILA, O_RDWR | O_CREAT, 0666, &attr)) < 0) {
    perror("mq_open");
    exit(1);
  }
  for (int c = 0; c< LOOP; c++) {
    if ((mq_receive(fila, (void *)&msg, sizeof(msg), 0)) < 0) {
      perror("mq_receive:");
      exit(1);
    }
    printf("Mensagem recebida %c\n", msg);
  }
  mq_close(fila);
  return 0;
}
