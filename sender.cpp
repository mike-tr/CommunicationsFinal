#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#define SERVER_PORT 5010
#define SERVER_IP_ADDRESS "127.0.0.1"
#define BUFF_SIZE 1024
#define END_OF_FILE "@@EOF@@"
#define START_FILE "@@START@@"

#define FILE_NAME "data.txt"

typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr sockaddr;

void setCC(int sock, const char *cc_type)
{
    socklen_t len = strlen(cc_type);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, cc_type, len) != 0)
    {
        perror("setsockopt");
        exit(1);
    }
}

int init_server(sockaddr_in *serverAddr);
int send_long(const int server, long to_send);
int send_file(const int server, FILE *fp);

int main(int argc, char **argv)
{
    int server = -1;
    FILE *fp;

    fp = fopen(FILE_NAME, "rb");

    sockaddr_in serverAddr;

    server = init_server(&serverAddr);

    setCC(server, "cubic");
    printf("sending as cubic\n");

    for (int i = 0; i < 5; i++)
    {
        printf("sending file for the %d time\n", i + 1);
        send_file(server, fp);
    }

    setCC(server, "reno");
    printf("sending as reno\n");

    for (int i = 0; i < 5; i++)
    {
        printf("sending file for the %d time\n", i + 1);
        send_file(server, fp);
    }

    printf("Closing client\n");
    close(server);
    return 0;
}

int init_server(sockaddr_in *serverAddr)
{
    int server = -1;

    printf("Starting client...\n");
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == -1)
    {
        printf("error connecting to socket");
        exit(1);
    }

    bzero(serverAddr, sizeof(*serverAddr));

    // fill server addr
    serverAddr->sin_family = AF_INET;
    serverAddr->sin_port = htons(SERVER_PORT);

    // convert ip to network format.
    int p = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &serverAddr->sin_addr);
    if (p <= 0)
    {
        printf("inet_pton has failed...\n");
        close(server);
        exit(1);
    }

    //connect to server
    printf("Connecting to server...\n");
    if (connect(server, (struct sockaddr *)serverAddr, sizeof(*serverAddr)) == -1)
    {
        printf("failed to connect to server\n");
        close(server);
        exit(1);
    }

    printf("Connected to server\n");

    return server;
}

int send_long(const int server, long to_send)
{
    char nsize[sizeof(long)] = {0};
    *((long *)nsize) = to_send;
    int bsent = send(server, &nsize, sizeof(nsize), 0);
    if (bsent == -1)
    {
        printf("error sending file length\n");
        exit(1);
    }
    return bsent;
}

int send_file(const int server, FILE *fp)
{
    int bytes_sent = 0;
    fseek(fp, 0L, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    printf("sending file of size %ld\n", file_size);
    send_long(server, file_size);
    printf("file size sent\n");

    char buff[BUFF_SIZE] = {'\0'};

    while (!feof(fp))
    {
        //printf("???\n");
        bzero(buff, BUFF_SIZE);
        //fgets(buff, BUFF_SIZE, (FILE *)fp);
        int length = fread(buff, 1, BUFF_SIZE - 1, fp);
        //printf("msg length is %d\n", length);
        if (length == 0)
        {
            printf("end of file...\n");
            break;
        }

        send(server, buff, length, 0);
    }

    printf("end of sending file... waiting for confirmation...\n");
    bzero(buff, BUFF_SIZE);
    int bytes_recived = recv(server, buff, BUFF_SIZE, 0);

    while (strcmp(buff, END_OF_FILE) != 0)
    {
        if (bytes_recived <= 0)
        {
            printf("failed to recieve confirmation\n");
            exit(1);
        }
        bytes_recived = recv(server, buff, BUFF_SIZE, 0);
    }

    printf("file succsesfully sent...\n");

    printf("\n");
    return 0;
}
