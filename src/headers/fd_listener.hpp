#pragma once
#include <set>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint;

class fd_listener
{
    fd_set rfds, rfds_copy;
    std::set<uint> descriptors;
    uint max_discriptor;

public:
    fd_listener(bool enable_user_input);
    void add_descriptor(const uint fd);
    void remove_descriptor(const uint fd);
    int wait_for_input();
};
