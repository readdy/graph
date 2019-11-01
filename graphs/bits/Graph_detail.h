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
        it->visited = true;
        auto v_type = it->particleType();
        auto v_idx = it->particleIndex;
        auto &neighbors = it->neighbors();
        for (auto it_neigh : neighbors) {
            auto vv_type = it_neigh->particleType();
            auto vv_idx = it_neigh->particleIndex;
            if (!it_neigh->visited) {
                // todo log::trace("got type tuple ({}, {}) for particles {}, {}", v_type, vv_type, v_idx, vv_idx);
                // got edge (v, vv), now look for N(v)\{vv} and N(vv)\(N(v) + v)
                tuple_callback(std::tie(it, it_neigh));
                for (auto quad_it_1 : neighbors) {
                    // N(v)\{vv}
                    if (it_neigh != quad_it_1) {
                        auto vvv_type = quad_it_1->particleType();
                        auto vvv_idx = quad_it_1->particleIndex;
                        // got one end of the quadruple
                        for (auto quad_it_2 : it_neigh->neighbors()) {
                            // if this other neighbor is no neighbor of v and not v itself,
                            // we got the other end of the quadruple
                            auto no_circle =
                                    std::find(neighbors.begin(), neighbors.end(), quad_it_2) == neighbors.end();
                            if (quad_it_2 != it && no_circle) {
                                auto vvvv_type = quad_it_2->particleType();
                                auto vvvv_idx = quad_it_2->particleIndex;
                                // todo log::trace("got type quadruple ({}, {}, {}, {}) for particles {}, {}, {}, {}", vvv_type, v_type,
                                //           vv_type, vvvv_type, vvv_idx, v_idx, vv_idx, vvvv_idx);
                                quadruple_callback(std::tie(quad_it_1, it, it_neigh, quad_it_2));
                            }
                        }
                    }
                }
            }
            for (auto it_neigh2 : neighbors) {
                if (it_neigh2 != it_neigh && it_neigh->particleIndex < it_neigh2->particleIndex) {
                    auto vvv_type = it_neigh2->particleType();
                    auto vvv_idx = it_neigh2->particleIndex;
                    // todo log::trace("got type triple ({}, {}, {}) for particles {}, {}, {}", vv_type, v_type, vvv_type,
                    //           vv_idx, v_idx, vvv_idx);
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
inline bool Graph<Vertex>::containsEdge(const Graph::Edge &edge) const {
    const auto& [v1, v2] = edge;
    const auto &v1Neighbors = v1->neighbors();
    const auto &v2Neighbors = v2->neighbors();
    return std::find(v1Neighbors.begin(), v1Neighbors.end(), v2) != v1Neighbors.end()
           && std::find(v2Neighbors.begin(), v2Neighbors.end(), v1) != v2Neighbors.end();
}

}
