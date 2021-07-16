#pragma once
#include "../headers/ProtoCF.hpp"

class INodeNet
{
public:
    virtual void ack(ProtoCF &incoming_message);
    virtual void nack(ProtoCF &incoming_message);
    virtual void discover(ProtoCF &incoming_message);
};