#include "MSTree.hpp"
#include <iostream>

MSTree::MSTree() : totalWeight_(0) {}

void MSTree::addEdge(const Edge& edge) {
    mstEdges_.push_back(edge);
    totalWeight_ += edge.weight_;
}

void MSTree::printMST() {
    std::cout << "MST Edges:\n";
    for (const auto& edge : mstEdges_) {
        std::cout << "Edge (" << edge.v1_ << ", " << edge.v2_ << ") -> Weight: " << edge.weight_ << std::endl;
    }
    std::cout << "Total MST Weight: " << totalWeight_ << std::endl;
}
void MSTree::findAverageDistance(){
    int numOfEdges =0;
for (const auto& edge : mstEdges_) {
        numOfEdges++;
     }
}
void MSTree::findLongestDistance(){

}
void MSTree::findShortestDistance(){
    double shortestDist=INT8_MAX;
     for (const auto& edge : mstEdges_) {
        if(edge.weight_<shortestDist){
            shortestDist=edge.weight_;
        }
     }
     this->shortestDistance_=shortestDist;
}
