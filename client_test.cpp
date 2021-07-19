#include "iostream"
// #include "hpp/select.hpp"
#include <string>
#include <unistd.h>

#include "src/headers/ProtoCF.hpp"
#include "src/headers/Node.hpp"
#include "src/headers/ClientNode.hpp"
#include <set>
using namespace std;

int main()
{
    fd_listener listener{true};

    ClientNode node{listener};
    int fd = node.connect_to("127.0.0.1", 5000);

    for (int i = 0; i < 10; i++)
    {
        string message = "test : " + std::to_string(i) + "\n";
        sleep(1);
        node.send_msg(fd, message);
    }
    return 0;
}