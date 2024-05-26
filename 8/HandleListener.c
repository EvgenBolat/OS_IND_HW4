#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <unistd.h>     /* for close() */

#include <stdlib.h> /* for atoi() and exit() */

#define LstnBufSize 80 /* Size of receive buffer */

void HandleListener(int sock) {
  int bytesRcvd;
  char lstnBuffer[LstnBufSize]; /* Buffer for echo string */
  if ((bytesRcvd = recvfrom(sock, lstnBuffer, LstnBufSize, 0, NULL, NULL)) <
      0) {
    printf("recvfrom() failed");
    exit(1);
  }
  printf("%s", lstnBuffer);
}
