//
// Created by mho on 10/28/19.
//

#pragma once

#include <list>
#include <algorithm>
#include <vector>

#include <fmt/format.h>

namespace graphs {

template<typename Vertex>
class Graph {
public:

    using VertexList = std::list<Vertex>;
    using VertexPtr = typename VertexList::iterator;

    using Edge = std::tuple<VertexPtr, VertexPtr>;
    using Path2 = std::tuple<VertexPtr, VertexPtr, VertexPtr>;
    using Path3 = std::tuple<VertexPtr, VertexPtr, VertexPtr, VertexPtr>;

    Graph();

    explicit Graph(VertexList vertexList);

    virtual ~Graph();

    Graph(const Graph &) = delete;

    Graph &operator=(const Graph &) = delete;

    Graph(Graph &&) = default;

    Graph &operator=(Graph &&) = default;

    const VertexList &vertices() const;

    VertexList &vertices();

    VertexPtr firstVertex();

    VertexPtr lastVertex();

    VertexPtr toRef(const Vertex &v);

    bool containsEdge(const Edge &edge) const;

    bool containsEdge(VertexPtr v1, VertexPtr v2) const;

    const Vertex &vertexForData(const typename Vertex::data_type &data) const;

    void addVertex(typename Vertex::data_type data);

    template<auto debug = nullptr>
    void addVertexNeighbor(Vertex& v1, const VertexPtr &v2);

    template<auto debug = nullptr>
    void removeVertexNeighbor(Vertex& v1, const VertexPtr &v2);

    template<auto debug = nullptr>
    void addEdge(VertexPtr v1, VertexPtr v2);

    template<auto debug = nullptr>
    void addEdge(const Edge &edge);

    void addEdgeBetweenParticles(std::size_t particleIndex1, std::size_t particleIndex2);

    void removeEdge(VertexPtr v1, VertexPtr v2);

    void removeEdge(const Edge &edge);

    void removeVertex(VertexPtr vertex);

    void removeParticle(std::size_t particleIndex);

    bool isConnected() const;

    const std::vector<Edge> &edges() const;

    bool hasEdge(const Edge &edge) const;

    std::size_t nEdges() const;

    template<typename TupleCallback>
    void findEdges(const TupleCallback &edgeCallback) const;

    template<typename TupleCallback, typename TripleCallback, typename QuadrupleCallback>
    void findNTuples(const TupleCallback &tuple_callback,
                     const TripleCallback &triple_callback,
                     const QuadrupleCallback &quadruple_callback);

    std::tuple<std::vector<Edge>, std::vector<Path2>, std::vector<Path3>> findNTuples();

    /**
     * Returns the connected components, invalidates this graph
     * @return connected components
     */
    std::vector<Graph<Vertex>> connectedComponentsDestructive();

    void concatenate(Graph &other);

private:
    VertexList _vertices{};

    mutable bool _dirty {true};
    mutable std::vector<Edge> _edges;

    void resetVertexVisitedState() const;

    void removeNeighborsEdges(VertexPtr vertex);

    VertexPtr vertexItForData(const typename Vertex::data_type &data);

};

}

#include "bits/Graph_detail.h"
