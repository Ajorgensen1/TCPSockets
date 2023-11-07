#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Practical.h"
// Alex Jorgensen and Paige Waisela 
// CPSC 3600 Assignment 1

int main(int argc, char *argv[]) {

  if (argc < 3 || argc > 4) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)",
        "<Server Address> [<Server Port>]");

  char *servIP = argv[1];     // First arg: server IP address (dotted quad)
  char *fileRequest = "REQUEST";

  // Third arg (optional): server port (numeric).  7 is well-known echo port
  in_port_t servPort = (argc == 3) ? atoi(argv[2]) : 7;

  // Create a reliable, stream socket using TCP
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0)
    DieWithSystemMessage("socket() failed");

  // Construct the server address structure
  struct sockaddr_in servAddr;            // Server address
  memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
  servAddr.sin_family = AF_INET;          // IPv4 address family
  // Convert address
  int rtnVal = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
  if (rtnVal == 0)
    DieWithUserMessage("inet_pton() failed", "invalid address string");
  else if (rtnVal < 0)
    DieWithSystemMessage("inet_pton() failed");
  servAddr.sin_port = htons(servPort);    // Server port

  // Establish the connection to the echo server
  if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
    DieWithSystemMessage("connect() failed");

  // Client sending request for files to server
  ssize_t fileRequestLen = strlen(fileRequest);
  ssize_t numBytesSent = send(sock, fileRequest, fileRequestLen, 0);
  if (numBytesSent < 0){
  DieWithSystemMessage("send() failed");
  }

  // creating required buffers and printing input prompt
  unsigned int num = 1;
  int choice[BUFSIZE];
  char buffer[BUFSIZE];
  char files[BUFSIZE];
  printf("Requesting a list of files from the server %s\n", servIP);

  // recieving names of files from server
  ssize_t numBytesRcvd = recv(sock, files, BUFSIZE, 0);
  if (numBytesRcvd < 0){
    DieWithSystemMessage("recv() failed");
  }
  printf("List received.\n");
  printf("User, please select a file: \n");
  // printing received files and taking user input of file choice
  printf("%s", files);
  scanf("%d", choice);

  // sending users choice over to server
  numBytesSent = send(sock, choice, sizeof(choice), 0);
  if (numBytesSent < 0){
    DieWithSystemMessage("send() failed");
  }

  // receiving contents of chosen file from server and printing 
  num = 1;
  while(( num = recv(sock, buffer, BUFSIZE, 0)) > 0){
    char chars[BUFSIZE];
    ssize_t numBytesRcvd = recv(sock, buffer, BUFSIZE, 0);
    memcpy(chars, buffer, sizeof(buffer));
    printf("%s", chars);
  }
  if (numBytesRcvd < 0){
    DieWithSystemMessage("recv() failed");
  }
  // exit prompt
  fputc('\n', stdout); 
  fputs("Goodbye!!!\n", stdout);

  close(sock);
  exit(0);
}
