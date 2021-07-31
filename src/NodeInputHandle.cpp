#include "headers/Node.hpp"
#include <unistd.h>
#include <iostream>
#include <unistd.h>
#include <sstream>

using namespace std;

void Node::handleNodeInput(int sock)
{
    // ON reciving a new message from another node
    memset(this->buff, 0, this->buff_size);
    this->msg_size = read(sock, buff, buff_size);
    if (this->msg_size == 0)
    {
        slog << "disconnected..." << endl;
        this->remove_sock(sock);
    }
    else
    {
        // IF the message length is not zero
        // we want to read it.
        // and handle the messages
        // the message suppose to be NodeMessage, if failed catch would catch it.
        try
        {
            // load the message from buff
            NodeMessage *ms = (NodeMessage *)buff;
            slog << "message from server size is : " << this->msg_size << endl;
            slog << ms->to_string() << endl;

            //  On connect message
            if (ms->function_id == net_fid::connect)
            {
                if (ms->destination_id != 0)
                {
                    // wrong format, return nack.
                    this->nack(sock, *ms);
                    this->remove_sock(sock);
                }
                else
                {
                    // right format, we add to list.
                    slog << "connected to : " << ms->source_id << endl;
                    this->ack(sock, *ms);
                    this->socketToId[sock] = ms->source_id;
                    this->idToSocket[ms->source_id] = sock;
                }
            }
            // On ack message
            else if (ms->function_id == net_fid::ack)
            {
                int mid = *ms->payload;
                int type = this->msgIdToMsg[mid];
                // if ack for connect message we wish to add the other
                // node to our neibour nodes and maybe notify everyone else.
                if (type == net_fid::connect)
                {
                    this->socketToId[sock] = ms->source_id;
                    this->idToSocket[ms->source_id] = sock;
                }
                this->msgIdToMsg.erase(mid);
            }
            else if (ms->function_id == net_fid::nack)
            {
                int mid = *ms->payload;
                int type = this->msgIdToMsg[mid];
                // if nack on connect, we dissconnect.
                if (type == net_fid::connect)
                {
                    this->remove_sock(sock);
                }
                this->msgIdToMsg.erase(mid);
            }
        }
        catch (exception exp)
        {
            slog << "Got wrong message format of size : " << this->msg_size << endl;
        }
    }
    this->plog(this->slog);
}

bool Node::handleUserInput()
{
    // here we read user input
    memset(this->buff, 0, this->buff_size);
    if (fgets(buff, buff_size, stdin))
    {
        // print user input ( if its from file we want to see given input )
        ulog << buff << endl;
    }
    else
    {
        // shoudn't happen if STDIN is both file and user input.
        cout << "EOF!" << endl;
        return false;
    }
    ulog << "my id : " << this->node_id << endl;
    // USER INPUT
    auto split = Utilities::splitBy(buff, ',');

    // when no id given we force user to give id first.
    if (this->node_id == -1)
    {
        if (split[0] != "setid")
        {
            ulog << "setid first!" << endl;
            ulog << "nack" << endl;
        }
        else if (this->formatCheck(split, 2))
        {
            this->setid(split[1]);
        }
    }
    else if (split[0] == "connect")
    {
        // handle connect.
        if (this->formatCheck(split, 2))
        {
            this->connect_tcp(split[1]);
        }
    }
    else if (split[0] == "send")
    {
        // handle send message
        this->send_message(split);
    }
    else if (split[0] == "peers")
    {
        // handle show peers.
        if (this->formatCheck(split, 1))
        {
            ulog << "Ack" << endl;
            for (auto a : this->sockToAddr)
            {
                int id = this->socketToId[a.first];
                ulog << a.second.ip << ":" << a.second.port << " , NODE ID : " << id << endl;
            }
        }
    }
    else if (split[0] == "sleep")
    {
        // i.e if we want to write an automatic script,
        // we would sometimes need to wait.
        sleep(1);
    }
    else if (split[0] == "exit")
    {
        // a command to close server in a "good" maner.
        this->close_server();
        return false;
    }
    else
    {
        // comman is not supported ( yet or at all ).
        ulog << "Nack : Invalid input!" << endl;
    }
    this->plog(this->ulog);
    return true;
}

void Node::userThread()
{
    // start a thread exclusively for listening for user input.
    cout << "listening to stdin..." << endl;
    while (this->running)
    {
        try
        {
            if (this->handleUserInput())
            {
                sleep(0.1);
            }
            else
            {
                break;
            }
        }
        catch (exception e)
        {
            cout << "ops something went wrong on user input..." << endl;
            // do nothing...
        }
    }
    cout << "exiting user thread" << endl;
}

void Node::plog(stringstream &log)
{
    // Log out in a compact manner all messages accumulated in the stringstrean
    cout << "----------------- LOG -----------------" << endl;
    cout << log.str();
    log.str(string{});
    cout << "-------------------------------------" << endl;
}