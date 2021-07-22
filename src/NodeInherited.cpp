// HERE WE WOULD IMPLEMENT THE VIRTUAL METHODS
#include <string>
#include <unistd.h>


#include "headers/Node.hpp"
using namespace std;
    
    int Node::connect_to(std::string ip, int port)
    {
        server = -1;
        printf("Starting client...\n");
        server = socket(AF_INET, SOCK_STREAM, 0);
        if (server < 0)
        {
            printf("Error connecting to socket \n");
            return -1;
        }
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        int p = inet_pton(AF_INET, &ip[0], &serv_addr.sin_addr);
        if (p <= 0)
        {
            printf("inet_pton has failed...\n");
            close(server);
            exit(1);
        }
        printf("Connecting to server...\n");
        if (connect(server, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        {
            printf("failed to connect to server\n");
            close(server);
            exit(1);
        }
        printf("Connected to server\n");
        listner.add_descriptor(server);
        return server;
    }

    void Node::send_message(int id, std::string message)
    {
        send(id, &message[0], message.length(), 0);
    }