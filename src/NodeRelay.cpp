#include "headers/Utilities.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

#include "headers/Node.hpp"
using namespace std;
using namespace Utilities;

void Node::send_message(vector<string> userinput) {
    if (userinput.size() < 4) {
        ulog << "Nack : invalid arguments" << endl;
        plog << "Nack" << endl;
    }

    int len = std::atoi(&userinput[2][0]);
    string message = userinput[3];
    for (uint i = 4; i < userinput.size(); i++) {
        message += "," + userinput[i];
    }

    int target_id = std::atoi(&userinput[1][0]);
    if (target_id == this->node_id) {
        plog << "\nAck, message from my self :" << endl;
        plog << message << endl;
        return;
    }

    if (this->socketToNodeData.size() == 0) {
        ulog << "Nack : No connections!" << endl;
        plog << "Nack" << endl;
        return;
    }

    // BUILD MESSAGE
    NodeMessage nm;
    nm.source_id = this->node_id;
    nm.destination_id = target_id;
    nm.function_id = net_fid::send;
    nm.msg_id = this->createUniqueMsgID();
    *(int *)nm.payload = len;
    nm.setPayload(message, 4);

    // SEND TRAILING MESSAGE
    int other_sock = idToSocket[target_id];
    if (other_sock == 0) {
        // send via relay
        idToSocket.erase(target_id);
        this->send_relay(nm);
        return;
    }
    this->send_netm(other_sock, nm);
}

void Node::send_relay(const NodeMessage &send_message) {
    int target_id = send_message.destination_id;
    this->find_route_user(target_id);
    const auto route = this->routes[target_id];
    if (route.status == discover_status::empty) {
        ulog << "Nack : target node is not connected to the network" << endl;
        plog << "Nack" << endl;
        return;
    }

    NodeMessage relay_message;
    relay_message.source_id = this->node_id;
    relay_message.function_id = net_fid::relay;

    int sock = this->idToSocket[route.path[0]];

    int size = route.path.size() - 1;
    for (int i = 0; i < size; i++) {
        relay_message.trailing_msg = size - i;
        relay_message.destination_id = route.path[i];
        relay_message.msg_id = this->createUniqueMsgID();
        *(int *)relay_message.payload = route.path[i + 1];
        *((int *)relay_message.payload + 1) = size - 1;

        if (i == 0) {
            this->add_funcID_by_MessageID(relay_message);
        }
        ulog << "send ::\n"
             << relay_message << endl;
        send(sock, &relay_message, sizeof(relay_message), 0);
    }
    ulog << "send ::\n"
         << send_message << endl;
    send(sock, &send_message, sizeof(send_message), 0);
    //cout << "send all to " << sock << " , id : " << route

    //cout << "sent all relay messages waiting for response..." << endl;
}

void Node::handle_relay(int sock, const NodeMessage &incoming_message) {
    // this method gets non reference because it uses buff with might change incoming_message indirectly.
    // opsy.
    uint num_trailing = incoming_message.trailing_msg;
    int next = *(int *)incoming_message.payload;
    int next_sock = this->idToSocket[next];
    if (next_sock == 0) {
        slog << "cant relay messages as this node is not connected to target..." << endl;
        this->idToSocket.erase(next);
        this->send_nack(sock, incoming_message);
        return;
    }

    slog << "got trailing message, cupturing the next " << num_trailing << " messages..." << endl;
    slog << "messages are from " << this->socketToNodeData[sock].node_id << endl;
    this->coutLog(slog);

    map<int, NodeMessage> relays;
    while (relays.size() < num_trailing) {
        // cupture all trailing messages then resend them.
        memset(this->buff_server, 0, this->buff_size);
        this->msg_size = read(sock, buff_server, buff_size);
        if (this->msg_size == 0) {
            slog << "disconnected..." << endl;
            this->remove_sock(sock);
            return;
        }
        auto msg = *(NodeMessage *)this->buff_server;
        relays[num_trailing - msg.trailing_msg - 1] = msg;
        // cout << "got one, id : " << num_trailing - msg.trailing_msg - 1 << endl
        //      << relays[num_trailing - msg.trailing_msg - 1] << endl;
        //relays.push_back(*(NodeMessage *)this->buff);
    }

    slog << "cuptured all messages, sending them to the next node." << endl;
    for (uint i = 0; i < num_trailing; i++) {
        if (i == 0) {
            //incoming_message.function_id = net_fid::relay;
            auto &cup = this->add_funcID_by_MessageID(relays[i]);
            cup.prev = new message_id_saver{};
            cup.prev->dest_id = this->socketToNodeData[sock].node_id;
            cup.prev->save_data = incoming_message.msg_id;
            cup.save_data = net_fid::relay;

            // slog << "will wait for response for message " << relays[i].msg_id << endl;
            //cup.prev->dest_id
        }
        // slog << "send :: \n"
        //      << relays[i] << endl;
        send(next_sock, &relays[i], sizeof(relays[i]), 0);
    }
}