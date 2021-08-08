#include "headers/Utilities.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

#include "headers/Node.hpp"
using namespace std;
using namespace Utilities;

void Node::ack_handle(int sock, const NodeMessage &incoming_message) {
    int mid = *(int *)incoming_message.payload;
    auto &type = this->msgIdToFuncID[mid];
    slog << "this Ack for message " << net_fid::fid_tostring(type.save_data) << endl;
    // if ack for connect message we wish to add the other
    // node to our neibour nodes and maybe notify everyone else.
    if (type.save_data == net_fid::connect) {
        this->socketToNodeData[sock] = incoming_message.source_id;
        this->idToSocket[incoming_message.source_id] = sock;
        slog << "\nConnected to : " << sockToAddr[sock] << " , id :" << incoming_message.source_id << endl;
        plog << "\nConnected to : " << incoming_message.source_id << endl;
        plog << "Ack" << endl;
    } else if (type.save_data == net_fid::relay) {
        if (type.prev == nullptr) {
            slog << "Successfully sent relay message..." << endl;
            plog << "Ack" << endl;

            this->msgIdToRelayTarget.erase(mid);
        } else {
            //cout << "sending back ack to prev" << endl;
            slog << "relaying back an ack message..." << endl;
            NodeMessage relay_back_ack;
            relay_back_ack.function_id = net_fid::ack;
            relay_back_ack.source_id = this->node_id;
            if (type.prev != nullptr) {
                relay_back_ack.destination_id = type.prev->dest_id;
                *(int *)relay_back_ack.payload = type.prev->save_data;
                relay_back_ack.msg_id = this->createUniqueMsgID();
                relay_back_ack.trailing_msg = 0;

                // cout << "sending the message?" << endl;
                // cout << this->idToSocket[relay_back_ack.destination_id] << endl;
                // cout << relay_back_ack << endl;

                this->send_netm(this->idToSocket[relay_back_ack.destination_id], relay_back_ack);
            } else {
                cout << "wtf something went wrong the prev is null" << endl;
            }
        }
    }
    this->msgIdToFuncID.erase(mid);
}

void Node::nack_handle(int sock, const NodeMessage &incoming_message) {
    int mid = *(int *)incoming_message.payload;
    auto &type = this->msgIdToFuncID[mid];
    // if ack for connect message we wish to add the other
    // node to our neibour nodes and maybe notify everyone else.

    if (type.save_data == net_fid::connect) {
        slog << "\nError, Connection failed." << endl
             << "On : " << sockToAddr[sock] << " , id :" << incoming_message.source_id << endl;
        plog << "\nConnection to failed..." << endl;
        plog << "Nack" << endl;
        remove_sock(sock);
        // this->socketToId[sock] = ms->source_id;
        // this->idToSocket[ms->source_id] = sock;
    } else if (type.save_data == net_fid::discover) {
        auto discover_id = msgIdToDiscoverID[mid];
        this->handle_route_update(discover_id.save_data, incoming_message);
        this->msgIdToDiscoverID.erase(mid);
    } else if (type.save_data == net_fid::relay) {
        if (type.prev == nullptr) {
            slog << "Failed to send relay message..." << endl;
            plog << "Nack" << endl;

            int target = this->msgIdToRelayTarget[mid];
            this->routes[target].dump();
            this->msgIdToRelayTarget.erase(mid);
        } else {
            slog << "relaying back an nack message..." << endl;
            NodeMessage relay_back_nack;
            relay_back_nack.function_id = net_fid::nack;
            relay_back_nack.source_id = this->node_id;
            relay_back_nack.destination_id = type.prev->dest_id;
            *(int *)relay_back_nack.payload = type.prev->save_data;
            relay_back_nack.msg_id = this->createUniqueMsgID();
            relay_back_nack.trailing_msg = 0;

            this->send_netm(this->idToSocket[relay_back_nack.destination_id], relay_back_nack);
        }
    } else {
        plog << "Nack" << endl;
    }
    this->msgIdToFuncID.erase(mid);
}