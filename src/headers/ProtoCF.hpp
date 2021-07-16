#include <cstring>

struct ProtoCF
{
    /* data */
    int msg_id = 0;
    int source_id = 0;
    int destination_id = 0;
    int trailing_msg = 0;
    int function_id = 0;
    char *payload = 0;
};