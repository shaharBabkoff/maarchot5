#include "MSTree.hpp"
#include <iostream>
#include <limits>
using namespace std;
MSTree::MSTree() : totalWeight_(0) {}

void MSTree::addEdge(const Edge &edge) {
    mstEdges_.push_back(edge);
    totalWeight_ += edge.weight_;

    // Ensure the adjacency list is correctly updated for both directions (undirected graph)
    adjList[edge.v1_].push_back({edge.v2_, edge.weight_});
    adjList[edge.v2_].push_back({edge.v1_, edge.weight_});
}
void MSTree::removeEdge(int v1,int v2){
    // Find and remove the edge in mstEdges_
    for (auto it = mstEdges_.begin(); it != mstEdges_.end(); ++it) {
        if ((it->v1_ == v1 && it->v2_ == v2) || (it->v1_ == v2 && it->v2_ == v1)) {
            totalWeight_ -= it->weight_;  // Subtract the weight from totalWeight_
            mstEdges_.erase(it);          // Remove the edge from mstEdges_
            break;
        }
    }

    // Remove the edge from the adjacency list for both directions
    auto removeEdgeFromAdjList = [](vector<vector<pair<int, double>>>& adjList, int v1, int v2) {
        adjList[v1].erase(remove_if(adjList[v1].begin(), adjList[v1].end(),
                                         [v2](const pair<int, double>& edge) {
                                             return edge.first == v2;
                                         }),
                          adjList[v1].end());
    };

    // Remove from both directions since it's undirected
    removeEdgeFromAdjList(adjList, v1, v2);
    removeEdgeFromAdjList(adjList, v2, v1);
}



void MSTree::printMST()
{
    cout << "MST Edges:\n";
    for (const auto &edge : mstEdges_)
    {
        cout << "Edge (" << edge.v1_ << ", " << edge.v2_ << ") -> Weight: " << edge.weight_ << endl;
    }
    cout << "Total MST Weight: " << totalWeight_ << endl;
}

vector<double> MSTree::bfs(int start)
{
    vector<double> distances(numVertices_, numeric_limits<double>::max());
    queue<int> q;
    distances[start] = 0;
    q.push(start);

    while (!q.empty())
    {
        int node = q.front();
        q.pop();

        for (auto &neighbor : adjList[node])
        {
            int nextNode = neighbor.first;
            double weight = neighbor.second;

            // Update distance if a shorter path is found
            if (distances[node] + weight < distances[nextNode])
            {
                distances[nextNode] = distances[node] + weight;
                q.push(nextNode);
            }
        }
    }

    return distances;
}

// Find the shortest distance between all pairs of vertices
void MSTree::findShortestDistance()
{
    double shortestDistance = numeric_limits<double>::max();
    for (int i = 0; i < numVertices_; ++i)
    {
        vector<double> distances = bfs(i);
        for (int j = 0; j < numVertices_; ++j)
        {
            if (i != j && distances[j] < shortestDistance)
            {
                shortestDistance = distances[j];
            }
        }
    }
    cout << "Shortest Distance: " << shortestDistance << endl;
}

// Helper function to perform DFS and return the farthest node and its distance
pair<int, double> MSTree::dfs(int node, int parent, vector<bool>& visited) {
    visited[node] = true;
    pair<int, double> farthest = {node, 0};  // {farthest node, distance}

    for (const auto& neighbor : adjList[node]) {
        int nextNode = neighbor.first;
        double weight = neighbor.second;

        if (nextNode != parent) {
            auto result = dfs(nextNode, node, visited);
            result.second += weight;  // Accumulate the distance
            
            if (result.second > farthest.second) {
                farthest = result;
            }
        }
    }

    return farthest;
}

void MSTree::findLongestDistance() {
    double ans;
    // Step 1: Perform DFS from any node (say node 0) to find the farthest node
    vector<bool> visited(numVertices_, false);
    auto farthestFromStart = dfs(0, -1, visited);

    // Step 2: Reset the visited vector and perform DFS from the farthest node found
    fill(visited.begin(), visited.end(), false);
    auto farthest = dfs(farthestFromStart.first, -1, visited);

    // The second DFS gives the longest distance
    ans = farthest.second;

    cout << "Longest Distance: " << ans << endl;
}

// Find the average distance between all pairs of vertices
void MSTree::findAverageDistance()
{
    double ans = 0;
    double totalDistance = 0;
    int count = 0;

    for (int i = 0; i < numVertices_; ++i)
    {
        vector<double> distances = bfs(i);

        for (int j = i + 1; j < numVertices_; ++j)
        { // Avoid double-counting
            if (distances[j] < numeric_limits<double>::max())
            { // Only consider valid paths
                totalDistance += distances[j];
                ++count;
            }
        }
    }

    if (count > 0)
    {
        ans = totalDistance / count;
    }
    else
    {
        ans = 0; // Handle the case where no valid distances were found
    }

    cout << "Average Distance: " << ans << endl;
}
