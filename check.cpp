#include <arpa/inet.h>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <time.h>
#include <unistd.h>

#include "src/headers/select.hpp"
// #include "src/select.cpp"

#define SERVER_PORT 5000
#define SERVER_IP_ADDRESS "127.0.0.1"

// void clients(int sockfd, sockaddr_in *new_addr, socklen_t addr_size, char *buff)
// {
//     printf("w8ing for clients...\n");
//     while (true)
//     {
//         memset(&addr_size, 0, sizeof(addr_size));
//         int client = accept(sockfd, (struct sockaddr *)new_addr, &addr_size);

//         char ip[16] = {0};
//         inet_ntop(AF_INET, &new_addr->sin_addr.s_addr, ip, 15);
//         uint16_t port = ntohs(new_addr->sin_port);
//         printf("connected succsesfully to %s:%d \n", ip, port);
//         //memset(buff, 1025, sizeof(char));
//         //int message = read(client, buff, 1025);
//         //printf("Client[%d]: %s\n", client, buff);
//         add_fd_to_monitoring(client);
//     }
// }

int main(int argc, char *argv[]) {
    int sockfd = 0;
    struct sockaddr_in serv_addr, new_addr;
    //int ret;
    // int opt = 1;
    char buff[1025];
    //time_t ticks;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    int p = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &serv_addr.sin_addr);
    if (p <= 0) {
        printf("inet_pton has failed...\n");
        close(sockfd);
        exit(1);
    }

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror(" Bind failed ");
        exit(1);
    }
    printf("adding fd1(%d) to monitoring\n", sockfd);
    add_fd_to_monitoring(sockfd);

    if (listen(sockfd, 10) == 0) {
        printf(" Listening right now... \n");
    } else {
        perror(" There's an ERROR in listening ");
        exit(1);
    }
    //printf(" Listening right now... \n");
    socklen_t addr_size = sizeof(new_addr);
    //std::thread t1(clients, sockfd, &new_addr, addr_size, buff);
    printf("after thread start...\n");

    bool listen = true;
    while (listen) {
        printf("waiting for input...\n");
        int ret = wait_for_input();
        printf("fd: %d is ready. reading...\n", ret);

        if (ret == sockfd) {
            memset(&addr_size, 0, sizeof(addr_size));
            int client = accept(sockfd, (struct sockaddr *)&new_addr, &addr_size);
            if (client == -1) {
                printf("failed to accept client\n");
                close(client);
                exit(1);
            }
            char ip[16] = {0};
            inet_ntop(AF_INET, &new_addr.sin_addr.s_addr, ip, 15);
            uint16_t port = ntohs(new_addr.sin_port);
            printf("connected succsesfully to %s:%d \n", ip, port);
            add_fd_to_monitoring(client);
        } else {
            memset(buff, 0, 1025);
            read(ret, buff, 1025);
            printf("%s", buff);
        }
    }
    return 0;
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
