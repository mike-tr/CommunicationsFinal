// HERE WE WOULD IMPLEMENT THE VIRTUAL METHODS
#include "headers/Utilities.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

#include "headers/Node.hpp"
using namespace std;
using namespace Utilities;

int Node::connect_to(Utilities::Address &address) {
    // in here we establish tcp connection to the given ip and port.
    int other_node = -1;
    struct sockaddr_in other_addr;
    // create the tcp socket.
    other_node = socket(AF_INET, SOCK_STREAM, 0);
    if (other_node < 0) {
        ulog << "NODE CONNECT ERROR : Error connecting to socket \n";
        plog << "Nack" << endl;
        return -1;
    }
    other_addr.sin_family = AF_INET;
    other_addr.sin_port = htons(address.port);
    // set the ip and port of the server
    int p = inet_pton(AF_INET, &address.ip[0], &other_addr.sin_addr);
    if (p <= 0) {
        ulog << "NODE CONNECT ERROR : inet_pton has failed...\n";
        plog << "Nack" << endl;
        close(other_node);
        return -1;
    }

    // connect to the server, and bind server responses to the newly created socket.
    if (connect(other_node, (struct sockaddr *)&other_addr, sizeof(other_addr)) == -1) {
        ulog << "NODE CONNECT ERROR : failed to connect to node...\n";
        plog << "Nack" << endl;
        close(other_node);
        return -1;
        //exit(1);
    }

    // add the server to an address list
    // and start listening to messages on the newly created socket.
    this->sockToAddr.insert({other_node, address});
    listner.add_descriptor(other_node);
    return other_node;
}

void Node::send_netm(int sock, const NodeMessage &message) {
    // simply given a NodeMEssage we send it to the right socket
    if (message.function_id > 2 and message.function_id != net_fid::route) {
        // if the message function is not nack ack or route
        // we would wish to get an ack or nack message in return
        // that is we also want to know for what message it was so we save the message id.
        // as tuples [MessageID, MessageFuncion]
        this->add_funcID_by_MessageID(message);
    }
    send(sock, &message, sizeof(message), 0);
}

void Node::setid(std::string id) {
    // set the id of the client.
    this->node_id = std::atoi(&id[0]);
    if (this->node_id == NULLID) {
        // we use id 0, to specify that this id is null.
        // due to it being the default value of int.
        this->node_id = NULLID;
        ulog << "Nack : id cant be 0" << endl;
        plog << "Nack" << endl;
        return;
    }
    ulog << "new id : " << this->node_id << endl;
    plog << "Ack" << endl;
}

void Node::connect_tcp(std::string ipport) {
    // given from user 127.0.0.1:5050
    // we break it dont to 127.0.0.1 and 5050
    // and try to create connection with the given ip and port.
    auto split = Utilities::splitBy(ipport, ':');
    auto address = Address{split[0], std::atoi(&split[1][0])};

    // if we already have connection with that address, we dont want secondary conenction
    // so we return nack.
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

void Node::send_ack(int sock, const NodeMessage &incoming_message) {
    // given an incoming message send ack on it.
    NodeMessage nm;
    nm.msg_id = this->createUniqueMsgID();
    nm.source_id = this->node_id;
    nm.destination_id = this->socketToNodeData[sock].node_id;
    nm.function_id = net_fid::ack;
    *(int *)nm.payload = incoming_message.msg_id;

    this->send_netm(sock, nm);
    slog << "sent ack message..." << endl;
    //((*int)nm.payload) = incoming_message.msg_id;
}

void Node::send_nack(int sock, const NodeMessage &incoming_message) {
    // given incoming message send nack on it.
    NodeMessage nm;
    nm.msg_id = this->createUniqueMsgID();
    nm.source_id = this->node_id;
    nm.destination_id = this->socketToNodeData[sock].node_id;
    nm.function_id = net_fid::nack;
    *(int *)nm.payload = incoming_message.msg_id;
    this->send_netm(sock, nm);
    slog << "sent nack message..." << endl;
}

message_id_saver &Node::add_funcID_by_MessageID(const NodeMessage &message) {
    // save the message id, and funcion and dest for future use. ( handle ack nack or route )
    message_id_saver &ms = this->msgIdToFuncID[message.msg_id];
    ms.save_data = message.function_id;
    ms.dest_id = message.destination_id;
    return ms;
}