#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

#include "headers/fd_listener.hpp"

using namespace std;

fd_listener::fd_listener(bool enable_user_input)
{
    FD_ZERO(&rfds_copy);
    this->descriptors.clear();
    if (enable_user_input)
    {
        this->add_descriptor(0);
    }
}

void fd_listener::add_descriptor(const uint fd)
{
    this->descriptors.insert(fd);
    FD_SET(fd, &rfds_copy);
    if (this->max_discriptor < fd)
    {
        this->max_discriptor = fd;
    }
}

void fd_listener::remove_descriptor(const uint fd)
{
    this->descriptors.erase(fd);
    FD_CLR(fd, &rfds_copy);
    if (this->max_discriptor == fd)
    {
        // find the new maximum
        this->max_discriptor = 0;
        for (uint ds : this->descriptors)
        {
            if (this->max_discriptor < ds)
            {
                this->max_discriptor = ds;
            }
        }
    }
}

int fd_listener::wait_for_input()
{
    memcpy(&rfds, &rfds_copy, sizeof(rfds_copy));
    int retval = select(this->max_discriptor + 1, &rfds, NULL, NULL, NULL);
    if (retval > 0)
    {
        for (uint fd : this->descriptors)
        {
            if (FD_ISSET(fd, &rfds))
                return fd;
        }
    }
    return -1;
}
