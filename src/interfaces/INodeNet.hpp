#pragma once
#include "../headers/NodeMessage.hpp"
#include <string>

class INodeNet
{
public:
    virtual void ack(NodeMessage &incoming_message) = 0;
    virtual void nack(NodeMessage &incoming_message) = 0;
    virtual void discover(NodeMessage &incoming_message) = 0;
    virtual void send_netm(int sock, NodeMessage &message) = 0;
    virtual void connect_tcp(std::string ipport) = 0;
};