#include "iostream"
// #include "hpp/select.hpp"
#include <string>
#include <unistd.h>
#include <iostream>

#include "src/headers/ProtoCF.hpp"
#include "src/headers/Node.hpp"
// #include "src/NodeInherited.cpp"
//#include "src/headers/ClientNode.hpp"
#include <set>
using namespace std;

int main()
{
    string ip;
    int port;
    printf("Enter Ip Address: ");
    cin >> ip;
    printf("Enter Port: ");
    cin >> port;
    Node node{ip, port, 10, true};
    int fd = node.connect_to(ip, 5000);
    ProtoCF protocol;
    printf("sending messages...\n");
    for (int i = 1; i <= 10; i++)
    {
        printf("send message %d...\n", i);
        string message = "test : " + std::to_string(i) + "\n";
        protocol.payload = &message[0];
        string proto = protocol.to_string(protocol);
        sleep(1);
        node.send_message(fd, proto);
        if(i==9){
            printf("\n Closing client");
            close(fd);
        }
    }
    return 0;
}