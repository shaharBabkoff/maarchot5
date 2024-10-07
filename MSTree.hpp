#ifndef MSTREE_HPP
#define MSTREE_HPP

#include "Graph.hpp"
#include <vector>
#include <queue>
#include <algorithm>

class MSTree
{
public:
    std::vector<Edge> mstEdges_; // Edges in the MST
    double totalWeight_;         // Total weight of the MST
    // double longestDistance_;     // Lomgest distance between two vertices
    // double averageDistance_;     // Average distance between every two edges in the graph
    int numVertices_;
    std::vector<std::vector<std::pair<int, double>>> adjList; // Adjacency list with weights
    std::vector<double> bfs(int start);
    std::pair<int, double> dfs(int node, int parent, std::vector<bool>& visited) ;
    MSTree() ;
    MSTree(int numVertices) : totalWeight_(0), numVertices_(numVertices)
    {
        adjList.resize(numVertices_); // Initialize adjList with the number of vertices
    }
    void addEdge(const Edge &edge);
    void removeEdge(int v1, int v2);
    void printMST();
    void findLongestDistance();
    void findAverageDistance();
    void findShortestDistance();
};

#endif // MSTREE_HPP
