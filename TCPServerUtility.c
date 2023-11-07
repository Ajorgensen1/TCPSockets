#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include "Practical.h"
// Alex Jorgensen and Paige Waisela 
// CPSC 3600 Assignment 1

static const int MAXPENDING = 1024; // Maximum outstanding connection requests
const char * FILES_LIST[] = {"song.txt", "poem.txt", "quote.txt"};

int SetupTCPServerSocket(const char *service) {
  // Construct the server address structure
  struct addrinfo addrCriteria;                   // Criteria for address match
  memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
  addrCriteria.ai_family = AF_UNSPEC;             // Any address family
  addrCriteria.ai_flags = AI_PASSIVE;             // Accept on any address/port
  addrCriteria.ai_socktype = SOCK_STREAM;         // Only stream sockets
  addrCriteria.ai_protocol = IPPROTO_TCP;         // Only TCP protocol

  struct addrinfo *servAddr; // List of server addresses
  int rtnVal = getaddrinfo(NULL, service, &addrCriteria, &servAddr);
  if (rtnVal != 0)
    DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

  int servSock = -1;
  for (struct addrinfo *addr = servAddr; addr != NULL; addr = addr->ai_next) {
    // Create a TCP socket
    servSock = socket(addr->ai_family, addr->ai_socktype,
        addr->ai_protocol);
    if (servSock < 0)
      continue;       // Socket creation failed; try next address

    // Bind to the local address and set socket to listen
    if ((bind(servSock, addr->ai_addr, addr->ai_addrlen) == 0) &&
        (listen(servSock, MAXPENDING) == 0)) {
      // Print local address of socket
      struct sockaddr_storage localAddr;
      socklen_t addrSize = sizeof(localAddr);
      if (getsockname(servSock, (struct sockaddr *) &localAddr, &addrSize) < 0)
        DieWithSystemMessage("getsockname() failed");
      fputs("Binding to ", stdout);
      PrintSocketAddress((struct sockaddr *) &localAddr, stdout);
      fputc('\n', stdout);
      break;       // Bind and listen successful
    }

    close(servSock);  // Close and try again
    servSock = -1;
  }

  // Free address list allocated by getaddrinfo()
  freeaddrinfo(servAddr);

  return servSock;
}

int AcceptTCPConnection(int servSock) {
  struct sockaddr_storage clntAddr; // Client address
  // Set length of client address structure (in-out parameter)
  socklen_t clntAddrLen = sizeof(clntAddr);

  // Wait for a client to connect
  int clntSock = accept(servSock, (struct sockaddr *) &clntAddr, &clntAddrLen);
  if (clntSock < 0)
    DieWithSystemMessage("accept() failed");

  // clntSock is connected to a client!

  fputs("Handling client ", stdout);
  PrintSocketAddress((struct sockaddr *) &clntAddr, stdout);
  fputc('\n', stdout);

  return clntSock;
}

void HandleTCPClient(int clntSocket, char clntName[16]) {
char *request[BUFSIZE];

// receiving file request from client
ssize_t numBytesRcvd = recv(clntSocket, request, BUFSIZE, 0);
if (numBytesRcvd < 0){
  DieWithSystemMessage("recv() failed");
}
fprintf(stderr, "Received file list request from %s\n", clntName);

// creating file input prompt with names of available files
fprintf(stderr, "Sending list of files\n");
char files[BUFSIZE];
memset(files, '\0', sizeof(files));
int counter = 1;

for (int i = 0; i < 3; i++){
  char c = counter + '0';
  strcat(files, &c);
  strcat(files, ". ");
  strcat(files, FILES_LIST[i]);
  strcat(files, "\n");
  ++counter;
}
strcat(files, "\n");

// sending input prompt over to client
ssize_t numBytesSent = send(clntSocket, files, BUFSIZE, 0);
if (numBytesSent < 0){
  DieWithSystemMessage("send() failed");
}

// receiving file choice from client
int choice[BUFSIZE];
numBytesRcvd = recv(clntSocket, choice, BUFSIZE, 0);
if (numBytesRcvd < 0){
  DieWithSystemMessage("recv() failed");
}
fprintf(stderr, "Received request for file %s\n", FILES_LIST[choice[0] - 1]);

// creating file pointer and creating required buffer and lengt of file variables
FILE *fileptr;
char *buffer;
long filelen;

// opening selected file, using fseek to find end of file, setting length of file
fileptr = fopen(FILES_LIST[choice[0] - 1], "rb");  
fseek(fileptr, 0, SEEK_END);          
filelen = ftell(fileptr);             
rewind(fileptr);                      

// allocating memory for file, reading in entire contents of file, closing file
buffer = (char *)malloc(filelen * sizeof(char)); 
fread(buffer, filelen, 1, fileptr); 
fclose(fileptr); 

// sending contents of file over to client
fprintf(stderr, "Sending file to the client\n");
numBytesSent = send(clntSocket, buffer, filelen, 0);
if (numBytesSent < 0){
  DieWithSystemMessage("send() failed");
}
fprintf(stderr, "File sent\n");

memset(files, '\0', sizeof(files)); 

// Close client socket
close(clntSocket); 
}
