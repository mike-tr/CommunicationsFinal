#pragma once
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Utilities {
struct Address {
    std::string ip;
    int port;

    Address() : ip(""), port(-1) {}

    Address(std::string ip, int port) : ip(ip), port(port) {}

    Address(const Address &add) : ip(add.ip), port(add.port) {}

    bool operator==(const Address &other) {
        return this->port == other.port && this->ip == other.ip;
    }

    bool operator<(const Address &l) const {
        return this->port < l.port;
    }

    static Address GetIPandPort(int argc, char *argv[]) {
        char buff[512] = {0};

        std::string ip;
        int port;
        if (argc == 3) {
            port = std::atoi(argv[2]);
            ip = argv[1];
            return {ip, port};
        }

        std::cout << "Enter Ip Address:" << std::endl;
        fgets(buff, 512, stdin);
        ip = std::string{buff};
        ip.erase(std::remove(ip.begin(), ip.end(), '\n'),
                 ip.end());
        std::cout << "Enter Port: " << std::endl;
        fgets(buff, 512, stdin);
        std::string response{buff};
        port = atoi(response.c_str());
        //std::cin >> port;
        return {ip, port};
    }

    static Address GetIPandPort(std::string ip, std::string port) {
        return {ip, std::atoi(&port[0])};
    }

    friend std::ostream &operator<<(std::ostream &out, const Address &add) {
        out << add.ip << ":" << add.port;
        return out;
    }
};

char *string_to_char_arr(std::string &ref);
std::vector<std::string> splitBy(std::string str, char c);
} // namespace Utilities
