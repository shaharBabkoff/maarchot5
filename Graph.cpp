#include "Graph.hpp"
#include <iostream>
#include <sstream>
#include <unistd.h>
using namespace std;
Edge::Edge(int u, int v, double weight) : v1_(u), v2_(v), weight_(weight) {}

Graph::Graph(int numVertices) : numVertices_(numVertices) {}

void Graph::addEdge(int u, int v, double weight)
{
    edges_.emplace_back(u, v, weight);
}
void Graph::removeEdge(int v1, int v2)
{
    // Remove the edge from the list of edges
    for (auto it = edges_.begin(); it != edges_.end(); ++it)
    {
        if ((it->v1_ == v1 && it->v2_ == v2) || (it->v1_ == v2 && it->v2_ == v1))
        {
            edges_.erase(it);
            break;
        }
    }
}
void Graph::printGraph(int fd)
{
    for (const auto &edge : edges_)
    {
        if (fd == -1)
        {
            cout << "Edge (" << edge.v1_ << ", " << edge.v2_ << ") -> Weight: " << edge.weight_ << endl;
        }
        else
        {
            ostringstream oss;
            oss << "Edge (" << edge.v1_ << ", " << edge.v2_ << ") -> Weight: " << edge.weight_ << endl;
            string output = oss.str();
            write(fd, output.c_str(), output.size());
        }
    }
}
