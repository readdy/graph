//
// Created by mho on 10/28/19.
//

#pragma once

#include <list>
#include <algorithm>

#include <fmt/format.h>

#include "Vertex.h"

using ParticleTypeId = std::size_t;

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

    const Vertex &vertexForParticleIndex(std::size_t particleIndex) const;

    void addVertex(std::size_t particleIndex, ParticleTypeId particleType);

    void addVertexNeighbor(Vertex& v1, const VertexPtr &v2) {
        _dirty = true;
        if (std::find(v1.neighbors_.begin(), v1.neighbors_.end(), v2) == v1.neighbors_.end()) {
            v1.neighbors_.push_back(v2);
        } else {
            //log::debug("tried to add an already existing edge ({} - {})", v1.particleIndex, v2->particleIndex);
            // todo
        }
    }

    void removeVertexNeighbor(Vertex& v1, const VertexPtr &v2) {
        _dirty = true;
        decltype(v1.neighbors_.begin()) it;
        if ((it = std::find(v1.neighbors_.begin(), v1.neighbors_.end(), v2)) != v1.neighbors_.end()) {
            v1.neighbors_.erase(it);
        } else {
            //log::debug("tried to remove a non existing edge {} - {}", v1.particleIndex, v2->particleIndex);
            // todo
        }
    }

    void addEdge(VertexPtr v1, VertexPtr v2);

    void addEdge(const Edge &edge);

    void addEdgeBetweenParticles(std::size_t particleIndex1, std::size_t particleIndex2);

    void removeEdge(VertexPtr v1, VertexPtr v2);

    void removeEdge(const Edge &edge);

    void removeVertex(VertexPtr vertex);

    void removeParticle(std::size_t particleIndex);

    bool isConnected() const;

    const std::vector<Edge> &edges() const;

    bool hasEdge(const Edge &edge) const;

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
    std::vector<Graph> connectedComponentsDestructive();

private:
    VertexList _vertices{};

    mutable bool _dirty {true};
    mutable std::vector<Edge> _edges;

    void removeNeighborsEdges(VertexPtr vertex) {
        std::for_each(std::begin(vertex->neighbors()), std::end(vertex->neighbors()), [this, vertex](const auto neighbor) {
            removeVertexNeighbor(*neighbor, vertex);
        });
        _dirty = true;
    }

    auto vertexItForParticleIndex(std::size_t particleIndex) -> decltype(_vertices.begin()) {
        return std::find_if(_vertices.begin(), _vertices.end(), [particleIndex](const Vertex &vertex) {
            return vertex.particleIndex == particleIndex;
        });
    }

};

}

#include "bits/Graph_detail.h"
