#include "iostream"
// #include "hpp/select.hpp"
#include <string>

#include "src/headers/ProtoCF.hpp"

using namespace std;

int main()
{
    auto pr = ProtoCF{};
    string baka = "kekekeke lololo";
    char *c = &baka[0];

    cout << pr.msg_id << endl;
    pr.msg_id = 1;
    cout << pr.msg_id << endl;
    cout << c << endl;
    cout << "tessdsdst" << endl;
    return 0;
}