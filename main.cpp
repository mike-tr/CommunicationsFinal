#include "iostream"
// #include "hpp/select.hpp"
#include <string>

#include "src/headers/Node.hpp"
#include "src/headers/NodeMessage.hpp"
#include "src/headers/Utilities.hpp"
#include <set>
using namespace std;

using namespace Utilities;

int main(int argc, char *argv[]) {
    auto ipport = Address::GetIPandPort(argc, argv);
    //auto address = Address{"127.0.0.1", 5000};
    Node node{ipport, 10, false};
    node.start_server();
    return 0;
}