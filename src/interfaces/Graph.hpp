#pragma once

class Graph
{
public:
    virtual void AddNode(int nodeID) = 0;
    virtual void AddEdge(int node1, int node2) = 0;
    virtual void RemoveEdge(int node1, int node2) = 0;
    virtual void RemoveNode(int nodeID) = 0;
};