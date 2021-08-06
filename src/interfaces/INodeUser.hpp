#pragma once
#include "../headers/Utilities.hpp"
#include <string>

class INodeUser {
public:
    // set the id of the node.
    virtual void setid(std::string id) = 0;
    // connect to node given ip and port, return file descriptor
    virtual int connect_to(Utilities::Address &address) = 0;
    // send message of length len, to node(id).
    virtual void send_message(std::vector<std::string> userinput) = 0;
    // print the rout to a given node ( exp 1->5->3->2 ).
    virtual void route(std::string id) = 0;
    // print the list of all connected nodes.
    virtual void peers() = 0;

    bool formatCheck(std::vector<std::string> vec, uint size) {
        if (vec.size() != size) {
            std::cout << "Invalid argument's : ";
            std::cout << "Nack" << std::endl;
            return false;
        }
        return true;
    }
};