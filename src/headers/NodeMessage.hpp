#pragma once
#include <cstring>

struct NodeMessage
{
    /* data */
    int msg_id = 0;
    int source_id = 0;
    int destination_id = 0;
    int trailing_msg = 0;
    int function_id = 0;
    char payload[492] = {0};

    void setPayload(std::string str)
    {
        std::strcpy(payload, str.c_str());
    }

    std::string to_string()
    {
        std::string proto;
        proto += ("Message ID: " + std::to_string(this->msg_id) + ", ");
        proto += ("Source ID: " + std::to_string(this->source_id) + "\n");
        proto += ("Destination ID: " + std::to_string(this->destination_id) + ", ");
        proto += ("Trailing Message: " + std::to_string(this->trailing_msg) + "\n");
        proto += ("Function ID: " + std::to_string(this->function_id) + ", ");
        std::string str(this->payload);
        proto += ("Payload: " + str);
        return proto;
    }
};