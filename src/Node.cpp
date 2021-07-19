#include "headers/Node.hpp"
#include <unistd.h>
#include <iostream>
#include <unistd.h>

using namespace std;

const int buff_size = 512;

Node::Node(std::string ip, int port, int max_connections, bool listen_to_input)
    : INodeNet(), INodeUser(), listner(listen_to_input)
{
    this->max_connections = max_connections;
    if ((this->server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    this->serv_addr.sin_family = AF_INET;
    this->serv_addr.sin_port = htons(port);
    int p = inet_pton(AF_INET, &ip[0], &serv_addr.sin_addr);
    if (p <= 0)
    {
        printf("inet_pton has failed...\n");
        failed = true;
        exit(1);
    }

    if (bind(this->server, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror(" Failed to bind server to speciefied ip and port ");
        exit(1);
    }
}

Node::~Node()
{
    close(this->server);
}

void Node::start_server()
{
    socketpair(AF_UNIX, SOCK_STREAM, 0, this->interrupt_sock);

    cout << "my file is : " << interrupt_sock[0] << endl;
    //add_fd_to_monitoring(this->interrupt_sock[0]);
    listner.add_descriptor(this->interrupt_sock[0]);
    cout << "Starting Node..." << endl;
    //add_fd_to_monitoring(this->server);
    this->running = true;
    this->w84connections = thread{&Node::connections_thread, this};
    this->user_input();
}

void Node::connections_thread()
{
    cout << "waiting for new connections..." << endl;
    if (listen(this->server, this->max_connections) != 0)
    {
        perror(" There's an ERROR in listening ");
        exit(1);
    }

    char buff[buff_size];
    sockaddr_in incomming_connection;
    socklen_t addr_size = sizeof(incomming_connection);

    while (this->running)
    {
        memset(&incomming_connection, 0, sizeof(incomming_connection));
        int client = accept(this->server, (struct sockaddr *)&incomming_connection, &addr_size);
        if (client == -1)
        {
            printf("failed to accept client\n");
            close(client);
            exit(1);
        }

        char ip[16] = {0};
        inet_ntop(AF_INET, &incomming_connection.sin_addr.s_addr, ip, 15);
        uint16_t port = ntohs(incomming_connection.sin_port);
        printf("connected succsesfully to %s:%d \n", ip, port);

        // We send a message to the socket inorder to update and get new data.
        string message = "intrupt by new connection\n";
        listner.add_descriptor(client);
        send(this->interrupt_sock[1], &message[0], message.length(), 0);
        //send(this->interrupt_sock[0], &message[0], message.length(), 0);
    }
}

void Node::interrupt()
{
}

void Node::listen_to(int sock)
{
    cout << "listening to sock " << sock << endl;
    // char buff[buff_size];
    int message;
    while (this->running)
    {
        // int ret = wait_for_input();
        // printf("fd: %d is ready. reading...\n", ret);
        // memset(buff, 0, buff_size);
        // message = read(sock, 0, buff_size);
        // printf("Client[%d]: %s\n", sock, buff);
    }

    cout << "sock : " << sock << " is down..." << endl;
}

void Node::user_input()
{
    cout << "listening to all..." << endl;
    // char buff[buff_size];
    char buff[buff_size];
    int size;
    while (this->running)
    {

        printf("waiting for input...\n");
        int ret = listner.wait_for_input();
        printf("fd: %d is ready. reading...\n", ret);
        // this simply reads the bytes send from the right socket, and droppes the message into bugg
        // Notice : that you want to remove any garbage from buff.
        // so i ADD here buff = 0
        memset(buff, 0, buff_size);
        //fgets(buff, buff_size, stdin);
        size = read(ret, buff, buff_size);
        if (size == 0)
        {
            cout << "disconnected..." << endl;
            this->listner.remove_descriptor(ret);
        }
        else
        {
            cout << "message size is : " << size << endl;
            printf("%s", buff);
        }

        // int k;
        // cin >> k;
    }

    // cout << "sock : " << sock << " is down..." << endl;
    // char input[buff_size];
    // while (this->running)
    // {
    //     fgets(input, buff_size, stdin);
    //     cout << input;
    // }
}