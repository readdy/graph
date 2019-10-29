//
// Created by mho on 10/28/19.
//

#pragma once

#include <list>
#include <ostream>
#include <vector>

namespace graphs {

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

    /**
     * default constructor
     */
    Vertex() = default;

    /**
     * constructs a vertex to a graph
     * @param particleIndex the particle index this vertex belongs to
     */
    Vertex(std::size_t particleIndex, ParticleTypeId particleType)
            : particleIndex(particleIndex), particleType_(particleType), visited(false) {}

    Vertex(const Vertex &) = default;

    Vertex &operator=(const Vertex &) = default;

    Vertex(Vertex && other) noexcept = default;

    Vertex &operator=(Vertex && rhs) noexcept = default;

    /**
     * default destructor
     */
    virtual ~Vertex() = default;

    /**
     * particle index in the topology this vertex belongs to
     */
    std::size_t particleIndex{0};

    bool operator==(const Vertex &rhs) const {
        return particleIndex == rhs.particleIndex;
    }

    friend std::ostream &operator<<(std::ostream &os, const Vertex &vertex) {
        os << "Vertex[particleIndex: " << vertex.particleIndex << ", neighbors=[";
        for (const auto neighbor : vertex.neighbors_) {
            os << neighbor->particleIndex << ",";
        }
        os << "]]";
        return os;
    }

    bool operator!=(const Vertex &rhs) const {
        return !(rhs == *this);
    }

    const std::vector<vertex_ptr> &neighbors() const {
        return neighbors_;
    }

    const ParticleTypeId &particleType() const {
        return particleType_;
    }

    ParticleTypeId &particleType() {
        return particleType_;
    }

private:
    friend class graphs::Graph;

    /**
     * the edges (i.e., pointers to neighboring vertices)
     */
    std::vector<vertex_ptr> neighbors_{};
    mutable bool visited;

    ParticleTypeId particleType_{0};
};

}
