//
// Created by mho on 10/28/19.
//

#pragma once

#include <list>
#include <algorithm>
#include <vector>
#include <sstream>

#include <fmt/format.h>

#include "IndexPersistentVector.h"

namespace graphs {

template<template<typename...> class VertexCollection, typename Vertex, typename... Rest>
class Graph {
public:
    using VertexList = VertexCollection<Vertex, Rest...>;
    using VertexIndex = typename VertexList::size_type;

    using Edge = std::tuple<VertexIndex, VertexIndex>;
    using Path2 = Edge;
    using Path3 = std::tuple<VertexIndex, VertexIndex, VertexIndex>;
    using Path4 = std::tuple<VertexIndex, VertexIndex, VertexIndex, VertexIndex>;

    using iterator = typename VertexList::active_iterator;
    using const_iterator = typename VertexList::const_active_iterator;

    Graph();

    explicit Graph(VertexList vertexList);

    virtual ~Graph();

    Graph(const Graph &) = default;

    Graph &operator=(const Graph &) = default;

    Graph(Graph &&) = default;

    Graph &operator=(Graph &&) = default;

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

    const VertexList &vertices() const;

    VertexList &vertices();

    typename VertexList::size_type nVertices() const;

    bool containsEdge(const Edge &edge) const;

    bool containsEdge(VertexIndex v1, VertexIndex v2) const;

    iterator addVertex(typename Vertex::data_type data = {});

    template<auto debug = nullptr>
    void addEdge(iterator it1, iterator it2);

    template<auto debug = nullptr>
    void addEdge(VertexIndex ix1, VertexIndex ix2);

    template<auto debug = nullptr>
    void addEdge(const Edge &edge);

    void removeEdge(iterator it1, iterator it2);

    void removeEdge(VertexIndex ix1, VertexIndex ix2);

    void removeEdge(const Edge &edge);

    void removeVertex(iterator it);

    void removeVertex(VertexIndex ix);

    bool isConnected() const;

    const std::vector<Edge> &edges() const;

    std::size_t nEdges() const;

    template<typename TupleCallback>
    void findEdges(const TupleCallback &edgeCallback) const;

    template<typename TupleCallback, typename TripleCallback, typename QuadrupleCallback>
    void findNTuples(const TupleCallback &pairCallback,
                     const TripleCallback &tripleCallback,
                     const QuadrupleCallback &quadrupleCallback);

    std::tuple<std::vector<Edge>, std::vector<Path3>, std::vector<Path4>> findNTuples();

    /**
     * Returns the connected components in terms of a list of new graph objects
     * @return connected components
     */
    std::vector<Graph> connectedComponents();

    /**
     * Appends the graph `other` to this graph. No edge is introduced, this graph will have at least two connected
     * components afterwards. Returns an index mapping for the `other` graph which (for the active vertices) contains
     * the index of the vertex in this graph, i.e., `newIndex = mapping[oldIndex]`.
     * @param other the other graph
     * @return index mapping
     */
    std::vector<VertexIndex> append(const Graph &other);

    /**
     * Appends other graph to this one and introduces an edge connecting the two.
     * @param other other graph
     * @param edgeIndexThis index pointing to a live vertex in this graph
     * @param edgeIndexOther index pointing to a live vertex in the other graph
     * @return index mapping
     */
    std::vector<VertexIndex> append(const Graph &other, VertexIndex edgeIndexThis, VertexIndex edgeIndexOther);

    std::string gexf() const;

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
