//
// Created by mho on 10/29/19.
//

#pragma once

#include "../Graph.h"

namespace graphs {

template<typename Vertex>
inline Graph<Vertex>::Graph() = default;

template<typename Vertex>
inline Graph<Vertex>::Graph(typename Graph<Vertex>::VertexList vertexList) : _vertices(std::move(vertexList)) {}

template<typename Vertex>
inline Graph<Vertex>::~Graph() = default;

template<typename Vertex>
template<typename TupleCallback, typename TripleCallback, typename QuadrupleCallback>
inline void Graph<Vertex>::findNTuples(const TupleCallback &tuple_callback,
                                       const TripleCallback &triple_callback,
                                       const QuadrupleCallback &quadruple_callback) {
    for (auto &&v : _vertices) {
        v.visited = false;
    }

    for (auto it = _vertices.begin(); it != _vertices.end(); ++it) {
        // vertex v1
        it->visited = true;
        auto &neighbors = it->neighbors();
        for (auto it_neigh : neighbors) {
            // vertex v2 in N(v1)
            if (!it_neigh->visited) {
                // got edge (v, vv), now look for N(v)\{vv} and N(vv)\(N(v) + v)
                tuple_callback(std::tie(it, it_neigh));
                for (auto quad_it_1 : neighbors) {
                    if (it_neigh != quad_it_1) {
                        // vertex v3 in N(v1)\{v2}
                        for (auto quad_it_2 : it_neigh->neighbors()) {
                            if (quad_it_2 != it && quad_it_2 != quad_it_1) {
                                // vertex v4 in N(v2)\{v1, v3}
                                quadruple_callback(std::tie(quad_it_1, it, it_neigh, quad_it_2));
                            }
                        }
                    }
                }
            }
            for (auto it_neigh2 : neighbors) {
                if (it_neigh2 != it_neigh && it_neigh->id() < it_neigh2->id()) {
                    triple_callback(std::tie(it_neigh, it, it_neigh2));
                }
            }
        }
    }
}

template<typename Vertex>
inline const typename Graph<Vertex>::VertexList &Graph<Vertex>::vertices() const {
    return _vertices;
}

template<typename Vertex>
inline typename Graph<Vertex>::VertexList &Graph<Vertex>::vertices() {
    return _vertices;
}

template<typename Vertex>
inline typename Graph<Vertex>::VertexPtr Graph<Vertex>::firstVertex() {
    return vertices().begin();
}

template<typename Vertex>
inline typename Graph<Vertex>::VertexPtr Graph<Vertex>::lastVertex() {
    return --vertices().end();
}

template<typename Vertex>
inline typename Graph<Vertex>::VertexPtr Graph<Vertex>::toRef(const Vertex &v) {
    auto it = std::find(std::begin(_vertices), std::end(_vertices), v);
    if (it != std::end(_vertices)) {
        return {it};
    }
    throw std::invalid_argument(fmt::format(
            "Provided vertex {} was not part of the graph, no ref could be created!", v
    ));
}

template<typename Vertex>
inline bool Graph<Vertex>::containsEdge(const Edge &edge) const {
    const auto& [v1, v2] = edge;
    const auto &v1Neighbors = v1->neighbors();
    const auto &v2Neighbors = v2->neighbors();
    return std::find(v1Neighbors.begin(), v1Neighbors.end(), v2) != v1Neighbors.end()
           && std::find(v2Neighbors.begin(), v2Neighbors.end(), v1) != v2Neighbors.end();
}

template<typename Vertex>
inline bool Graph<Vertex>::containsEdge(VertexPtr v1, VertexPtr v2) const {
    return containsEdge(std::tie(v1, v2));
}

template<typename Vertex>
inline const Vertex &Graph<Vertex>::vertexForId(const typename Vertex::id_type &id) const {
    auto it = std::find_if(_vertices.begin(), _vertices.end(), [&id](const Vertex &vertex) {
        return vertex.id() == id;
    });
    if (it != _vertices.end()) {
        return *it;
    }
    throw std::invalid_argument(fmt::format("graph did not contain vertex with id {}", id));
}

template<typename Vertex>
inline void Graph<Vertex>::addVertex(typename Vertex::id_type id, typename Vertex::data_type data) {
    _vertices.emplace_back(id, std::move(data));
}

template<typename Vertex>
template<auto debug>
inline void Graph<Vertex>::addEdge(Graph::VertexPtr v1, Graph::VertexPtr v2) {
    addVertexNeighbor<debug>(*v1, v2);
    addVertexNeighbor<debug>(*v2, v1);
}

template<typename Vertex>
template<auto debug>
inline void Graph<Vertex>::addEdge(const Graph::Edge &edge) {
    addEdge<debug>(std::get<0>(edge), std::get<1>(edge));
}

template<typename Vertex>
inline void Graph<Vertex>::removeEdge(typename Vertex::id_type id1, typename Vertex::id_type id2) {
    auto it1 = vertexItForId(id1);
    auto it2 = vertexItForId(id2);
    if (it1 != _vertices.end() && it2 != _vertices.end()) {
        addVertexNeighbor(*it1, it2);
        addVertexNeighbor(*it2, it1);
    } else {
        throw std::invalid_argument("the particles indices did not exist...");
    }
}

template<typename Vertex>
inline void Graph<Vertex>::removeEdge(Graph::VertexPtr v1, Graph::VertexPtr v2) {
    assert(v1 != v2);
    removeVertexNeighbor(*v1, v2);
    removeVertexNeighbor(*v2, v1);
}

template<typename Vertex>
inline void Graph<Vertex>::removeEdge(const Graph::Edge &edge) {
    removeEdge(std::get<0>(edge), std::get<1>(edge));
}

template<typename Vertex>
inline void Graph<Vertex>::removeVertex(Graph::VertexPtr vertex) {
    removeNeighborsEdges(vertex);
    _vertices.erase(vertex);
}

template<typename Vertex>
inline void Graph<Vertex>::removeVertex(typename Vertex::id_type id) {
    auto v = vertexItForId(id);
    if (v != _vertices.end()) {
        removeNeighborsEdges(v);
        _vertices.erase(v);
    } else {
        throw std::invalid_argument(fmt::format("Vertex with id {} did not exist in the graph.", id));
    }
}

template<typename Vertex>
inline const std::vector<typename Graph<Vertex>::Edge> &Graph<Vertex>::edges() const {
    if(_dirty) {
        _edges.clear();
        findEdges([this](const Edge &tup) {
            _edges.push_back(tup);
        });
        _dirty = false;
    }
    return _edges;
}

template<typename Vertex>
inline bool Graph<Vertex>::hasEdge(const Graph::Edge &edge) const {
    const auto &v1 = std::get<0>(edge);
    const auto &v2 = std::get<1>(edge);
    const auto &e = edges();
    auto it = std::find_if(e.begin(), e.end(), [&v1, &v2](const auto& ee) {
        const auto &[ev1, ev2] = ee;
        return (v1 == ev1 && v2 == ev2) || (v1 == ev2 && v2 == ev1);
    });
    return it != e.end();
}

template<typename Vertex>
template<typename TupleCallback>
inline void Graph<Vertex>::findEdges(const TupleCallback &edgeCallback) const {
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

template<typename Vertex>
inline std::tuple<std::vector<typename Graph<Vertex>::Edge>,
                  std::vector<typename Graph<Vertex>::Path2>,
                  std::vector<typename Graph<Vertex>::Path3>> Graph<Vertex>::findNTuples() {
    auto tuple = std::make_tuple(std::vector<Edge>(), std::vector<Path2>(), std::vector<Path3>());
    findNTuples([&](const Edge &tup) {
        std::get<0>(tuple).push_back(tup);
    }, [&](const Path2 &path2) {
        std::get<1>(tuple).push_back(path2);
    }, [&](const Path3 &path3) {
        std::get<2>(tuple).push_back(path3);
    });
    return tuple;
}

template<typename Vertex>
template<auto debug>
inline void Graph<Vertex>::addVertexNeighbor(Vertex &v1, const VertexPtr &v2) {
    _dirty = true;
    v1.template addNeighbor<debug>(v2);
}

template<typename Vertex>
template<auto debug>
inline void Graph<Vertex>::removeVertexNeighbor(Vertex &v1, const VertexPtr &v2) {
    _dirty = true;
    v1.template removeNeighbor<debug>(v2);
}

template<typename Vertex>
inline bool Graph<Vertex>::isConnected() const {
    resetVertexVisitedState();

    std::vector<typename VertexList::const_iterator> unvisited;
    unvisited.emplace_back(_vertices.begin());
    std::size_t nVisited = 0;
    while(!unvisited.empty()) {
        auto vertex = unvisited.back();
        unvisited.pop_back();
        if(!vertex->visited) {
            vertex->visited = true;
            ++nVisited;
            for (auto neighbor : vertex->neighbors()) {
                if (!neighbor->visited) {
                    unvisited.emplace_back(neighbor);
                }
            }
        }
    }
    return nVisited == _vertices.size();
}

template<typename Vertex>
inline void Graph<Vertex>::resetVertexVisitedState() const {
    std::for_each(_vertices.begin(), _vertices.end(), [](const Vertex &v) { v.visited = false; });
}

template<typename Vertex>
inline std::vector<Graph<Vertex>> Graph<Vertex>::connectedComponentsDestructive() {
    _dirty = true;
    std::vector<VertexList> subVertexLists {};
    {
        std::vector<std::vector<VertexPtr>> components;

        std::for_each(_vertices.begin(), _vertices.end(), [](Vertex &v) { v.visited = false; });

        for(auto it = _vertices.begin(); it != _vertices.end(); ++it) {
            if(!it->visited) {
                // got a new component
                components.emplace_back();
                subVertexLists.emplace_back();

                auto& component = components.back();

                std::vector<VertexPtr> unvisitedInComponent;
                unvisitedInComponent.emplace_back(it);
                while (!unvisitedInComponent.empty()) {
                    auto& vertex = unvisitedInComponent.back();
                    unvisitedInComponent.pop_back();
                    if (!vertex->visited) {
                        vertex->visited = true;
                        {
                            component.emplace_back(vertex);
                        }
                        for (auto neighbor : vertex->neighbors()) {
                            if (!neighbor->visited) {
                                unvisitedInComponent.emplace_back(neighbor);
                            }
                        }
                    }
                }
            }
        }

        {
            // transfer vertices
            auto it_components = components.begin();
            auto it_subLists = subVertexLists.begin();
            for(; it_components != components.end(); ++it_components, ++it_subLists) {
                for(const auto& vertex_ref : *it_components) {
                    it_subLists->splice(it_subLists->end(), _vertices, vertex_ref);
                }
            }
        }
    }

    std::vector<Graph> subGraphs {};
    subGraphs.reserve(subVertexLists.size());
    {
        for (auto &subVertexList : subVertexLists) {
            subGraphs.emplace_back(std::move(subVertexList));
        }
    }
    return std::move(subGraphs);
}

template<typename Vertex>
inline void Graph<Vertex>::removeNeighborsEdges(Graph::VertexPtr vertex) {
    std::for_each(std::begin(vertex->neighbors()), std::end(vertex->neighbors()), [this, vertex](const auto neighbor) {
        removeVertexNeighbor(*neighbor, vertex);
    });
    _dirty = true;
}

template<typename Vertex>
inline typename Graph<Vertex>::VertexPtr Graph<Vertex>::vertexItForId(const typename Vertex::id_type &id) {
    return std::find_if(_vertices.begin(), _vertices.end(), [&id](const Vertex &vertex) {
        return vertex.id() == id;
    });
}

template<typename Vertex>
inline std::size_t Graph<Vertex>::nEdges() const {
    return edges().size();
}

}
