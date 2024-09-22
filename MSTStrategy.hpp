#ifndef MSTSTRATEGY_HPP
#define MSTSTRATEGY_HPP

#include "Graph.hpp"
#include "MSTree.hpp"
#include "union_find.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
#include <set>
#include <memory>

constexpr int INF = 0x3f3f3f3f;

// Abstract base class for MST algorithms
class MSTStrategy
{
public:
    virtual MSTree computeMST(const Graph &graph) = 0;
};

// Prim's Algorithm implementation
class PrimMST : public MSTStrategy
{
public:

// Local Edge structure for Prim's algorithm
struct PrimEdge {
    int w = INF, to = -1, id;
    bool operator<(PrimEdge const& other) const {
        return std::make_pair(w, to) < std::make_pair(other.w, other.to);
    }
    PrimEdge() = default;
    PrimEdge(int _w, int _to, int _id) : w(_w), to(_to), id(_id) {}
};

    MSTree computeMST(const Graph &graph) override;
};

// Kruskal's Algorithm implementation
class KruskalMST : public MSTStrategy
{
public:
    MSTree computeMST(const Graph &graph);
};
// Factory for creating MST strategy objects
class MSTFactory
{
public:
    enum MSTType
    {
        PRIM,
        KRUSKAL
    };

    std::unique_ptr<MSTStrategy> getMSTStrategy(MSTType type);
};

#endif // MSTSTRATEGY_HPP
