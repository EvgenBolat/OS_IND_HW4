#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <unistd.h>     /* for close() */

#define MAXPENDING 10 /* Maximum outstanding connection requests */

void HandleUDPClient(int servSock, struct sockaddr_in echoClntAddr, pid_t pid); /* TCP client handling function */

#define RCVBUFLSTNSIZE 80
#define RCVBUFSIZE 1 /* Size of receive buffer */
char lstnerBuffer[RCVBUFLSTNSIZE];

struct sockaddr_in echoListnerServAddrs[10];
char lstnerBuffer[RCVBUFLSTNSIZE];
char new_server_string[100];

char listenerServersIP[10][100];
int listenerServersPorts[10];
char answer;
char sep = ' ';

int lstnsock;
int serversCount = 0;

void makeServerLog(char *sending) {
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
  for (int serverNum = 0; serverNum < serversCount; serverNum++) {
    if ((lstnsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
      printf("socket() failed");
      exit(0);
    }
    if (sendto(lstnsock, lstnerBuffer, RCVBUFLSTNSIZE, 0,
               (struct sockaddr *)&echoListnerServAddrs[serverNum],
               sizeof(echoListnerServAddrs[serverNum])) != RCVBUFLSTNSIZE) {
      printf("something goes wrong");
    }
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

  if ((argc != 5 && (argc - 3) % 2 != 0) ||
      (argc - 3) / 2 > 10) /* Test for correct number of arguments */
  {
    fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
    exit(1);
  }
  int pointer = 0;
  for (int arg_num = 3; arg_num < argc; arg_num += 2) {
    strcpy(listenerServersIP[pointer], argv[arg_num]);
    listenerServersPorts[pointer++] = atoi(argv[arg_num + 1]);
    ++serversCount;
  }
  for (int i = 0; i < serversCount; i++) {
    /* Construct the server address structure */
    memset(&echoListnerServAddrs[i], 0,
           sizeof(echoListnerServAddrs[i]));      /* Zero out structure */
    echoListnerServAddrs[i].sin_family = AF_INET; /* Internet address family */
    echoListnerServAddrs[i].sin_addr.s_addr =
        inet_addr(listenerServersIP[i]); /* Server IP address */
    echoListnerServAddrs[i].sin_port = htons(listenerServersPorts[i]);
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
      HandleUDPClient(servSock, echoClntAddr, pid);
      makeServerLog("Покупатель ушёл");
      printf("Продавец %d: Покупатель ушел\n", pid);
      putchar('\n');
      echoBuffer[0] = '1';
      printf("Продавец %d: Вы хотите изменить состав "
             "серверов-слушателей?(Введите y или n)\n",
             pid);
      if (getchar() == 'y') {
        printf(
            "Продавец %d: Введите количество серверов-слушателей, которые вы "
            "хотите "
            "использовать(от 1 до 10): ",
            pid);
        scanf(" %d", &serversCount);
        for (int i = 0; i < serversCount; i++) {
          printf("Продавец %d: Введите айпи и порт сервера-слушателя: ", pid);
          scanf(" %[^\n]s", new_server_string);
          char *istr = strtok(new_server_string, " ");
          memset(&echoListnerServAddrs[i], 0, sizeof(echoListnerServAddrs[i]));
          echoListnerServAddrs[i].sin_family = AF_INET;
          echoListnerServAddrs[i].sin_addr.s_addr = inet_addr(istr);
          istr = strtok(NULL, " ");
          echoListnerServAddrs[i].sin_port = htons(atoi(istr));
        }
      }
    }
  }
  /* NOT REACHED */
}
