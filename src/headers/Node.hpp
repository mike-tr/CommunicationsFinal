#pragma once
#include "../interfaces/INodeNet.hpp"
#include "../interfaces/INodeUser.hpp"
#include "fd_listener.hpp"
#include "nroute.hpp"
#include <map>
#include <sstream>
#include <string>
#include <thread>

// #include <sys/socket.h>
// #include <netinet/in.h>
#include <arpa/inet.h>

class Node : public INodeNet, public INodeUser {
private:
    static const int NONE = -1;
    static const uint buff_size = sizeof(NodeMessage);
    // SERVER SIDE
    Utilities::Address &my_address;
    int node_id = NONE;
    fd_listener listner;
    int server;
    int max_connections;
    std::thread usert;
    std::map<int, Utilities::Address> sockToAddr;
    std::map<int, int> idToSocket;
    std::map<int, int> socketToId;
    std::map<int, message_id_saver> msgIdToFuncID;
    std::map<int, message_id_saver> msgIdToDiscoverID;
    std::map<int, nroute> routes;
    std::vector<waiting_route_response> respond_back_list;

    sockaddr_in incomming_connection;
    socklen_t addr_size = sizeof(incomming_connection);

    // this are messages collected at user thread.
    std::stringstream ulog;
    // this are messages collected at server thread.
    std::stringstream slog;
    // this are messages collected at server thread.
    std::stringstream plog;

    void handle_connection();
    void server_loop();
    void user_loop();

    bool running = false;
    char buff[buff_size];
    uint msg_size = 0;
    // CLIENT SIDE ( SHOULD BE PRIVATE )
    void handleNodeInput(int sock);
    bool handleUserInput();

    void coutLog(std::stringstream &log);
    int createUniqueMsgID();

    void remove_sock(int sock);
    void send_discover(int did);
    void handle_rout_update(int discover_id, NodeMessage &ms);
    int message_id_counter = 1;
    bool on_wait = false;
    bool on_route = false;
    bool block_server = false;
    bool block_user = false;

public:
    Node(Utilities::Address &address, int max_connections, bool listen_to_input);
    ~Node();

    void start_server();
    void close_server();
    // set the id of the node.
    virtual void setid(std::string id);
    // connect to node given ip and port.
    virtual int connect_to(Utilities::Address &address);
    // send message of length len, to node(id).
    virtual void send_message(std::vector<std::string> userinput);
    // print the rout to a given node ( exp 1->5->3->2 ).
    virtual void route(std::string id);
    // print the list of all connected nodes.
    virtual void peers();

    // SERVER
    virtual void send_ack(int sock, NodeMessage &incoming_message);
    virtual void send_nack(int sock, NodeMessage &incoming_message);
    virtual void handle_discover(NodeMessage &incoming_message);
    virtual void send_netm(int sock, NodeMessage &message);
    virtual void connect_tcp(std::string ipport);
};