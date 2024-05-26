#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdio.h>     /* for printf() and fprintf() */
#include <stdlib.h>
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <time.h>   // чтобы использовать функцию time()
#include <unistd.h> /* for close() */

#define RCVBUFSIZE 1 /* Size of receive buffer */
#define RCVBUFLSTNSIZE 80

int Nproc;
pid_t t[100];

int sock, lstnsock; /* Socket descriptor */
struct sockaddr_in echoServAddr, echoListnerServAddr,
    fromAddr; /* Echo server addresses */
unsigned short echoServPort, echoServPort1,
    echoServPort2; /* Echo server port */
int listinerServPort;
char *servIPListener;
char *servIP;                /* Server IP address (dotted quad) */
char *echoString;            /* String to send to echo server */
char echoBuffer[RCVBUFSIZE]; /* Buffer for echo string */

char lstnerBuffer[RCVBUFLSTNSIZE];
unsigned int echoStringLen, fromSize; /* Length of string to echo */
int bytesRcvd, totalBytesRcvd;        /* Bytes read in single recv()
                                         and total bytes read */
char *sending;

void makeLog(char *sending) {
  if ((lstnsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    printf("socket() failed");
    exit(0);
  }
  int curr = 0;
  char my_pid[6];
  sprintf(my_pid, "%d", getpid());
  for (; curr < strlen(my_pid); curr++) {
    lstnerBuffer[curr] = my_pid[curr];
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
    printf("send() failed");
  }
}

void Customer() {
  int i = 0;
  srand(time(NULL) * getpid());
  pid_t my_number = getpid();
  int k = rand() % 5 + 1;
  for (int i = 0; i < k; i++) {
    if (rand() % 2 == 0) {
      printf("\n%d: Товар находится в первом отделе.", my_number);
      sending = "Товар в первом отделе.";
      echoServPort = echoServPort1;
    } else {
      sending = "Товар во втором отделе.";
      printf("\n%d: Товар находится во втором отделе.", my_number);
      echoServPort = echoServPort2;
    }
    makeLog(sending);
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
      printf("socket() failed");
      exit(0);
    }

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
    echoServAddr.sin_port = htons(echoServPort);      /* Server port */
    makeLog("Иду в нужный отдел");
    printf("\n%d: Иду в нужный отдел", my_number);
    usleep(2000000);
    makeLog("Пришёл, жду своей очереди");
    printf("\n%d: Пришёл, жду своей очереди", my_number);
    echoBuffer[0] = '0';
    if (sendto(sock, echoBuffer, RCVBUFSIZE, 0,
               (struct sockaddr *)&echoServAddr,
               sizeof(echoServAddr)) != RCVBUFSIZE) {
      printf("sending() failed");
      exit(1);
    }
    while (recvfrom(sock, echoBuffer, RCVBUFSIZE, 0,
                    (struct sockaddr *)&fromAddr, &fromSize) == 0 ||
           echoBuffer[0] != '1') {
      echoBuffer[0] = '0';
      if (sendto(sock, echoBuffer, RCVBUFSIZE, 0,
                 (struct sockaddr *)&echoServAddr,
                 sizeof(echoServAddr)) != RCVBUFSIZE) {
        printf("sending() failed");
        exit(1);
      }
      usleep(1000);
    }
    makeLog("Продавец пробивает товар.");
    printf("\n%d: Продавец проводит пробивание товара.", my_number);
    if (recvfrom(sock, echoBuffer, RCVBUFSIZE, 0, (struct sockaddr *)&fromAddr,
                 &fromSize) < 0) {
      printf("error");
      exit(0);
    }
    while (echoBuffer[0] != '2') {
      if (recvfrom(sock, echoBuffer, RCVBUFSIZE, 0,
                   (struct sockaddr *)&fromAddr, &fromSize) < 0) {
        printf("recv() failed 2");
        exit(0);
      }
      usleep(20000);
    }
    makeLog("Оплатил и получил товар.");
    printf("\n%d: Оплатил и получил товар", my_number);
    putchar('\n');
  }
  makeLog("Поход завершён.");
  printf("\n%d: Поход в магазин завершён", my_number);
  putchar('\n');
}

int main(int argc, char *argv[]) {
  if (argc != 7) /* Test for correct number of arguments */
  {
    fprintf(stderr, "Usage: %s <Server IP>  [<Echo Port_1>] [<Echo Port_1>]\n",
            argv[0]);
    exit(1);
  }

  servIP = argv[1]; /* First arg: server IP address (dotted quad) */
  echoServPort1 = atoi(argv[2]);
  echoServPort2 = atoi(argv[3]);
  Nproc = atoi(argv[4]);
  listinerServPort = atoi(argv[6]);

  /* Construct the server address structure */
  memset(&echoListnerServAddr, 0,
         sizeof(echoListnerServAddr));      /* Zero out structure */
  echoListnerServAddr.sin_family = AF_INET; /* Internet address family */
  echoListnerServAddr.sin_addr.s_addr =
      inet_addr(argv[5]); /* Server IP address */
  echoListnerServAddr.sin_port = htons(listinerServPort);

  for (int i = 0; i < Nproc; i++) {
    if (fork() == 0) {
      Customer();
      exit(0);
    }
  }
  exit(0);
}
