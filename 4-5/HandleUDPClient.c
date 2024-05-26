#include <arpa/inet.h>
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <stdlib.h>
#include <unistd.h>     /* for close() */

#define RCVBUFSIZE 1 /* Size of receive buffer */



void HandleUDPClient(int socket, struct sockaddr_in echoClntAddr, pid_t pid) {
  char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */
  printf("Продавец %d: Сообщаю, что принял заказ\n", pid);
  echoBuffer[0] = '1';
  if (sendto(socket, echoBuffer, RCVBUFSIZE, 0,
             (struct sockaddr *)&echoClntAddr,
             sizeof(echoClntAddr)) != RCVBUFSIZE) {
    printf("sendto() sent a different number of bytes than expected");
  }
  echoBuffer[0] = '2';
  printf("Продавец %d: Сообщаю, что провожу пробивание товара на кассе\n", pid);
  usleep(2000000);
  printf("Продавец %d: Сообщаю, что выполнил заказ\n", pid);
  if (sendto(socket, echoBuffer, RCVBUFSIZE, 0,
             (struct sockaddr *)&echoClntAddr,
             sizeof(echoClntAddr)) != RCVBUFSIZE){
    printf("sendto() sent a different number of bytes than expected");
    exit(1);
  }
  printf("Продавец %d: Завершаю общение с данным покупателем\n", pid);
}
