#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <unistd.h>     /* for close() */

#define MAXPENDING 10 /* Maximum outstanding connection requests */

void HandleUDPClient(int servSock, struct sockaddr_in echoClntAddr, pid_t pid,
                     int lstnrPort); /* TCP client handling function */

#define RCVBUFLSTNSIZE 80
#define RCVBUFSIZE 1 /* Size of receive buffer */
struct sockaddr_in echoListnerServAddr;
char lstnerBuffer[RCVBUFLSTNSIZE];
int listinerServPort;

int lstnsock;

void makeServerLog(char *sending) {
  if ((lstnsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    printf("socket() failed");
    exit(0);
  }
  int curr = 0;
  char my_pid[6];
  sprintf(my_pid, "%d", getpid());
  char *seller_word = "Продавец";
  for (; curr < strlen(seller_word); curr++) {
    lstnerBuffer[curr] = seller_word[curr];
  }
  lstnerBuffer[curr++] = ' ';
  for (int j = 0; j < strlen(my_pid); j++) {
    lstnerBuffer[curr++] = my_pid[j];
  }
  lstnerBuffer[curr++] = ':';
  lstnerBuffer[curr++] = ' ';
  char buffer[RCVBUFLSTNSIZE];
  sprintf(buffer, "%s", sending);
  for (int j = 0; j < strlen(buffer); j++) {
    lstnerBuffer[curr++] = buffer[j];
  }
  lstnerBuffer[curr] = '\0';
  if (sendto(lstnsock, lstnerBuffer, RCVBUFLSTNSIZE, 0,
             (struct sockaddr *)&echoListnerServAddr,
             sizeof(echoListnerServAddr)) != RCVBUFLSTNSIZE) {
    printf("something goes wrong");
  }
}

int main(int argc, char *argv[]) {
  char echoBuffer[RCVBUFSIZE];     /* Buffer for echo string */
  int servSock;                    /* Socket descriptor for server */
  int clntSock;                    /* Socket descriptor for client */
  struct sockaddr_in echoServAddr; /* Local address */
  struct sockaddr_in echoClntAddr; /* Client address */
  unsigned short echoServPort;     /* Server port */
  unsigned int clntLen, recvMsgSize,
      cliAddrLen; /* Length of client address data structure */

  if (argc != 5) /* Test for correct number of arguments */
  {
    fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
    exit(1);
  }
  /* Construct the server address structure */
  memset(&echoListnerServAddr, 0,
         sizeof(echoListnerServAddr));      /* Zero out structure */
  echoListnerServAddr.sin_family = AF_INET; /* Internet address family */
  echoListnerServAddr.sin_addr.s_addr =
      inet_addr(argv[3]); /* Server IP address */
  echoListnerServAddr.sin_port = htons(atoi(argv[4]));
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
  pid = getpid();

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
    makeServerLog("Ожидаю покупателя");
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
      makeServerLog("Покупатель появился");
      printf("Продавец %d: Покупатель появился\n", pid);
      HandleUDPClient(servSock, echoClntAddr, pid, listinerServPort);
      makeServerLog("Покупатель ушёл");
      printf("Продавец %d: Покупатель ушел\n", pid);
      putchar('\n');
      echoBuffer[0] = '1';
    }
  }
  /* NOT REACHED */
}
