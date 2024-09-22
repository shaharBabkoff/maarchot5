#include "Graph.hpp"
#include <iostream>

Edge::Edge(int u, int v, double weight) : v1_(u), v2_(v), weight_(weight) {}

Graph::Graph(int numVertices) : numVertices_(numVertices) {}

void Graph::addEdge(int u, int v, double weight) {
    edges_.emplace_back(u, v, weight);
}

void Graph::printGraph() {
    for (const auto& edge : edges_) {
        std::cout << "Edge (" << edge.v1_ << ", " << edge.v2_ << ") -> Weight: " << edge.weight_ << std::endl;
    }
}
