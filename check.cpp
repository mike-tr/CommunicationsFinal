#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <thread>
#include <iostream>
#include <stdio.h>

#include "src/headers/select.hpp"
#include "src/select.cpp"

void clients(int sockfd, sockaddr_in *new_addr, socklen_t addr_size, char* buff){
  while(true)
      {
        int client = accept(sockfd, (struct sockaddr*)new_addr, &addr_size);
        add_fd_to_monitoring(client);
        if(listen(client, 10) == 0)
        {
          printf(" Listening right now... \n");
        }
        else
        {
          perror(" There's an ERROR in listening ");
          exit(1);
        }
        char ip[16] = {0};
        inet_ntop(AF_INET, &new_addr->sin_addr.s_addr, ip, 15);
        uint16_t port = ntohs(new_addr->sin_port);
        printf("connected succsesfully to %s:%d \n", ip, port);
        memset(buff, 1025, sizeof(char)); 
        int message = read(client, buff, 1025);
        printf("Client[%d]: %s\n",client, buff);
      }
}
int main(int argc, char *argv[])
{
    int sockfd = 0;
    struct sockaddr_in serv_addr, new_addr;
    //int ret;
    // int opt = 1;
    int port = 5010;
    char buff[1025];
    //time_t ticks;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
       perror("socket failed");
        exit(EXIT_FAILURE);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = port;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
      perror(" Bind failed ");
      exit(1);
    }
    printf("adding fd1(%d) to monitoring\n", sockfd);
    add_fd_to_monitoring(sockfd);

    if(listen(sockfd, 10) == 0)
    {
      printf(" Listening right now... \n");
    }
    else
    {
      perror(" There's an ERROR in listening ");
      exit(1);
    }

    bool listen = true;
    while(listen)
    {
      printf("  Waiting for input...\n");
      int client_sock = wait_for_input();
      printf("fd: %d is ready. reading...\n", client_sock);
      memset(buff, 1025, sizeof(char)); 
      ssize_t message = read(client_sock,buff,sizeof(buff));
      printf("Input is: %s", buff);
    }
    socklen_t addr_size = sizeof(new_addr);
    std::thread t1(clients, sockfd, new_addr, addr_size, buff);  
    // Forcefully attaching socket to the port 8080

    // printf("adding fd1(%d) to monitoring\n", sockfd);
    // add_fd_to_monitoring(sockfd);
    //listen(server_fd, 10);


    // for (i = 0; i < 10; ++i)
    // {
    //     printf("waiting for input...\n");
    //     ret = wait_for_input();
    //     printf("fd: %d is ready. reading...\n", ret);
    //     read(ret, buff, 1025);
    //     printf("%s", buff);
    // }
}
