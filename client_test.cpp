#include "iostream"
// #include "hpp/select.hpp"
#include <string>
#include <unistd.h>
#include <iostream>

#include "src/headers/NodeMessage.hpp"
#include "src/headers/Node.hpp"
#include "src/headers/Utilities.hpp"
// #include "src/NodeInherited.cpp"
//#include "src/headers/ClientNode.hpp"
#include <set>
using namespace std;
using namespace Utilities;

int main(int argc, char *argv[])
{
    // cout << argc << endl;
    // for (int i = 0; i < argc; i++)
    // {
    //     cout << argv[i] << endl;
    // }
    // string ip;
    // int port;
    // printf("Enter Ip Address: ");
    // cin >> ip;
    // printf("Enter Port: ");
    // cin >> port;

    auto ipport = Address::GetIPandPort(argc, argv);
    Node node{ipport, 10, false};
    auto address = Address{"127.0.0.1", 5000};
    int fd = node.connect_to(address);
    NodeMessage protocol;
    printf("sending messages...\n");
    for (int i = 1; i <= 10; i++)
    {
        printf("send message %d...\n", i);
        string message = "test : " + std::to_string(i) + "\n";
        //protocol.payload = &message[0];
        //string proto = protocol.to_string(protocol);
        sleep(1);
        node.send_message(fd, message);
        if (i == 9)
        {
            printf("\n Closing client");
            close(fd);
        }
    }
    return 0;
}