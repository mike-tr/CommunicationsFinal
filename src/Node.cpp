#include "headers/Node.hpp"
#include <iostream>
#include <unistd.h>
//#include "headers/Utilities.hpp"

using namespace std;

const int buff_size = 512;

Node::Node(Utilities::Address &address, int max_connections, bool listen_to_input)
    : INodeNet(), INodeUser(), my_address(address), listner(listen_to_input) {
    struct sockaddr_in serv_addr;
    this->max_connections = max_connections;
    if ((this->server = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        cerr << "socket failed" << endl;
        exit(EXIT_FAILURE);
    }

    // int free_sock = 1;
    // if (setsockopt(this->server, SOL_SOCKET, SO_REUSEADDR, &free_sock, sizeof free_sock) == -1)
    // {
    //     perror("failed to free socket\n");
    //     exit(1);
    // }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(address.port);
    int p = inet_pton(AF_INET, &address.ip[0], &serv_addr.sin_addr);
    if (p <= 0) {
        cerr << "inet_pton has failed...\n";
        exit(1);
    }

    if (bind(this->server, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << " Failed to bind server to speciefied ip and port " << endl;
        exit(1);
    }
}

void Node::close_server() {
    running = false;
    listner.stop();
    sleep(0.1);
}

Node::~Node() {
    listner.stop();
    running = false;
    close(this->server);
    //w84connections.join();

    for (auto sock : sockToAddr) {
        close(sock.first);
    }
    cout << "~Node" << endl;
}

void Node::start_server() {
    // this method start the server
    // and set the sock for server to handle connections.
    // i.e marks the socket to handle connections.
    cout << "Starting Node..." << endl;

    this->running = true;
    cout << "waiting for new connections..." << endl;
    if (listen(this->server, this->max_connections) != 0) {
        cerr << " There's an ERROR in listening " << endl;
        exit(1);
    }
    listner.add_descriptor(this->server);
    this->server_loop();
}

void Node::handle_connection() {
    // this bit of code accepts a client and put it into the fd listener.
    // that way when we get messages from said client we can handle them.
    memset(&incomming_connection, 0, sizeof(incomming_connection));
    int client = accept(this->server, (struct sockaddr *)&incomming_connection, &addr_size);
    if (client == -1) {
        cerr << "failed to accept client\n";
        close(client);
    }

    char ip[16] = {0};
    inet_ntop(AF_INET, &incomming_connection.sin_addr.s_addr, ip, 15);
    uint16_t port = ntohs(incomming_connection.sin_port);
    //ulog << "connected succsesfully to " << ip << ":" << port << endl;
    //printf("connected succsesfully to %s:%d \n", ip, port);

    Utilities::Address add{ip, port};
    this->sockToAddr.insert({client, add});
    // this->connections[client] = add;

    // We send a message to the socket inorder to update and get new data.
    listner.add_descriptor(client);
}

void Node::server_loop() {
    // this piece of code gets a message from any connected socket,
    // and maps it to the right function
    cout << "listening to all..." << endl;
    usert = thread{&Node::user_loop, this};
    while (this->running) {
        slog << "\nwaiting for input...\n";
        int sock = listner.wait_for_input();
        slog << "fd: " << sock << " is ready. reading..." << endl;
        // this simply reads the bytes send from the right socket
        // Notice : that you want to remove any garbage from buff.
        if (sock == -1) {
            cerr << "Error in input... " << strerror(errno) << endl;
            //cout << "Error in input... " << strerror(errno) << endl;
        } else if (sock == -2) {
            usert.join();
            cout << "got stop message" << endl;
            return;
        } else if (sock == this->server) {
            // NEW CONNECTION
            this->handle_connection();
        } else {
            // TCP message, recived
            this->handleNodeInput(sock);
        }
    }
}

int Node::createUniqueMsgID() {
    // give a "unique" msg id, by forcibly adding nodeid at the start,
    // we force each node have different msg id sent by them.
    string m = to_string(this->node_id) + to_string(this->message_id_counter++);
    return std::atoi(&m[0]);
}

void Node::remove_sock(int sock) {
    // delete a given scoket
    this->listner.remove_descriptor(sock);
    this->sockToAddr.erase(sock);

    // if we knew the id of the socket we remove the id record.
    int disconnected_id = this->socketToNodeData[sock].node_id;
    if (disconnected_id != 0) {
        this->socketToNodeData.erase(sock);
        this->idToSocket.erase(disconnected_id);
    }
    this->listner.interupt();

    // we trace message id's we send in order to handle different responses
    // acks/nacks/routes, hence if the client was dissconected but was suppose to return a message
    // we just drop it.
    vector<int> remove_later;
    for (auto m : this->msgIdToFuncID) {
        if (m.second.dest_id == disconnected_id) {
            remove_later.push_back(m.first);
        }
    }

    // we acan remove directly so that's how it is done.
    for (auto r : remove_later) {
        this->msgIdToDiscoverID.erase(r);
        this->msgIdToFuncID.erase(r);
    }

    close(sock);
}