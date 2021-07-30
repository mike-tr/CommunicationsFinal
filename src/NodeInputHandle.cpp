#include "headers/Node.hpp"
#include <unistd.h>
#include <iostream>
#include <unistd.h>

using namespace std;

void Node::handleNodeInput(int socket)
{
    memset(this->buff, 0, this->buff_size);
    this->msg_size = read(socket, buff, buff_size);
    if (this->msg_size == 0)
    {
        cout << "disconnected..." << endl;
        this->listner.remove_descriptor(socket);
        this->connections.erase(socket);

        int v = this->socketToId[socket];
        if (v != 0)
        {
            this->socketToId.erase(socket);
            this->idToSocket.erase(v);
        }
    }
    else
    {
        // cout << "message from server size is : " << size << endl;
        // cout << buff << endl;
        try
        {
            NodeMessage *ms = (NodeMessage *)buff;
            cout << "message from server size is : " << this->msg_size << endl;
            cout << ms->to_string() << endl;
        }
        catch (exception exp)
        {
            cout << "Got wrong message format of size : " << this->msg_size << endl;
        }
        //printf("%s", buff);
    }
}

bool Node::handleUserInput()
{
    memset(this->buff, 0, this->buff_size);
    //this->msg_size = read(0, buff, buff_size);
    if (fgets(buff, buff_size, stdin))
    {
        cout << buff << endl;
        //sleep(1);
    }
    else
    {
        cout << "EOF!" << endl;
        return false;
    }
    cout << "my id : " << this->node_id << endl;
    // USER INPUT
    auto split = Utilities::splitBy(buff, ',');
    //
    //cout << split[0] << endl;
    if (this->node_id == -1)
    {
        if (split[0] != "setid")
        {
            cout << "setid first!" << endl;
            cout << "nack" << endl;
        }
        else if (this->formatCheck(split, 2))
        {
            this->setid(split[1]);
        }
    }
    else if (split[0] == "connect")
    {
        if (this->formatCheck(split, 2))
        {
            this->connect_tcp(split[1]);
        }
    }
    else if (split[0] == "peers")
    {
        if (this->formatCheck(split, 1))
        {
            cout << "printing ips..." << endl;
            for (auto a : this->connections)
            {
                cout << a.second.ip << " " << a.second.port << endl;
            }
        }
    }
    else
    {
        cout << "Nack : Invalid input!" << endl;
    }
    return true;
}

void Node::userThread()
{
    //cout << fseek(stdin, 0, SEEK_END) << " " << ftell(stdin) << endl;
    cout << "listening to stdin..." << endl;
    while (this->running && this->handleUserInput())
    {
        sleep(1);
    }
    cout << "stdin input is empty... closing thread" << endl;
}