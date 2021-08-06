#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

#define SERVER_PORT 5019
#define SERVER_PORT 5000
#define SERVER_IP_ADDRESS "127.0.0.1"
#define BUFF_SIZE 1024
#define END_OF_FILE "@@EOF@@"
#define START_FILE "@@START@@"

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

int init_server(sockaddr_in *serverAddr);
int send_long(const int server, long to_send);
int send_file(const int server, FILE *fp);

int main(int argc, char **argv) {
    int server = -1;

    sockaddr_in serverAddr;

    server = init_server(&serverAddr);

    //char *message = "hello from client!";

    for (int i = 0; i < 10; i++) {
        sleep(1);
        std::string message = "testing : " + std::to_string(i) + "\n";
        send(server, &message[0], message.length(), 0);
    }

    //bytes_recived = recv(server, buff, BUFF_SIZE, 0);

    printf("Closing client\n");
    close(server);
    return 0;
}

int init_server(sockaddr_in *serverAddr) {
    int server = -1;

    printf("Starting client...\n");
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1) {
        printf("error connecting to socket");
        exit(1);
    }

    bzero(serverAddr, sizeof(*serverAddr));

    // fill server addr
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons(SERVER_PORT);

    // convert ip to network format.
    int p = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &serverAddr->sin_addr);
    if (p <= 0) {
        printf("inet_pton has failed...\n");
        close(server);
        exit(1);
    }

    //connect to server
    printf("Connecting to server...\n");
    if (connect(server, (struct sockaddr *)serverAddr, sizeof(*serverAddr)) == -1) {
        printf("failed to connect to server\n");
        close(server);
        exit(1);
    }

    printf("Connected to server\n");
    char *data = "hello";
    send(server, data, sizeof(data), 0);
    return server;
}
