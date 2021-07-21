#include "iostream"
// #include "hpp/select.hpp"
#include <string>

#include "src/headers/ProtoCF.hpp"
#include "src/headers/Node.hpp"
#include <set>
using namespace std;

int main()
{
    Node node{"127.0.0.1", 5000, 10, true};
    node.start_server();

    // set<int> s{1, 2, 3, 4};
    // cout << *s.begin() << endl;

    // auto pr = ProtoCF{};
    // char *c1 = "12345\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    // string baka{c1};
    // char *c = &baka[0];

    // cout << baka.size() << endl;

    // cout << pr.msg_id << endl;
    // pr.msg_id = 1;
    // cout << pr.msg_id << endl;
    // cout << c << endl;
    // cout << "test" << endl;

    // char input[512];
    // fgets(input, 512, stdin);
    // cout << input << endl;
    return 0;
}