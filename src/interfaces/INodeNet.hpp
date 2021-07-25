#pragma once
#include "../headers/NodeMessage.hpp"

class INodeNet
{
public:
    virtual void ack(NodeMessage &incoming_message) = 0;
    virtual void nack(NodeMessage &incoming_message) = 0;
    virtual void discover(NodeMessage &incoming_message) = 0;
    virtual void send_netm(NodeMessage &message) = 0;
};