#pragma once
#include "NodeMessage.hpp"
#include <ctime>
#include <deque>
#include <map>
#include <string>

enum class discover_status {
    empty,
    found
};

struct waiting_route_response {
    int source_id;
    int target_id;
    int message_id;

    waiting_route_response(int source_id, int waiting_id, int message_id)
        : source_id(source_id), target_id(waiting_id), message_id(message_id) {
    }

    bool operator==(const waiting_route_response &other) {
        return other.source_id == this->source_id and other.target_id == this->target_id;
    }
};

struct nroute {
    std::time_t rtime;
    std::deque<int> path;
    std::map<int, bool> responded;
    int ignore_id;
    discover_status status;
    uint responses;
    bool searching;

    nroute();
    void update(const NodeMessage &msg);
    bool check_valid(int timeout);
    void dump();
    std::string to_string() const;
    void pack_msg(NodeMessage &msg);
    void remove_ignore_id();
};