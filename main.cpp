#include "Graph.hpp"
#include "MSTStrategy.hpp"
#include "MSTree.hpp"
#include <iostream>
#include <memory>
using namespace std;
int main() {
    // Create a graph with 5 vertices
    Graph graph(5);
    
    // Add edges (undirected graph)
    graph.addEdge(0, 1, 10);  // Edge from vertex 0 to 1 with weight 10
    graph.addEdge(0, 2, 5);   // Edge from vertex 0 to 2 with weight 5
    graph.addEdge(1, 2, 7);   // Edge from vertex 1 to 2 with weight 7
    graph.addEdge(1, 3, 8);   // Edge from vertex 1 to 3 with weight 8
    graph.addEdge(2, 3, 6);   // Edge from vertex 2 to 3 with weight 6
    graph.addEdge(3, 4, 9);   // Edge from vertex 3 to 4 with weight 9

    // Display the original graph
    cout << "Original Graph Edges:\n";
    graph.printGraph(); // Assuming Graph class has a method to print all edges

    // Create a factory to obtain MST algorithms
    MSTFactory factory;

    // Test Prim's Algorithm
    cout << "\n--- Prim's Algorithm ---\n";
    unique_ptr<MSTStrategy> primStrategy = factory.getMSTStrategy(MSTFactory::PRIM);
    MSTree primMST = primStrategy->computeMST(graph);
    primMST.printMST(); // Assuming MSTree class has a method to print the MST

    // Test Kruskal's Algorithm
    cout << "\n--- Kruskal's Algorithm ---\n";
    unique_ptr<MSTStrategy> kruskalStrategy = factory.getMSTStrategy(MSTFactory::KRUSKAL);
    MSTree kruskalMST = kruskalStrategy->computeMST(graph);
    kruskalMST.printMST(); // Assuming MSTree class has a method to print the MST

    return 0;
}
