// HERE WE WOULD IMPLEMENT THE VIRTUAL METHODS
#include <string>
#include <unistd.h>
#include <iostream>

#include "headers/Node.hpp"
using namespace std;

int Node::connect_to(Utilities::Address &address)
{
    int other_server = -1;
    struct sockaddr_in other_addr;
    printf("Starting client...\n");
    other_server = socket(AF_INET, SOCK_STREAM, 0);
    if (other_server < 0)
    {
        printf("Error connecting to socket \n");
        return -1;
    }
    other_addr.sin_family = AF_INET;
    other_addr.sin_port = htons(address.port);
    int p = inet_pton(AF_INET, &address.ip[0], &other_addr.sin_addr);
    if (p <= 0)
    {
        printf("inet_pton has failed...\n");
        close(other_server);
        //exit(1);
    }
    printf("Connecting to server...\n");
    if (connect(other_server, (struct sockaddr *)&other_addr, sizeof(other_addr)) == -1)
    {
        printf("failed to connect to server\n");
        close(other_server);
        //exit(1);
    }
    printf("Connected to server\n");
    listner.add_descriptor(other_server);
    return other_server;
}

void Node::send_message(int other_id, std::string message)
{
    // NodeMessage nm;
    // nm.source_id = this->id;
    // nm.destination_id = other_id;
    send(other_id, &message[0], message.length(), 0);
}

void Node::send_netm(NodeMessage &message)
{
}

void Node::setid(std::string id)
{
    this->id = std::atoi(&id[0]);
    //cout << "new id : " << this->id << endl;
}