#pragma once
#include "../headers/NodeMessage.hpp"
#include <string>

class INodeNet {
public:
    virtual void send_ack(int sock, const NodeMessage &incoming_message) = 0;
    virtual void send_nack(int sock, const NodeMessage &incoming_message) = 0;
    virtual void handle_discover(NodeMessage &incoming_message) = 0;
    virtual void handle_relay(int sock, const NodeMessage &incoming_message) = 0;
    virtual void send_netm(int sock, const NodeMessage &message) = 0;
    virtual void connect_tcp(std::string ipport) = 0;
};