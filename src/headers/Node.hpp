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

class Node : public INodeNet, public INodeUser
{
private:
    // SERVER SIDE
    struct sockaddr_in serv_addr;
    fd_listener listner;
    int server;
    int max_connections;
    std::thread w84connections;
    std::map<int, int> connections;
    void connections_thread();
    void handle_input();

    bool running = false;
    // CLIENT SIDE ( SHOULD BE PRIVATE )

public:
    Node(std::string ip, int port, int max_connections, bool listen_to_input);
    ~Node();

    void start_server();
    // Given user input process it, and call the relevant function.
    virtual void process_user_input(std::string &input) {}
    // set the id of the node.
    virtual void setid(int id) {}
    // connect to node given ip and port.
    virtual int connect_to(std::string ip, int port)
    {
        return 0;
    }
    // send message of length len, to node(id).
    virtual void send_message(int id, std::string message) {}
    // print the rout to a given node ( exp 1->5->3->2 ).
    virtual void route(int id) {}
    // print the list of all connected nodes.
    virtual void peers() {}

    // SERVER
    virtual void ack(ProtoCF &incoming_message) {}
    virtual void nack(ProtoCF &incoming_message) {}
    virtual void discover(ProtoCF &incoming_message) {}
};