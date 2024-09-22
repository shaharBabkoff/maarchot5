#include "MSTStrategy.hpp"


using namespace std;
// Implement Prim's MST Algorithm
MSTree PrimMST::computeMST(const Graph &graph)
{

    int n = graph.numVertices_;                // Number of vertices
    std::vector<std::vector<PrimEdge>> adj(n); // Adjacency list

    // Convert the edges from the Graph object to an adjacency list
    for (const auto &edge : graph.edges_)
    {
        adj[edge.v1_].push_back(PrimEdge(edge.weight_, edge.v2_, edge.v1_));
        adj[edge.v2_].push_back(PrimEdge(edge.weight_, edge.v1_, edge.v2_)); // Because the graph is undirected
    }

    MSTree mst; // To store the resulting MST
    std::vector<PrimEdge> min_e(n);
    min_e[0].w = 0; // Start with vertex 0
    set<PrimEdge> q;
    q.insert({0, 0, -1}); // Starting from node 0

    std::vector<bool> selected(n, false);

    for (int i = 0; i < n; ++i)
    {
        if (q.empty())
            break; // In case the graph is disconnected

        int v = q.begin()->to; // Select the vertex with the smallest edge weight
        selected[v] = true;
        q.erase(q.begin()); // Remove this vertex from the set

        if (min_e[v].to != -1)
        {
            mst.addEdge({min_e[v].to, v, min_e[v].w}); // Add the edge to the MST
        }

        // Explore the adjacent vertices
        for (const PrimEdge &e : adj[v])
        {
            if (!selected[e.to] && e.w < min_e[e.to].w)
            {
                q.erase({min_e[e.to].w, e.to, min_e[e.to].id});
                min_e[e.to] = {e.w, v, e.id};
                q.insert({e.w, e.to, e.id});
            }
        }
    }

    return mst; // Return the MST result
}

// Implement Kruskal's MST Algorithm
MSTree KruskalMST::computeMST(const Graph &graph)
{
    // Kruskal's algorithm logic here

    UnionFind uf(graph.numVertices_);  // Union-Find initialized with number of vertices
    vector<Edge> edges = graph.edges_; // Get all edges from the graph
    MSTree mst;                        // To store the resulting MST

    // Sort edges by weight (cost) in ascending order
    sort(edges.begin(), edges.end(), [](const Edge &a, const Edge &b)
         { return a.weight_ < b.weight_; });

    // Iterate through sorted edges and add to MST if no cycle is formed
    for (const auto &edge : edges)
    {
        int v1 = edge.v1_;
        int v2 = edge.v2_;

        // Use Union-Find to check if u and v are already connected
        if (uf.unite(v1, v2))
        {
            // If they are not connected, add the edge to the MST
            mst.addEdge(edge);
        }
    }

    return mst; // Return the resulting MST
}

// Factory method to create the correct MST strategy based on the type
unique_ptr<MSTStrategy> MSTFactory::getMSTStrategy(MSTType type)
{
    switch (type)
    {
    case PRIM:
        return std::make_unique<PrimMST>();
    case KRUSKAL:
        return std::make_unique<KruskalMST>();
    default:
        return nullptr;
    }
}

