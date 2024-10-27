#include "MSTree.hpp"
#include <iostream>
#include <limits>
#include <sstream>
#include <unistd.h>

using namespace std;

MSTree::MSTree() : totalWeight_(0) {}

void MSTree::addEdge(const Edge &edge)
{
    mstEdges_.push_back(edge);
    totalWeight_ += edge.weight_;

    // Ensure the adjacency list is correctly updated for both directions (undirected graph)
    adjList[edge.v1_].push_back({edge.v2_, edge.weight_});
    adjList[edge.v2_].push_back({edge.v1_, edge.weight_});
}


void MSTree::printMST(int fd)
{
    if (fd == -1)
    {
        cout << "MST Edges:\n";
        for (const auto &edge : mstEdges_)
        {
            cout << "Edge (" << edge.v1_ << ", " << edge.v2_ << ") -> Weight: " << edge.weight_ << endl;
        }
    }
    else
    {
        ostringstream oss;
        oss << "MST Edges:\n";
        for (const auto &edge : mstEdges_)
        {
            oss << "Edge (" << edge.v1_ << ", " << edge.v2_ << ") -> Weight: " << edge.weight_ << endl;
        }
        string output = oss.str();
        write(fd, output.c_str(), output.size());
    }
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
double MSTree::getTotalWeight(){
    return totalWeight_;
}
// Find the shortest distance between all pairs of vertices
double MSTree::findShortestDistance()
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
    return shortestDistance;
}

// Helper function to perform DFS and return the farthest node and its distance
pair<int, double> MSTree::dfs(int node, int parent, vector<bool> &visited)
{
    visited[node] = true;
    pair<int, double> farthest = {node, 0}; // {farthest node, distance}

    for (const auto &neighbor : adjList[node])
    {
        int nextNode = neighbor.first;
        double weight = neighbor.second;

        if (nextNode != parent)
        {
            auto result = dfs(nextNode, node, visited);
            result.second += weight; // Accumulate the distance

            if (result.second > farthest.second)
            {
                farthest = result;
            }
        }
    }

    return farthest;
}

double MSTree::findLongestDistance()
{
    double ans;
    // Step 1: Perform DFS from any node (say node 0) to find the farthest node
    vector<bool> visited(numVertices_, false);
    auto farthestFromStart = dfs(0, -1, visited);

    // Step 2: Reset the visited vector and perform DFS from the farthest node found
    fill(visited.begin(), visited.end(), false);
    auto farthest = dfs(farthestFromStart.first, -1, visited);

    // The second DFS gives the longest distance
    ans = farthest.second;

    //cout << "Longest Distance: " << ans << endl;
    return ans;
}

// Find the average distance between all pairs of vertices
double MSTree::findAverageDistance()
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
    return ans;
}
