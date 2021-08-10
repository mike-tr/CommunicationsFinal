#include "headers/Utilities.hpp"
#include "headers/NodeMessage.hpp"
#include <algorithm>
using namespace std;

namespace Utilities {
char *string_to_char_arr(std::string &ref) {
    // string to char *
    return &ref[0];
}

std::vector<std::string> splitBy(std::string str, char c) {
    // split string by some character. also remove \n from sring.
    str.erase(std::remove(str.begin(), str.end(), '\n'),
              str.end());
    std::vector<std::string> result;
    std::stringstream ss(str);
    while (getline(ss, str, c)) {
        result.push_back(str);
    }
    return result;
}
} // namespace Utilities

namespace net_fid {
std::string fid_tostring(int fid) {
    // function id to string.
    switch (fid) {
    case ack:
        return "Ack";
    case nack:
        return "Nack";
    case connect:
        return "Connected";
    case discover:
        return "Discover";
    case route:
        return "Route";
    case send:
        return "Send";
    case relay:
        return "Relay";
    default:
        return std::to_string(fid);
    }
}
} // namespace net_fid