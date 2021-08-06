#include "headers/Node.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>

using namespace std;

void Node::handleNodeInput(int sock) {
    while (this->block_server) {
        sleep(0.1);
    }

    // ON reciving a new message from another node
    memset(this->buff, 0, this->buff_size);
    this->msg_size = read(sock, buff, buff_size);
    this->block_user = true;

    if (this->msg_size == 0) {
        slog << "disconnected..." << endl;
        this->remove_sock(sock);
    } else {
        // IF the message length is not zero
        // we want to read it.
        // and handle the messages
        // the message suppose to be NodeMessage, if failed catch would catch it.
        try {
            // load the message from buff
            NodeMessage *ms = (NodeMessage *)buff;
            slog << "message from server size is : " << this->msg_size << endl;
            slog << ms->to_string() << endl;

            //  On connect message
            if (ms->function_id == net_fid::connect) {
                bool ncon = true;
                for (auto i : idToSocket) {
                    if (i.first == ms->source_id) {
                        ncon = false;
                        break;
                    }
                }

                if (ms->destination_id != 0 or !ncon) {
                    // wrong format, return nack.
                    // or infact we are already have such connection.
                    // we cannot be connected twice to the same nodeid
                    this->send_nack(sock, *ms);
                    this->remove_sock(sock);
                } else {
                    // right format, we add to list.
                    slog << "connected to : " << ms->source_id << endl;
                    this->send_ack(sock, *ms);
                    this->socketToId[sock] = ms->source_id;
                    this->idToSocket[ms->source_id] = sock;
                }
            }
            // On ack message
            else if (ms->function_id == net_fid::ack) {
                int mid = *(int *)ms->payload;
                auto type = this->msgIdToFuncID[mid];
                // if ack for connect message we wish to add the other
                // node to our neibour nodes and maybe notify everyone else.
                if (type.save_data == net_fid::connect) {
                    this->socketToId[sock] = ms->source_id;
                    this->idToSocket[ms->source_id] = sock;
                    slog << "\nConnected to : " << sockToAddr[sock] << " , id :" << ms->source_id << endl;
                }
                plog << "Ack" << endl;
                this->msgIdToFuncID.erase(mid);
            } else if (ms->function_id == net_fid::nack) {
                int mid = *(int *)ms->payload;
                auto type = this->msgIdToFuncID[mid];
                // if ack for connect message we wish to add the other
                // node to our neibour nodes and maybe notify everyone else.

                if (type.save_data == net_fid::connect) {
                    slog << "\nError, Connection failed." << endl
                         << "On : " << sockToAddr[sock] << " , id :" << ms->source_id << endl;
                    remove_sock(sock);
                    // this->socketToId[sock] = ms->source_id;
                    // this->idToSocket[ms->source_id] = sock;
                } else if (type.save_data == net_fid::discover) {
                    auto discover_id = msgIdToDiscoverID[mid];
                    this->handle_rout_update(discover_id.save_data, *ms);
                    this->msgIdToDiscoverID.erase(mid);
                } else {
                    plog << "Nack" << endl;
                }
                this->msgIdToFuncID.erase(mid);
            } else if (ms->function_id == net_fid::send) {
                if (ms->destination_id == this->node_id) {
                    this->send_ack(sock, *ms);
                } else {
                    this->send_nack(sock, *ms);
                }
            } else if (ms->function_id == net_fid::discover) {
                this->handle_discover(*ms);
            } else if (ms->function_id == net_fid::route) {
                int mid = *(int *)ms->payload;
                auto discover_id = msgIdToDiscoverID[mid];
                this->handle_rout_update(discover_id.save_data, *ms);
                this->msgIdToFuncID.erase(mid);
                this->msgIdToDiscoverID.erase(mid);
            }
        } catch (exception exp) {
            slog << "Got wrong message format of size : " << this->msg_size << endl;
        }
    }
    this->coutLog(this->slog);
    this->block_user = false;
}

bool Node::handleUserInput() {
    // here we read user input
    memset(this->buff, 0, this->buff_size);
    if (fgets(buff, buff_size, stdin)) {
        // print user input ( if its from file we want to see given input )
        ulog << buff << endl;
    } else {
        // shoudn't happen if STDIN is both file and user input.
        cout << "EOF!" << endl;
        return false;
    }
    ulog << "my id : " << this->node_id << endl;
    while (this->block_user) {
        sleep(0.1);
    }

    while (this->msgIdToFuncID.size() > 0) {
        cout << "Node is currently on wait..." << endl;
        for (auto m : this->msgIdToFuncID) {
            cout << "waiting for response to : " << m.first
                 << ", " << net_fid::fid_tostring(m.second.save_data) << endl;
        }
        sleep(1);
    }

    this->block_server = true;

    // USER INPUT
    auto split = Utilities::splitBy(buff, ',');

    // when no id given we force user to give id first.
    if (this->node_id == -1) {
        if (split[0] != "setid") {
            ulog << "setid first!" << endl;
            ulog << "nack" << endl;
            plog << "Nack" << endl;
        } else if (this->formatCheck(split, 2)) {
            this->setid(split[1]);
        }
    } else if (split[0] == "connect") {
        // handle connect.
        if (this->formatCheck(split, 2)) {
            this->connect_tcp(split[1]);
        }
    } else if (split[0] == "send") {
        // handle send message
        this->send_message(split);
    } else if (split[0] == "route") {
        if (this->formatCheck(split, 2)) {
            this->route(split[1]);
        }
    } else if (split[0] == "peers") {
        // handle show peers.
        if (this->formatCheck(split, 1)) {
            this->peers();
        }
    } else if (split[0] == "sleep") {
        // i.e if we want to write an automatic script,
        // we would sometimes need to wait.
        sleep(1);
    } else if (split[0] == "exit") {
        // a command to close server in a "good" maner.
        this->close_server();
        return false;
    } else {
        // comman is not supported ( yet or at all ).
        ulog << "Nack : Invalid input!" << endl;
        plog << "Nack" << endl;
    }
    this->coutLog(this->ulog);
    this->block_server = false;
    return true;
}

void Node::peers() {
    ulog << "Ack" << endl;
    plog << "Ack" << endl;
    int i = 0;
    for (auto a : this->sockToAddr) {
        if (i > 0) {
            plog << ",";
        }
        int id = this->socketToId[a.first];
        ulog << a.second.ip << ":" << a.second.port << " , NODE ID : " << id << endl;
        plog << id;
        i++;
    }
    plog << endl;
}

void Node::user_loop() {
    // start a thread exclusively for listening for user input.
    cout << "listening to stdin..." << endl;
    while (this->running) {
        try {
            if (this->handleUserInput()) {
                // when reading from file i want some delay,
                // for a human that sleep is non existent
                sleep(0.1);
            } else {
                break;
            }
        } catch (exception e) {
            cout << "ops something went wrong on user input..." << endl;
            // do nothing...
        }
    }
    cout << "exiting user thread" << endl;
}

void Node::coutLog(stringstream &log) {
    // Log out in a compact manner all messages accumulated in the stringstrean
    cout << "----------------- LOG -----------------" << endl;
    cout << log.str();
    log.str(string{});
    cout << "-------------------------------------" << endl;
    cout << this->plog.str();
    this->plog.str(string{});
}