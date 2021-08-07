#pragma once
#include <cstring>
#include <iostream>
#include <sstream>

namespace net_fid {
const int ack = 1;
const int nack = 2;
const int connect = 4;
const int discover = 8;
const int route = 16;
const int send = 32;
const int relay = 64;

std::string fid_tostring(int fid);
} // namespace net_fid

struct message_id_saver {
    int save_data;
    int dest_id;
    message_id_saver *prev = nullptr;

    message_id_saver() : save_data{0}, dest_id{0}, prev{nullptr} {
    }

    ~message_id_saver() {
        if (prev != nullptr) {
            delete prev;
            prev = nullptr;
        }
    }
};

struct NodeMessage {
    /* data */
    int msg_id = 0;
    int source_id = 0;
    int destination_id = 0;
    int trailing_msg = 0;
    int function_id = 0;
    char payload[492] = {0};

    void setPayload(std::string str, uint offset) {
        std::strcpy(&payload[offset], str.c_str());
    }

    std::string to_string() const {
        std::string proto;
        proto += ("Message ID: " + std::to_string(this->msg_id) + ", ");
        proto += ("Source ID: " + std::to_string(this->source_id) + "\n");
        proto += ("Destination ID: " + std::to_string(this->destination_id) + ", ");
        proto += ("Trailing Message: " + std::to_string(this->trailing_msg) + "\n");
        proto += ("Function : " + net_fid::fid_tostring(this->function_id) + "\n");
        if (this->function_id == net_fid::ack or this->function_id == net_fid::nack or this->function_id == net_fid::discover) {
            int mid = *(int *)this->payload;
            proto += ("Payload: " + std::to_string(mid));
        } else if (this->function_id == net_fid::connect) {
            // do nothing
        } else if (this->function_id == net_fid::route) {
            int mid = *(int *)this->payload;
            int path_len = *((int *)this->payload + 1);
            proto += ("Response ID: " + std::to_string(mid) + ", Path len: " + std::to_string(path_len));
        } else {
            int len = *(int *)this->payload;
            std::string str(this->payload + 4);
            proto += "Len : " + std::to_string(len) + ", Payload : " + str;
        }
        return proto;
    }

    friend std::ostream &operator<<(std::ostream &out, const NodeMessage &msg) {
        out << msg.to_string();
        return out;
    }

    friend std::stringstream &operator<<(std::stringstream &out, const NodeMessage &msg) {
        out << msg.to_string();
        return out;
    }
};