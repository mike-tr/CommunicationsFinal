#include "iostream"
// #include "hpp/select.hpp"
#include <string>
#include <unistd.h>

#include "src/headers/ProtoCF.hpp"
#include "src/headers/Node.hpp"
//#include "src/headers/ClientNode.hpp"
#include <set>
using namespace std;

int main()
{
    Node node{"127.0.0.1", 5001, 10, true};
    int fd = node.connect_to("127.0.0.1", 5000);

    printf("sending messages...\n");
    for (int i = 1; i <= 10; i++)
    {
        printf("send message %d...\n", i);
        string message = "test : " + std::to_string(i) + "\n";
        sleep(1);
        node.send_message(fd, message);
    }
    return 0;
}