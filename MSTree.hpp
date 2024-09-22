#ifndef MSTREE_HPP
#define MSTREE_HPP

#include "Graph.hpp"
#include <vector>

class MSTree {
public:
    std::vector<Edge> mstEdges_; // Edges in the MST
    double totalWeight_; // Total weight of the MST
    double longestDistance_;// Lomgest distance between two vertices
    double averageDistance_;// Average distance between every two edges in the graph
    double shortestDistance_;// Shortest distance between 2 vertices Xi, Xj where i!=j and edge belongs to MST


    MSTree();
    void addEdge(const Edge& edge);
    void printMST();
    void findLongestDistance();
    void findAverageDistance();
    void findShortestDistance();
};

#endif // MSTREE_HPP
