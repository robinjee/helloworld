#include <iostream>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

using namespace std;

int server_start(char*, int );
int client_start(char* ,int );
int udp_server(char* address, int port);
int udp_client(char* address, int port);


int main(int argc, char *argv[]) {

  if (argc < 5) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(0);
  }

  int port = atoi(argv[4]);

  if(strcmp(argv[1],"server") == 0 )
  {
    if(strcmp(argv[2],"tcp") == 0 ){
      server_start(argv[3], port);
    }
    else if(strcmp(argv[2],"udp") == 0){
      udp_server(argv[3], port);

    }
  }
  else if(strcmp(argv[1],"client") == 0 )
  {
    if(strcmp(argv[2],"tcp") == 0 ){
      client_start(argv[3], port);
    }
    else if(strcmp(argv[2],"udp") == 0){
      udp_client(argv[3], port);
    }

  }

  return 0;
}

void error(const char *msg)
{
  perror(msg);
  exit(0);
}


void func(int sockfd)
{
  char buff[80];
  int n;
  // infinite loop for chat
  for (;;) {
    memset(buff,0, 80);

    // read the message from client and copy it in buffer
    read(sockfd, buff, sizeof(buff));
    // print buffer which contains the client contents
    printf("From client: %s\t To client : ", buff);
    memset(buff,0, 80);
    n = 0;
    // copy server message in the buffer
    while ((buff[n++] = getchar()) != '\n')
      ;

    // and send that buffer to client
    write(sockfd, buff, sizeof(buff));

    // if msg contains "Exit" then server exit and chat ended.
    if (strncmp("exit", buff, 4) == 0) {
      printf("Server Exit...\n");
      break;
    }
  }
}

// Driver code
int server_start(char* address, int port) {

  int sockfd, connfd;
  socklen_t len;
  struct sockaddr_in servaddr, cli;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  }
  else
    printf("Socket successfully created..\n");
  memset(&servaddr, 0, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(address);//htonl(INADDR_ANY);
  servaddr.sin_port = port;

  // Binding newly created socket to given IP and verification
  if (-1 == ::bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) {
    printf("socket bind failed...\n");
    exit(0);
  }
  else
    printf("Socket successfully binded..\n");

  // Now server is ready to listen and verification
  if ((listen(sockfd, 5)) != 0) {
    printf("Listen failed...\n");
    exit(0);
  }
  else
    printf("Server listening..\n");
  len = sizeof(cli);

  // Accept the data packet from client and verification
  connfd = accept(sockfd, (struct sockaddr*)&cli, &len);
  if (connfd < 0) {
    printf("server acccept failed...\n");
    exit(0);
  }
  else
    printf("server acccept the client...\n");

  // Function for chatting between client and server
  func(connfd);

  // After chatting close the socket
  close(sockfd);
  return 0;
}

void func2(int sockfd)
{
  char buff[80];
  int n;
  for (;;) {
    memset(buff, 0, sizeof(buff));
    printf("Enter the string : ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
      ;
    write(sockfd, buff, sizeof(buff));
    memset(buff, 0, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
    if ((strncmp(buff, "exit", 4)) == 0) {
      printf("Client Exit...\n");
      break;
    }
  }
}

int client_start(char* address, int port)
{
  int sockfd;
  struct sockaddr_in servaddr;

  // socket create and varification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  }
  else
    printf("Socket successfully created..\n");
  memset(&servaddr, 0, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(address);
  servaddr.sin_port = port;

  // connect the client socket to server socket
  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
    printf("connection with the server failed...\n");
    exit(0);
  }
  else
    printf("connected to the server..\n");

  // function for chat
  func2(sockfd);

  // close the socket
  close(sockfd);
  return 0;
}

int udp_server(char* address, int port)
{
  int sockfd;
  char buffer[1024];
  char hello[18] = "Hello from server";
  struct sockaddr_in servaddr, cliaddr;

  // Creating socket file descriptor
  if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Filling server information
  servaddr.sin_family    = AF_INET; // IPv4
  servaddr.sin_addr.s_addr = inet_addr(address);
  servaddr.sin_port = port;

  // Bind the socket with the server address
  if ( -1 == ::bind(sockfd, (const struct sockaddr *)&servaddr,
      sizeof(servaddr)) )
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  int len, n;

  len = sizeof(cliaddr);  //len is value/resuslt

  n = recvfrom(sockfd, (char *)buffer, 1024,
      MSG_WAITALL, ( struct sockaddr *) &cliaddr,
      (socklen_t*)&len);
  buffer[n] = '\0';
  printf("Client : %s\n", buffer);
  sendto(sockfd, (const char *)hello, strlen(hello),
      MSG_NOSIGNAL , (const struct sockaddr *) &cliaddr,
      len);
  printf("Hello message sent.\n");

  return 0;
}

int udp_client(char* address, int port) {
    int sockfd;
    char buffer[1024];
    char hello[18] = "Hello from client";
    struct sockaddr_in     servaddr, clientaddr;

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
     memset(&servaddr, 0, sizeof(clientaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(address);


    // Filling client information
    clientaddr.sin_family    = AF_INET; // IPv4
    clientaddr.sin_addr.s_addr = inet_addr("192.168.56.102");
    clientaddr.sin_port = htons(7700);

    // Bind the socket with the server address
    if ( -1 == ::bind(sockfd, (const struct sockaddr *)&clientaddr,
        sizeof(clientaddr)) )
    {
      perror("bind failed");
      exit(EXIT_FAILURE);
    }

    int n, len;

    sendto(sockfd, (const char *)hello, strlen(hello),
        MSG_NOSIGNAL, (const struct sockaddr *) &servaddr,
            sizeof(servaddr));
    printf("Hello message sent.\n");

    n = recvfrom(sockfd, (char *)buffer, 1024,
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                (socklen_t*)&len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);

    close(sockfd);
    return 0;
}



