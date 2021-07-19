#pragma once
#include <map>
#include <string>
#include "../interfaces/INodeNet.hpp"
#include "../interfaces/INodeUser.hpp"
#include "fd_listener.hpp"
#include <thread>

// #include <sys/socket.h>
// #include <netinet/in.h>
#include <arpa/inet.h>

class ClientNode
{
private:
    // SERVER SIDE
    fd_listener &ls;

    // CLIENT SIDE ( SHOULD BE PRIVATE )

public:
    ClientNode(fd_listener &listener);
    // this should conenct to a given server, returns the file descriptor.
    // adds, the server to the listener.
    int connect_to(std::string ip, int port);
    // whatever u need
    void send_msg(int server_fd, std::string message);
};