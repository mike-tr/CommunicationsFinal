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

    int interrupt_sock[2];

public:
    fd_listener(bool enable_user_input);
    // Add listenning functionallity on given file descriptor
    void add_descriptor(const uint fd);
    // Remove given file descriptor from listening
    void remove_descriptor(const uint fd);
    // Intrupt the wait_for_input function, i.e call this after you have added a new descriptor.
    void interupt();
    // W8 for input on any file descriptor, when there is input in a file returns the file descriptor.
    int wait_for_input();
};
