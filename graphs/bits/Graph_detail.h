//
// Created by mho on 10/29/19.
//

#pragma once

#include "../Graph.h"

namespace graphs {

namespace {
template<typename T, size_t... I>
auto reverse_tuple_impl(T t, std::index_sequence<I...>) {
    return std::make_tuple(std::get<sizeof...(I) - 1 - I>(std::forward<T>(t))...);
}

template<typename T>
auto reverse_tuple(T t) {
    return reverse_tuple_impl(std::forward<T>(t), std::make_index_sequence<std::tuple_size<T>::value>());
}
}

template<typename Vertex>
inline Graph<Vertex>::Graph() = default;

template<typename Vertex>
inline Graph<Vertex>::Graph(typename Graph<Vertex>::VertexList vertexList) : _vertices(std::move(vertexList)) {
    findEdges([this](const auto& edge) {
        _edges.push_back(edge);
    });
}

template<typename Vertex>
inline Graph<Vertex>::~Graph() = default;

template<typename Vertex>
template<typename PairCallback, typename TripleCallback, typename QuadrupleCallback>
inline void Graph<Vertex>::findNTuples(const PairCallback &pairCallback,
                                       const TripleCallback &tripleCallback,
                                       const QuadrupleCallback &quadrupleCallback) {
    std::vector<char> visited (_vertices.size(), false);

    for (auto vertexIndex = 0; vertexIndex < _vertices.size(); ++vertexIndex) {
        // vertex v1
        visited.at(vertexIndex) = true;
        const auto &v1 = vertices().at(vertexIndex);
        if(!v1.deactivated()) {
            auto &neighbors = v1.neighbors();
            for (auto neighborIndex : neighbors) {
                // vertex v2 in N(v1)
                if (!visited.at(neighborIndex)) {
                    const auto &v2 = _vertices.at(neighborIndex);
                    pairCallback(std::tie(vertexIndex, neighborIndex));
                    for (auto quadIx1 : neighbors) {
                        if (neighborIndex != quadIx1) {
                            // vertex v3 in N(v1)\{v2}
                            for (auto quadIx2 : v2.neighbors()) {
                                if (quadIx2 != vertexIndex && quadIx2 != quadIx1) {
                                    // vertex v4 in N(v2)\{v1, v3}
                                    quadrupleCallback(std::tie(quadIx1, vertexIndex, neighborIndex, quadIx2));
                                }
                            }
                        }
                    }
                }
                for (auto neighborIx2 : neighbors) {
                    if (neighborIx2 != neighborIndex && neighborIx2 < neighborIndex) {
                        tripleCallback(std::tie(neighborIx2, vertexIndex, neighborIndex));
                    }
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
inline bool Graph<Vertex>::containsEdge(const Edge &edge) const {
    if(std::find(edges().begin(), edges().end(), edge) != edges().end()) {
        return true;
    } else {
        return std::find(edges().begin(), edges().end(), reverse_tuple(edge)) != edges().end();
    }
}

template<typename Vertex>
inline bool Graph<Vertex>::containsEdge(VertexIndex v1, VertexIndex v2) const {
    return containsEdge(std::tie(v1, v2));
}

template<typename Vertex>
inline typename Graph<Vertex>::VertexIndex Graph<Vertex>::addVertex(typename Vertex::data_type data) {
    auto it = _vertices.emplace_back(std::move(data));
    return std::distance(_vertices.begin(), it);
}

template<typename Vertex>
template<auto debug>
inline void Graph<Vertex>::addEdge(VertexIndex ix1, VertexIndex ix2) {
    auto &v1 = _vertices.at(ix1);
    auto &v2 = _vertices.at(ix2);
    addVertexNeighbor<debug>(v1, ix2);
    addVertexNeighbor<debug>(v2, ix1);
    _edges.push_back(std::make_tuple(ix1, ix2));
}

template<typename Vertex>
template<auto debug>
inline void Graph<Vertex>::addEdge(const Graph::Edge &edge) {
    addEdge<debug>(std::get<0>(edge), std::get<1>(edge));
}

template<typename Vertex>
inline void Graph<Vertex>::removeEdge(VertexIndex ix1, VertexIndex ix2) {
    auto &v1 = _vertices.at(ix1);
    auto &v2 = _vertices.at(ix2);
    auto it = std::find_if(_edges.begin(), _edges.end(), [ix1, ix2](const auto& edge) {
        const auto &[e1, e2] = edge;
        return (e1 == ix1 && e2 == ix2) || (e1 == ix2 && e2 == ix1);
    });
    if(it != edges().end()) {
        removeVertexNeighbor(v1, ix2);
        removeVertexNeighbor(v2, ix1);
        _edges.erase(it);
    } else {
        throw std::invalid_argument(fmt::format("Tried to remove non-existing edge {} - {}", ix1, ix2));
    }
}

template<typename Vertex>
inline void Graph<Vertex>::removeEdge(const Graph::Edge &edge) {
    removeEdge(std::get<0>(edge), std::get<1>(edge));
}

template<typename Vertex>
inline void Graph<Vertex>::removeVertex(VertexIndex ix) {
    removeNeighborsEdges(ix);
    _vertices.erase(_vertices.begin() + ix);
    _edges.erase(std::remove_if(_edges.begin(), _edges.end(), [ix](const auto &edge) {
        return std::get<0>(edge) == ix || std::get<1>(edge) == ix;
    }), _edges.end());
}

template<typename Vertex>
inline const std::vector<typename Graph<Vertex>::Edge> &Graph<Vertex>::edges() const {
    return _edges;
}

template<typename Vertex>
template<typename PairCallback>
inline void Graph<Vertex>::findEdges(const PairCallback &edgeCallback) const {
    for(std::size_t i = 0; i < vertices().size(); ++i) {
        for(VertexIndex neighbor : vertices().at(i).neighbors()) {
            if(neighbor < i) {
                const Edge e{std::tie(i, neighbor)};
                edgeCallback(e);
            }
        }
    }
}

template<typename Vertex>
inline std::tuple<std::vector<typename Graph<Vertex>::Edge>,
                  std::vector<typename Graph<Vertex>::Path3>,
                  std::vector<typename Graph<Vertex>::Path4>> Graph<Vertex>::findNTuples() {
    auto tuple = std::make_tuple(std::vector<Edge>(), std::vector<Path3>(), std::vector<Path4>());
    findNTuples([&](const Edge &edge) {
        std::get<0>(tuple).push_back(edge);
    }, [&](const Path3 &path3) {
        std::get<1>(tuple).push_back(path3);
    }, [&](const Path4 &path4) {
        std::get<2>(tuple).push_back(path4);
    });
    return tuple;
}

template<typename Vertex>
template<auto debug>
inline void Graph<Vertex>::addVertexNeighbor(Vertex &v1, VertexIndex v2) {
    v1.template addNeighbor<debug>(v2);
}

template<typename Vertex>
template<auto debug>
inline void Graph<Vertex>::removeVertexNeighbor(Vertex &v1, VertexIndex v2) {
    v1.template removeNeighbor<debug>(v2);
}

template<typename Vertex>
inline bool Graph<Vertex>::isConnected() const {
    if(_vertices.empty()) return true;

    std::vector<char> visited (_vertices.size(), false);

    std::vector<VertexIndex> unvisited;
    unvisited.reserve(_vertices.size());
    unvisited.emplace_back([this](){
        for(std::size_t i = 0; i < _vertices.size(); ++i) {
            if(!_vertices.at(i).deactivated()) return i;
        }
        throw std::logic_error("vertices is not empty but could not find an active vertex");
    }());

    std::size_t nVisited = 0;
    while(!unvisited.empty()) {
        auto vertexIndex = unvisited.back();
        const auto &vertex = _vertices.at(vertexIndex);
        unvisited.pop_back();
        if(!visited.at(vertexIndex)) {
            visited.at(vertexIndex) = true;
            ++nVisited;
            for (auto neighbor : vertex.neighbors()) {
                if (!visited.at(neighbor)) {
                    unvisited.emplace_back(neighbor);
                }
            }
        }
    }
    return nVisited == _vertices.size();
}

template<typename Vertex>
inline std::vector<Graph<Vertex>> Graph<Vertex>::connectedComponents() {
    std::vector<VertexList> subVertexLists {};

    {
        std::vector<std::vector<VertexIndex>> components;
        std::vector<char> visited (_vertices.size(), false);

        for(std::size_t ix = 0; ix < _vertices.size(); ++ix) {
            if(!_vertices.at(ix).deactivated() && !visited.at(ix)) {
                // got a new component
                components.emplace_back();
                subVertexLists.emplace_back();

                auto& component = components.back();

                std::vector<VertexIndex> unvisitedInComponent;
                unvisitedInComponent.emplace_back(ix);
                while (!unvisitedInComponent.empty()) {
                    auto vertexIndex = unvisitedInComponent.back();
                    unvisitedInComponent.pop_back();
                    if (!visited.at(vertexIndex)) {
                        visited.at(vertexIndex) = true;
                        component.emplace_back(vertexIndex);
                        for (auto neighbor : _vertices.at(vertexIndex).neighbors()) {
                            if (!visited.at(neighbor)) {
                                unvisitedInComponent.emplace_back(neighbor);
                            }
                        }
                    }
                }
            }
        }

        {
            // transfer vertices
            auto itComponents = components.begin();
            auto itSubLists = subVertexLists.begin();

            // mapping (previous vertex index) -> (new vertex index)
            std::vector<std::vector<VertexIndex>> reverseMappings;
            {
                reverseMappings.reserve(components.size());
                for(const auto &component : components) {
                    std::vector<VertexIndex> reverseMapping;
                    reverseMapping.resize(_vertices.size());
                    for(std::size_t i = 0; i < component.size(); ++i) {
                        reverseMapping[component.at(i)] = i;
                    }
                    reverseMappings.push_back(std::move(reverseMapping));
                }
            }

            auto itReverseMappings = reverseMappings.begin();

            for(; itComponents != components.end(); ++itComponents, ++itSubLists, ++itReverseMappings) {
                for(std::size_t i = 0; i < (*itComponents).size(); ++i) {
                    auto previousVertexIndex = itComponents->at(i);
                    itSubLists->push_back(_vertices.at(previousVertexIndex));
                    for(auto &neighborIndex : (itSubLists->end()-1)->neighbors()) {
                        neighborIndex = itReverseMappings->at(neighborIndex);
                    }
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
inline void Graph<Vertex>::removeNeighborsEdges(VertexIndex ix) {
    auto &vertex = _vertices.at(ix);
    std::for_each(std::begin(vertex.neighbors()), std::end(vertex.neighbors()), [this, ix](const auto neighbor) {
        removeVertexNeighbor(_vertices.at(neighbor), ix);
    });
}

template<typename Vertex>
inline std::size_t Graph<Vertex>::nEdges() const {
    return edges().size();
}

template<typename Vertex>
inline std::vector<typename Graph<Vertex>::VertexIndex> Graph<Vertex>::append(const Graph &other) {
    std::vector<VertexIndex> indexMapping;
    indexMapping.resize(other.vertices().size());
    // insert vertices
    {
        VertexIndex ix{0};
        for (const auto &v : other.vertices()) {
            if (!v.deactivated()) {
                auto newIndex = addVertex(v.data());
                indexMapping.at(ix) = newIndex;
            }
            ++ix;
        }
    }
    // insert edges
    {
        VertexIndex ix{0};
        for(const auto &v : other.vertices()) {
            if(!v.deactivated()) {
                auto newIndex = indexMapping.at(ix);
                for(auto neighborIndex : v.neighbors()) {
                    // only add once since neighbors are bidirectional
                    if(ix < neighborIndex) {
                        addEdge(newIndex, indexMapping.at(neighborIndex));
                    }
                }
            }
            ++ix;
        }
    }
    return std::move(indexMapping);
}

template<typename Vertex>
inline std::vector<typename Graph<Vertex>::VertexIndex> Graph<Vertex>::append(const Graph<Vertex> &other,
                                                                              VertexIndex edgeIndexThis,
                                                                              VertexIndex edgeIndexOther) {
    auto mapping = append(other);
    addEdge(edgeIndexThis, mapping.at(edgeIndexOther));
    return std::move(mapping);
}

}
