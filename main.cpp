#include "Graph.hpp"
#include "MSTStrategy.hpp"
#include "MSTree.hpp"
#include <iostream>
#include <memory>
using namespace std;
int main()
{
  // Create a graph with 5 vertices
  Graph graph(5);

  // Add edges (undirected graph)
  graph.addEdge(0, 1, 10); // Edge from vertex 0 to 1 with weight 10
  graph.addEdge(0, 2, 5);  // Edge from vertex 0 to 2 with weight 5
  graph.addEdge(1, 2, 7);  // Edge from vertex 1 to 2 with weight 7
  graph.addEdge(1, 3, 8);  // Edge from vertex 1 to 3 with weight 8
  graph.addEdge(2, 3, 6);  // Edge from vertex 2 to 3 with weight 6
  graph.addEdge(3, 4, 9);  // Edge from vertex 3 to 4 with weight 9

  // Display the original graph
  cout << "Original Graph Edges:\n";
  graph.printGraph(); // Assuming Graph class has a method to print all edges

  // Create a factory to obtain MST algorithms
  MSTFactory factory;

  // Test Prim's Algorithm
  cout << "\n--- Prim's Algorithm ---\n";
  unique_ptr<MSTStrategy> primStrategy = factory.getMSTStrategy(MSTFactory::PRIM);
  MSTree primMST(graph.numVertices_);
  primMST = primStrategy->computeMST(graph);
  primMST.printMST(); // Assuming MSTree class has a method to print the MST

  // Test Kruskal's Algorithm
  cout << "\n--- Kruskal's Algorithm ---\n";
  unique_ptr<MSTStrategy> kruskalStrategy = factory.getMSTStrategy(MSTFactory::KRUSKAL);
  MSTree kruskalMST = kruskalStrategy->computeMST(graph);
  kruskalMST.printMST(); // Assuming MSTree class has a method to print the MST
  primMST.findLongestDistance();
  primMST.findAverageDistance();
  primMST.findShortestDistance();
  kruskalMST.findLongestDistance();
  kruskalMST.findAverageDistance();
  kruskalMST.findShortestDistance();
  Graph graph2(5);
  graph2.addEdge(0,1,8);
  graph2.addEdge(1,2,3);
  graph2.addEdge(2,3,4);
  graph2.addEdge(3,4,12);
  graph2.addEdge(4,0,1);
  graph2.addEdge(0,2,8);
  graph2.addEdge(0,3,20);
  graph2.addEdge(1,4,6);
  graph2.addEdge(1,3,18);
  graph2.addEdge(2,4,9);
  graph2.addEdge(0,1,8);
   // Display the original graph
  cout << "Original Graph2 Edges:\n";
  graph2.printGraph(); // Assuming Graph class has a method to print all edges

  // Create a factory to obtain MST algorithms
  MSTFactory factory2;

  // Test Prim's Algorithm
  cout << "\n--- Prim's Algorithm ---\n";
  unique_ptr<MSTStrategy> primStrategy2 = factory2.getMSTStrategy(MSTFactory::PRIM);
  MSTree primMST2(graph2.numVertices_);
  primMST2 = primStrategy2->computeMST(graph2);
  primMST2.printMST(); // Assuming MSTree class has a method to print the MST

  // Test Kruskal's Algorithm
  cout << "\n--- Kruskal's Algorithm ---\n";
  unique_ptr<MSTStrategy> kruskalStrategy2 = factory2.getMSTStrategy(MSTFactory::KRUSKAL);
  MSTree kruskalMST2 = kruskalStrategy2->computeMST(graph2);
  kruskalMST2.printMST(); // Assuming MSTree class has a method to print the MST
  primMST2.findLongestDistance();
  primMST2.findAverageDistance();
  primMST2.findShortestDistance();
  kruskalMST2.findLongestDistance();
  kruskalMST2.findAverageDistance();
  kruskalMST2.findShortestDistance();

//---------------------------------removeEdgeTest-----------------------------------------------------
Graph graph3(5);
    graph3.addEdge(0, 1, 10);
    graph3.addEdge(0, 2, 5);
    graph3.addEdge(1, 2, 7);
    graph3.addEdge(1, 3, 8);
    graph3.addEdge(2, 3, 6);
    graph3.addEdge(3, 4, 9);

    // Display the original graph
    cout << "Original Graph Edges:\n";
    graph3.printGraph();

    // Step 2: Use MSTFactory to compute MST using Prim's algorithm
    MSTFactory factory3;
    unique_ptr<MSTStrategy> primStrategy3 = factory3.getMSTStrategy(MSTFactory::PRIM);
    MSTree mst3 = primStrategy3->computeMST(graph3);

    // Print the MST before removing an edge
    cout << "\nMST Edges Before Removal:\n";
    mst3.printMST();

    // Step 3: Remove an edge and test
    cout << "\nRemoving edge (2, 3) from MST...\n";
    mst3.removeEdge(2, 3);

    // Print the MST after removing the edge to see if it has been removed correctly
    cout << "\nMST Edges After Removal of (2, 3):\n";
    mst3.printMST();

}
