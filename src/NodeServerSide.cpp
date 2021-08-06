// HERE WE WOULD IMPLEMENT THE VIRTUAL METHODS
#include "headers/Utilities.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

#include "headers/Node.hpp"
using namespace std;
using namespace Utilities;

int Node::connect_to(Utilities::Address &address) {
    int other_node = -1;
    struct sockaddr_in other_addr;
    other_node = socket(AF_INET, SOCK_STREAM, 0);
    if (other_node < 0) {
        ulog << "NODE CONNECT ERROR : Error connecting to socket \n";
        plog << "Nack" << endl;
        return -1;
    }
    other_addr.sin_family = AF_INET;
    other_addr.sin_port = htons(address.port);
    int p = inet_pton(AF_INET, &address.ip[0], &other_addr.sin_addr);
    if (p <= 0) {
        ulog << "NODE CONNECT ERROR : inet_pton has failed...\n";
        plog << "Nack" << endl;
        close(other_node);
        return -1;
        //exit(1);
    }
    //printf("Connecting to server...\n");
    if (connect(other_node, (struct sockaddr *)&other_addr, sizeof(other_addr)) == -1) {
        ulog << "NODE CONNECT ERROR : failed to connect to node...\n";
        plog << "Nack" << endl;
        close(other_node);
        return -1;
        //exit(1);
    }
    //printf("Connected to server\n");
    this->sockToAddr.insert({other_node, address});
    listner.add_descriptor(other_node);
    return other_node;
}

void Node::send_message(vector<string> userinput) {
    if (userinput.size() < 4) {
        ulog << "Nack : invalid arguments" << endl;
        plog << "Nack" << endl;
    }

    if (this->socketToId.size() == 0) {
        ulog << "Nack : No connections!" << endl;
        plog << "Nack" << endl;
        return;
    }

    int other_id = std::atoi(&userinput[1][0]);
    int other_sock = idToSocket[other_id];
    if (other_sock == 0) {
        idToSocket.erase(other_id);
        ulog << "Nack, not supported messages between unconnected nodes..." << endl;
        plog << "Nack";
        return;
    }

    int len = std::atoi(&userinput[2][0]);
    string message = userinput[3];
    for (uint i = 4; i < userinput.size(); i++) {
        message += "," + userinput[i];
    }

    NodeMessage nm;
    nm.source_id = this->node_id;
    nm.destination_id = other_id;
    nm.function_id = net_fid::send;
    nm.msg_id = this->createUniqueMsgID();
    *(int *)nm.payload = len;
    nm.setPayload(message, 4);

    // THIS SHOULD BE THE COMMENTED LINE FROM BELOW!
    //int other_sock = other_id;
    this->send_netm(other_sock, nm);
}

void Node::send_netm(int sock, NodeMessage &message) {
    // if not ack or nack
    if (message.function_id > 2 and message.function_id != net_fid::route) {
        message_id_saver ms;
        ms.save_data = message.function_id;
        ms.dest_id = message.destination_id;
        this->msgIdToFuncID[message.msg_id] = ms;
    }
    send(sock, &message, sizeof(message), 0);
}

void Node::setid(std::string id) {
    this->node_id = std::atoi(&id[0]);
    if (this->node_id == 0) {
        this->node_id = -1;
        ulog << "Nack : id must be a positive integer." << endl;
        plog << "Nack" << endl;
        return;
    }
    ulog << "new id : " << this->node_id << endl;
    plog << "Ack" << endl;
}

void Node::connect_tcp(std::string ipport) {
    auto split = Utilities::splitBy(ipport, ':');
    auto address = Address{split[0], std::atoi(&split[1][0])};

    for (auto add : sockToAddr) {
        if (add.second == address) {
            // we already connected to that address
            plog << "Nack" << endl;
            return;
        }
    }

    int fd = this->connect_to(address);
    if (fd == -1) {
        return;
    }

    NodeMessage nm;
    nm.msg_id = this->createUniqueMsgID();
    nm.source_id = this->node_id;
    nm.destination_id = 0;
    nm.function_id = net_fid::connect;

    // we save in this map all messages we want to keep track of,
    // when we get nack or ack we would remove the message from the list.
    //this->msgIdToMsg[nm.msg_id] = nm.function_id;
    this->send_netm(fd, nm);
}

void Node::send_ack(int sock, NodeMessage &incoming_message) {
    NodeMessage nm;
    nm.msg_id = this->createUniqueMsgID();
    nm.source_id = this->node_id;
    nm.destination_id = incoming_message.source_id;
    nm.function_id = net_fid::ack;
    *(int *)nm.payload = incoming_message.msg_id;

    this->send_netm(sock, nm);
    slog << "sent ack message..." << endl;
    //((*int)nm.payload) = incoming_message.msg_id;
}

void Node::send_nack(int sock, NodeMessage &incoming_message) {
    NodeMessage nm;
    nm.msg_id = this->createUniqueMsgID();
    nm.source_id = this->node_id;
    nm.destination_id = incoming_message.source_id;
    nm.function_id = net_fid::nack;
    *(int *)nm.payload = incoming_message.msg_id;
    //*nm.payload = incoming_message.msg_id;
    //((*int)nm.payload) = incoming_message.msg_id;
    this->send_netm(sock, nm);
    slog << "sent nack message..." << endl;
    slog << nm << endl;
}