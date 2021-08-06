#include "headers/Utilities.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

#include "headers/Node.hpp"
using namespace std;
using namespace Utilities;

const double stimeout = 60;

void Node::handle_rout_update(int target_id, NodeMessage &ms) {
    auto &rout = this->routes[target_id];

    rout.update(ms);
    slog << "handeling discover response..." << endl;

    if (rout.responses == this->socketToId.size()) {
        // this means that all the neibouring nodes have responded to the discover.
        // hence we have the shortest path ( if any exists ).
        slog << "reporting back to all the waiting nodes..." << endl;
        if (rout.status == discover_status::found) {
            slog << "route has been found!!" << endl;
            slog << "route is : " << rout.to_string() << endl;

            auto r2 = this->routes[target_id];
            slog << r2.responses << " ," << target_id << endl;

            // send back the rout to all nodes that asked for a rout to the target
            NodeMessage route_message;
            route_message.source_id = this->node_id;
            route_message.function_id = net_fid::route;
            route_message.trailing_msg = 0;
            auto rn = rout;
            rn.path.push_front(this->node_id);
            rn.pack_msg(route_message);

            vector<waiting_route_response> res;
            for (auto wr : this->respond_back_list) {
                if (wr.target_id == target_id) {
                    int sock = this->idToSocket[wr.source_id];
                    route_message.msg_id = this->createUniqueMsgID();
                    route_message.destination_id = wr.source_id;
                    (*(int *)route_message.payload) = wr.message_id;
                    this->send_netm(sock, route_message);
                }
            }
        } else {
            // recreate the discover message, and send back the response.
            NodeMessage original_msg;
            original_msg.destination_id = this->node_id;
            original_msg.function_id = net_fid::discover;
            (*(int *)original_msg.payload) = target_id;
            for (auto wr : this->respond_back_list) {
                if (wr.target_id == target_id) {
                    int sock = this->idToSocket[wr.source_id];
                    original_msg.msg_id = wr.message_id;
                    original_msg.source_id = wr.source_id;
                    this->send_nack(sock, original_msg);
                }
            }
        }
        this->on_wait = false;
    }
}

void Node::send_discover(int target_id) {
    auto &rout = this->routes[target_id];
    slog << "did : " << target_id << ", " << rout.responses << endl;
    for (auto rrr : rout.responded) {
        slog << "got respond from " << rrr.first << endl;
        slog << rout.responded[rrr.first] << endl;
    }
    NodeMessage dmsg;
    dmsg.source_id = this->node_id;
    dmsg.function_id = net_fid::discover;
    (*(int *)dmsg.payload) = target_id;
    for (auto ntd : this->socketToId) {
        slog << "checking sock " << ntd.second << endl;
        if (rout.responded[ntd.second]) {
            slog << "skip sock..." << endl;
            continue;
        }
        slog << "sending discover to that socket..." << endl;
        dmsg.destination_id = ntd.second;
        dmsg.msg_id = this->createUniqueMsgID();

        message_id_saver msv;
        msv.save_data = target_id;
        msv.dest_id = ntd.second;
        this->msgIdToDiscoverID[dmsg.msg_id] = msv;
        this->send_netm(ntd.first, dmsg);
    }
}

void Node::route(std::string sid) {
    int target_id = std::atoi(&sid[0]);
    uint num_connections = this->socketToId.size();
    if (num_connections == 0) {
        ulog << "no such route, there is no connections!" << endl;
        plog << "Nack" << endl;
        return;
    }

    for (auto ntd : this->socketToId) {
        if (ntd.second == target_id) {
            ulog << "Directly connected to : " << target_id << endl;
            plog << "Ack";
            return;
        }
    }

    auto &rout = this->routes[target_id];
    rout.remove_ignore_id();
    if (rout.check_valid(stimeout) && rout.responses == num_connections) {
        if (rout.status == discover_status::empty) {
            ulog << "no such route..." << endl;
            plog << "Nack" << endl;
            return;
        } else {
            ulog << "route : " << rout.to_string() << endl;
            plog << rout.to_string() << endl;
            return;
        }
    }

    this->on_wait = true;
    this->on_route = true;
    this->send_discover(target_id);

    this->block_server = false;
    while (this->on_wait) {
        sleep(0.1);
        //cout << "waiting..." << endl;
    }
    this->block_server = true;
    const auto rr = this->routes[target_id];
    ulog << rr.responses << ", " << target_id << endl;
    if (rr.status == discover_status::found) {
        ulog << "Ack, found route to : " << target_id << endl;
        ulog << rout.to_string() << endl;
        plog << "Ack" << endl
             << rout.to_string() << endl;
    } else {
        ulog << "Nack, no route route to : " << target_id << endl;
        plog << "Nack" << endl;
    }
}

void Node::handle_discover(NodeMessage &incoming_message) {
    int target_id = *(int *)incoming_message.payload;
    slog << "\n\nhandeling incoming discover message" << endl;

    for (auto ntd : this->socketToId) {
        if (ntd.second == target_id) {
            slog << "connected to " << target_id << ", sending route..." << endl;
            nroute pres;
            pres.path.push_front(target_id);
            pres.path.push_front(this->node_id);

            NodeMessage response;
            response.destination_id = incoming_message.source_id;
            response.source_id = this->node_id;
            response.function_id = net_fid::route;
            response.msg_id = this->createUniqueMsgID();
            *(int *)response.payload = incoming_message.msg_id;
            pres.pack_msg(response);

            int sock = this->idToSocket[incoming_message.source_id];

            this->send_netm(sock, response);
            return;
        }
    }

    auto &route = this->routes[target_id];
    slog << (int)route.status << ", " << route.responses << endl;
    if (route.check_valid(stimeout)) {
        route.update(incoming_message);
        if (this->socketToId.size() == route.responses) {
            int sock = idToSocket[incoming_message.source_id];
            if (route.status == discover_status::empty) {
                slog << "no path sending nack back" << endl;
                this->send_nack(sock, incoming_message);
                return;
            } else if (route.status == discover_status::found) {
                NodeMessage msg;
                msg.source_id = this->node_id;
                msg.function_id = net_fid::route;
                route.update(incoming_message);
                msg.destination_id = incoming_message.source_id;
                msg.msg_id = this->createUniqueMsgID();
                msg.trailing_msg = 0;
                *(int *)msg.payload = incoming_message.msg_id;
                auto route_copy = route;
                route_copy.path.push_front(this->node_id);

                route_copy.pack_msg(msg);
                this->send_netm(sock, msg);
                return;
            }
        }
    }

    slog << "need to ask for discover on other nodes" << endl;
    route.update(incoming_message);
    slog << route.responses << endl;
    waiting_route_response res{incoming_message.source_id, target_id, incoming_message.msg_id};
    this->respond_back_list.push_back(res);
    this->send_discover(target_id);
}