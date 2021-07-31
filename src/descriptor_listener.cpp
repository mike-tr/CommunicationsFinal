#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "headers/fd_listener.hpp"
#include <arpa/inet.h>

using namespace std;

const string interupt_msg = "#I#";
const uint buff_size = 1024;
char buff[buff_size];

fd_listener::fd_listener(bool enable_user_input) : max_discriptor(0)
{
    // char s[] = "tempXXXXXX";
    // int fd = mkstemp(s);
    // cout << fd << endl;
    socketpair(AF_UNIX, SOCK_STREAM, 0, this->interrupt_sock);
    FD_ZERO(&rfds_copy);
    this->descriptors.clear();
    if (enable_user_input)
    {
        this->add_descriptor(0);
    }
    this->add_descriptor(this->interrupt_sock[0]);
    this->add_descriptor(this->interrupt_sock[1]);
}

void fd_listener::add_descriptor(const uint fd)
{
    this->descriptors.insert(fd);
    FD_SET(fd, &rfds_copy);
    if (this->max_discriptor < fd)
    {
        this->max_discriptor = fd;
    }
    this->interupt();
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
        for (int fd : this->descriptors)
        {
            if (FD_ISSET(fd, &rfds))
            {
                if (fd == this->interrupt_sock[0])
                {
                    read(fd, buff, buff_size);
                    return wait_for_input();
                }
                else if (fd == this->interrupt_sock[1])
                {
                    cout << "Exiting..." << endl;
                    return -2;
                }
                return fd;
            }
        }
    }
    return -1;
}

void fd_listener::interupt()
{
    send(this->interrupt_sock[1], &interupt_msg[0], interupt_msg.length(), 0);
}

void fd_listener::stop()
{
    send(this->interrupt_sock[0], &interupt_msg[0], interupt_msg.length(), 0);
}