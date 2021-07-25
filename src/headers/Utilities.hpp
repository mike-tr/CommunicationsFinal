#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

namespace Utilities
{
    struct Address
    {
        std::string ip;
        int port;

        Address(std::string ip, int port) : ip(ip), port(port) {}

        bool operator=(const Address &other)
        {
            return this->port == other.port && this->ip == other.ip;
        }

        bool operator<(const Address &l) const
        {
            return this->port < l.port;
        }

        static Address GetIPandPort(int argc, char *argv[])
        {
            std::string ip;
            int port;
            if (argc == 3)
            {
                port = std::atoi(argv[2]);
                ip = argv[1];
                return {ip, port};
            }
            std::cout << "Enter Ip Address:" << std::endl;
            std::cin >> ip;
            std::cout << "Enter Port: " << std::endl;
            std::cin >> port;
            return {ip, port};
        }

        static Address GetIPandPort(std::string ip, std::string port)
        {
            return {ip, std::atoi(&port[0])};
        }
    };

    char *string_to_char_arr(std::string &ref);
    std::vector<std::string> splitBy(std::string str, char c);
}
