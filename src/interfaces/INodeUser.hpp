#pragma once
#include <string>

class INodeUser
{
public:
    // Given user input process it, and call the relevant function.
    virtual void process_user_input(std::string &input) = 0;
    // set the id of the node.
    virtual void setid(int id) = 0;
    // connect to node given ip and port, return file descriptor
    virtual int connect_to(std::string ip, int port) = 0;
    // send message of length len, to node(id).
    virtual void send_message(int id, std::string message) = 0;
    // print the rout to a given node ( exp 1->5->3->2 ).
    virtual void route(int id) = 0;
    // print the list of all connected nodes.
    virtual void peers() = 0;
};