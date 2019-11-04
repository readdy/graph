//
// Created by mho on 10/28/19.
//

#pragma once

#include <list>
#include <ostream>
#include <vector>
#include <sstream>

#include <fmt/format.h>

using ParticleTypeId = std::size_t;

namespace graphs {

class Vertex;

template<typename Vertex>
class Graph;

/**
 * Struct representing a vertex in a topology-connectivity-graph
 */
class Vertex {
public:
    /**
     * edge in the graph (i.e., pointer to neighboring vertex)
     */
    using VertexPtr = std::list<Vertex>::iterator;
    using VertexCPtr = std::list<Vertex>::const_iterator;

    Vertex();

    Vertex(std::size_t particleIndex, ParticleTypeId particleType);

    Vertex(const Vertex &);

    Vertex &operator=(const Vertex &);

    Vertex(Vertex && other) noexcept;

    Vertex &operator=(Vertex && rhs) noexcept;

    virtual ~Vertex();

    /**
     * particle index in the topology this vertex belongs to
    */
    std::size_t particleIndex() const;

    void setParticleIndex(std::size_t particleIndex);

    bool operator==(const Vertex &rhs) const;

    friend std::ostream &operator<<(std::ostream &os, const Vertex &vertex);

    bool operator!=(const Vertex &rhs) const;

    const std::vector<VertexPtr> &neighbors() const;

    template<auto debug=nullptr>
    void addNeighbor(VertexPtr neighbor);

    template<auto debug=nullptr>
    void removeNeighbor(VertexPtr neighbor);

    const ParticleTypeId &particleType() const;

    void setParticleType(ParticleTypeId typeId);

private:
    friend class graphs::Graph<graphs::Vertex>;

    /**
     * the edges (i.e., pointers to neighboring vertices)
     */
    std::vector<VertexPtr> _neighbors{};
    mutable bool visited {false};

    ParticleTypeId _particleType{0};
    std::size_t _particleIndex {0};
};

}

#include "bits/Vertex_detail.h"
