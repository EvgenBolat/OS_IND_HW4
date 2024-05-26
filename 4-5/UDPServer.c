#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <unistd.h>     /* for close() */

#define MAXPENDING 5 /* Maximum outstanding connection requests */
#define RCVBUFSIZE 1 /* Size of receive buffer */

void HandleUDPClient(int clntSocket, struct sockaddr_in echoClntAddr,
                     pid_t pid); /* TCP client handling function */

int main(int argc, char *argv[]) {
  char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
  int servSock;                    /* Socket descriptor for server */
  int clntSock;                    /* Socket descriptor for client */
  struct sockaddr_in echoServAddr; /* Local address */
  struct sockaddr_in echoClntAddr; /* Client address */
  unsigned short echoServPort;     /* Server port */
  unsigned int cliAddrLen,
      recvMsgSize; /* Length of client address data structure */

  if (argc != 3) /* Test for correct number of arguments */
  {
    fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
    exit(1);
  }
  int pid = fork();
  if (pid == -1) {
    printf("Что-то случилось");
    return 0;
  } else if (pid == 0) {
    usleep(1000);
    echoServPort =
        atoi(argv[1]); /* First arg:  local port for the first department */
  } else {
    echoServPort =
        atoi(argv[2]); /* Second arg:  local port for the second department */
  }

  /* Create socket for incoming connections */
  if ((servSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    printf("socket() failed");
    exit(1);
  }

  /* Construct local address structure */
  memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
  echoServAddr.sin_family = AF_INET;              /* Internet address family */
  echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
  echoServAddr.sin_port = htons(echoServPort);      /* Local port */

  /* Bind to the local address */
  if (bind(servSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) <
      0) {
    printf("bind() failed");
    exit(1);
  }

  cliAddrLen = sizeof(echoClntAddr);
  for (;;) /* Run forever */
  {
    printf("Продавец %d: Ожидаю нового покупателя\n", pid);
    echoBuffer[0] = '3';
    while (echoBuffer[0] != '0') {
      if ((recvMsgSize =
               recvfrom(servSock, echoBuffer, RCVBUFSIZE, 0,
                        (struct sockaddr *)&echoClntAddr, &cliAddrLen)) < 0) {
        printf("recvfrom() failed");
        exit(1);
      }
      usleep(100);
    }
    printf("Продавец %d: Покупатель появился\n", pid);
    HandleUDPClient(servSock, echoClntAddr, pid);
    printf("Продавец %d: Покупатель ушел\n", pid);
    putchar('\n');
    echoBuffer[0] = '1';
  }
  /* NOT REACHED */
}
