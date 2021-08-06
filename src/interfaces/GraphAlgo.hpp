#pragma once
#include "Graph.hpp"
#include <vector>

class GraphAlgo {
public:
    // set new graph
    virtual void init(Graph graoh) = 0;
    // return the graph we are working on
    virtual Graph getGraph() = 0;
    // shortest path with the lowest lexicographic order
    virtual std::vector<int> shortestPath();
};