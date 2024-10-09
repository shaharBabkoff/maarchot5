#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>

struct Edge {
    int v1_, v2_;
    double weight_;
    Edge(int v1, int v2, double weight);
};

class Graph {
public:
    int numVertices_; // Number of vertices
    std::vector<Edge> edges_; // List of all edges in the graph

    Graph(int vertices);
    void addEdge(int v1, int v2, double weight);
    void removeEdge(int v1, int v2);
    void printGraph(int fd);
};

#endif // GRAPH_HPP
