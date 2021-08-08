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

// struct Relaydata{
//     int trailing
// }

struct NodeData {
    int node_id;

    NodeData() {
        node_id = 0;
    }

    NodeData(int id) {
        node_id = id;
    }
};

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
    std::map<int, NodeData> socketToNodeData;
    std::map<int, message_id_saver> msgIdToFuncID;
    std::map<int, message_id_saver> msgIdToDiscoverID;
    std::map<int, nroute> routes;
    std::vector<waiting_route_response> respond_back_list;

    sockaddr_in incomming_connection;
    socklen_t addr_size = sizeof(incomming_connection);

    // this are messages collected at user thread, logs that we can see what's happening
    std::stringstream ulog;
    // this are messages collected at server thread, logs that we can see what's happening
    std::stringstream slog;
    // this are messages that we are told to print exclusivly.
    std::stringstream plog;

    // here we handle new connection
    void handle_connection();
    // this is the main loop for server, i.e here we "listen" to messages from other nodes.
    void server_loop();
    // this is the main loop for user input, i.e here we "listen" to user input.
    void user_loop();

    // if node is not running we terminate the threads.
    bool running = false;
    // the main buffer simply so we wont alocate resources everytime.
    char buff_server[buff_size];
    char buff_user[buff_size];
    // the size of the last message read,
    uint msg_size = 0;

    // here we write the logic for messages recived from other nodes.
    void handleNodeInput(int sock);
    // here we write the logic for messages recived from user.
    bool handleUserInput();

    // here we print and flush the stringstream, we also print plog.
    void coutLog(std::stringstream &log);
    // method that generate a unique message id, that is based on this node id.
    int createUniqueMsgID();

    // remove the socket, i,e when connection is lost we want to reset all data regarding that socket
    // and stop listening to that socket.
    void remove_sock(int sock);
    // here we send to all neibours a discover message
    void send_discover(int did);
    // here we handle route update, i.e if we get nack or route for a discover message,
    // we update the route acordingly
    void handle_route_update(int discover_id, const NodeMessage &ms);
    // this is the "main" part of find route, i.e call this on user thread in order
    // to find a route to specified node, i.e this does nothing if path already known
    // this method also check's and refind the path if path is timedout.
    void find_route_user(int target_id);
    // this method sends the message in a relay
    // assumes that the message destination id, is not a neibour!
    // i.e this method has undefiend behaviour on neibouring nodes.
    void send_relay(const NodeMessage &message);

    // here we specify the logic when we get a ack message
    // i.e for example on ack for relay we send ack to prev node.
    void ack_handle(int sock, const NodeMessage &incoming_message);
    // specify the logic on nack's
    // i.e on route nack, we update the route acordingly ( we check that path not found via that node. )
    void nack_handle(int sock, const NodeMessage &incoming_message);
    // here we "mark" a message id, and we will know how to respond to answers for that message
    message_id_saver &add_funcID_by_MessageID(const NodeMessage &message);

    // counter so we can create unique message id.
    int message_id_counter = 1;

    // we set it to force the user thread to wait untill route response is done.
    bool on_route_wait = false;
    // block server thread.
    bool block_server = false;
    // block user thread.
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

private:
    // SERVER
    // send ack on given message to specified sock
    virtual void send_ack(int sock, const NodeMessage &incoming_message);
    // send nack on given message to specified sock
    virtual void send_nack(int sock, const NodeMessage &incoming_message);
    // here we specify logic when we get a discover message
    virtual void handle_discover(NodeMessage &incoming_message);
    // send given message to sock, and save "messageID" if needed.
    virtual void send_netm(int sock, const NodeMessage &message);
    // given an ip and port connect to specified ip and port
    virtual void connect_tcp(std::string ipport);
    // here we specify the logic when we get a relay message
    virtual void handle_relay(int sock, const NodeMessage &incoming_message);
};