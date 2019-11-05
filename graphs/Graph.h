//
// Created by mho on 10/28/19.
//

#pragma once

#include <list>
#include <algorithm>
#include <vector>

#include <fmt/format.h>

#include "index_persistent_vector.h"

namespace graphs {

template<typename Vertex>
class Graph {
public:
    using VertexList = graphs::index_persistent_vector<Vertex>;
    using VertexIndex = typename VertexList::size_type;

    using Edge = std::tuple<VertexIndex, VertexIndex>;
    using Path2 = Edge;
    using Path3 = std::tuple<VertexIndex, VertexIndex, VertexIndex>;
    using Path4 = std::tuple<VertexIndex, VertexIndex, VertexIndex, VertexIndex>;

    Graph();

    explicit Graph(VertexList vertexList);

    virtual ~Graph();

    Graph(const Graph &) = delete;

    Graph &operator=(const Graph &) = delete;

    Graph(Graph &&) = default;

    Graph &operator=(Graph &&) = default;

    const VertexList &vertices() const;

    VertexList &vertices();

    bool containsEdge(const Edge &edge) const;

    bool containsEdge(VertexIndex v1, VertexIndex v2) const;

    VertexIndex addVertex(typename Vertex::data_type data = {});

    template<auto debug = nullptr>
    void addEdge(VertexIndex ix1, VertexIndex ix2);

    template<auto debug = nullptr>
    void addEdge(const Edge &edge);

    void removeEdge(VertexIndex ix1, VertexIndex ix2);

    void removeEdge(const Edge &edge);

    void removeVertex(VertexIndex ix);

    bool isConnected() const;

    const std::vector<Edge> &edges() const;

    std::size_t nEdges() const;

    template<typename TupleCallback>
    void findEdges(const TupleCallback &edgeCallback) const;

    template<typename TupleCallback, typename TripleCallback, typename QuadrupleCallback>
    void findNTuples(const TupleCallback &tuple_callback,
                     const TripleCallback &triple_callback,
                     const QuadrupleCallback &quadruple_callback);

    std::tuple<std::vector<Edge>, std::vector<Path3>, std::vector<Path4>> findNTuples();

    /**
     * Returns the connected components, invalidates this graph
     * @return connected components
     */
    std::vector<Graph<Vertex>> connectedComponentsDestructive();

    void concatenate(Graph &other);

private:
    VertexList _vertices{};
    std::vector<Edge> _edges {};

    void removeNeighborsEdges(VertexIndex ix);

    /**
     * this has always to be called for both v1 and v2 (symmetric neighborship)
     * @tparam debug
     * @param v1
     * @param v2
     */
    template<auto debug = nullptr>
    void addVertexNeighbor(Vertex& v1, VertexIndex v2);

    /**
     * this has always to be called for both v1 and v2 (symmetric neighborship)
     * @tparam debug
     * @param v1
     * @param v2
     */
    template<auto debug = nullptr>
    void removeVertexNeighbor(Vertex& v1, VertexIndex v2);

};

}

#include "bits/Graph_detail.h"
