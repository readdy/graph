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

    Graph(const Graph &);

    Graph &operator=(const Graph &);

    Graph(Graph &&) = default;

    Graph &operator=(Graph &&) = default;

    const VertexList &vertices() const {
        return _vertices;
    }

    VertexList &vertices() {
        return _vertices;
    }

    VertexPtr firstVertex() {
        return vertices().begin();
    }

    VertexPtr lastVertex() {
        return --vertices().end();
    }

    VertexPtr toRef(const Vertex &v) {
        auto it = std::find(std::begin(_vertices), std::end(_vertices), v);
        if (it != std::end(_vertices)) {
            return {it};
        }
        //throw std::invalid_argument(fmt::format(
        //        "Provided vertex {} was not part of the graph, no ref could be created!", v
        //));
        // todo
    }

    bool containsEdge(const Edge &edge) const {
        const auto& [v1, v2] = edge;
        const auto &v1Neighbors = v1->neighbors();
        const auto &v2Neighbors = v2->neighbors();
        return std::find(v1Neighbors.begin(), v1Neighbors.end(), v2) != v1Neighbors.end()
               && std::find(v2Neighbors.begin(), v2Neighbors.end(), v1) != v2Neighbors.end();
    }

    bool containsEdge(VertexPtr v1, VertexPtr v2) const {
        return containsEdge(std::tie(v1, v2));
    }

    const Vertex &vertexForParticleIndex(std::size_t particleIndex) const {
        auto it = std::find_if(_vertices.begin(), _vertices.end(), [particleIndex](const Vertex &vertex) {
            return vertex.particleIndex == particleIndex;
        });
        if (it != _vertices.end()) {
            return *it;
        }
        throw std::invalid_argument("graph did not contain the particle index " + std::to_string(particleIndex));
    }

    void addVertex(std::size_t particleIndex, ParticleTypeId particleType) {
        _vertices.emplace_back(particleIndex, particleType);
    }

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

    void addEdge(VertexPtr v1, VertexPtr v2) {
        addVertexNeighbor(*v1, v2);
        addVertexNeighbor(*v2, v1);
    }

    void addEdge(const Edge &edge) {
        addEdge(std::get<0>(edge), std::get<1>(edge));
    }

    void addEdgeBetweenParticles(std::size_t particleIndex1, std::size_t particleIndex2) {
        auto it1 = vertexItForParticleIndex(particleIndex1);
        auto it2 = vertexItForParticleIndex(particleIndex2);
        if (it1 != _vertices.end() && it2 != _vertices.end()) {
            addVertexNeighbor(*it1, it2);
            addVertexNeighbor(*it2, it1);
        } else {
            throw std::invalid_argument("the particles indices did not exist...");
        }
    }

    void removeEdge(VertexPtr v1, VertexPtr v2) {
        assert(v1 != v2);
        removeVertexNeighbor(*v1, v2);
        removeVertexNeighbor(*v2, v1);
    }

    void removeEdge(const Edge &edge) {
        removeEdge(std::get<0>(edge), std::get<1>(edge));
    }

    void removeVertex(VertexPtr vertex) {
        removeNeighborsEdges(vertex);
        _vertices.erase(vertex);
    }

    void removeParticle(std::size_t particleIndex) {
        auto v = vertexItForParticleIndex(particleIndex);
        if (v != _vertices.end()) {
            removeNeighborsEdges(v);
            _vertices.erase(v);
        } else {
            throw std::invalid_argument(
                    "the vertex corresponding to the particle with topology index " + std::to_string(particleIndex) +
                    " did not exist in the graph");
        }
    }

    bool isConnected() const;

    const std::vector<Edge> &edges() const {
        if(_dirty) {
            _edges.clear();
            findEdges([this](const Edge &tup) {
                _edges.push_back(tup);
            });
            _dirty = false;
        }
        return _edges;
    };

    bool hasEdge(const Edge &edge) const {
        const auto &v1 = std::get<0>(edge);
        const auto &v2 = std::get<1>(edge);
        const auto &e = edges();
        auto it = std::find_if(e.begin(), e.end(), [&v1, &v2](const auto& ee) {
            const auto &[ev1, ev2] = ee;
            return (v1 == ev1 && v2 == ev2) || (v1 == ev2 && v2 == ev1);
        });
        return it != e.end();
    }

    template<typename TupleCallback>
    void findEdges(const TupleCallback &edgeCallback) const {
        for (auto &&v : vertices()) {
            v.visited = false;
        }

        auto &vert = const_cast<VertexList &>(_vertices);
        for (auto it = vert.begin(); it != vert.end(); ++it) {
            it->visited = true;
            auto &neighbors = it->neighbors();
            for (auto it_neigh : neighbors) {
                if (!it_neigh->visited) {
                    const Edge e{std::tie(it, it_neigh)};
                    edgeCallback(e);
                }
            }
        }
    }

    template<typename TupleCallback, typename TripleCallback, typename QuadrupleCallback>
    void findNTuples(const TupleCallback &tuple_callback,
                     const TripleCallback &triple_callback,
                     const QuadrupleCallback &quadruple_callback);

    std::tuple<std::vector<Edge>, std::vector<Path2>, std::vector<Path3>>
    findNTuples() {
        auto tuple = std::make_tuple(std::vector<Edge>(), std::vector<Path2>(), std::vector<Path3>());
        findNTuples([&](const Edge &tup) {
            std::get<0>(tuple).push_back(tup);
        }, [&](const Path2 &path2) {
            std::get<1>(tuple).push_back(path2);
        }, [&](const Path3 &path3) {
            std::get<2>(tuple).push_back(path3);
        });
        return tuple;
    };

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
