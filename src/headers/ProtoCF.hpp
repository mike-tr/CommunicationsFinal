#pragma once
#include <cstring>

struct ProtoCF
{
    /* data */
    int msg_id = 0;
    int source_id = 0;
    int destination_id = 0;
    int trailing_msg = 0;
    int function_id = 0;
    char *payload = 0;
    std::string to_string(ProtoCF protocol){
        std::string proto;
        proto+=("Message ID: " + std::to_string(protocol.msg_id) + ", ");
        proto+=("Source ID: " + std::to_string(protocol.source_id) + "\n");
        proto+=("Destination ID: " + std::to_string(protocol.destination_id) + ", ");
        proto+=("Trailing Message: " + std::to_string(protocol.trailing_msg) + "\n");
        proto+=("Function ID: " + std::to_string(protocol.function_id) + ", ");
        std::string str(protocol.payload);
        proto+=("Payload: " + str + "\n");
        return proto;
    }
};