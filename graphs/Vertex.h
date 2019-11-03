//
// Created by mho on 10/28/19.
//

#pragma once

#include <list>
#include <ostream>
#include <vector>
#include <sstream>

using ParticleTypeId = std::size_t;

namespace graphs {

class Vertex;

template<typename Vertex = graphs::Vertex>
class Graph;

/**
 * Struct representing a vertex in a topology-connectivity-graph
 */
class Vertex {
public:
    /**
     * edge in the graph (i.e., pointer to neighboring vertex)
     */
    using vertex_ptr = std::list<Vertex>::iterator;
    using vertex_cptr = std::list<Vertex>::const_iterator;

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
    std::size_t particleIndex{0};

    bool operator==(const Vertex &rhs) const;

    friend std::ostream &operator<<(std::ostream &os, const Vertex &vertex);

    bool operator!=(const Vertex &rhs) const;

    const std::vector<vertex_ptr> &neighbors() const;

    const ParticleTypeId &particleType() const;

    ParticleTypeId &particleType();

private:
    friend class graphs::Graph<graphs::Vertex>;

    /**
     * the edges (i.e., pointers to neighboring vertices)
     */
    std::vector<vertex_ptr> neighbors_{};
    mutable bool visited {false};

    ParticleTypeId particleType_{0};
};

}

#include "bits/Vertex_detail.h"
