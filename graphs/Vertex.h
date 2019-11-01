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
        os << fmt::format("{}", vertex);
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
    friend class graphs::Graph<graphs::Vertex>;

    /**
     * the edges (i.e., pointers to neighboring vertices)
     */
    std::vector<vertex_ptr> neighbors_{};
    mutable bool visited {false};

    ParticleTypeId particleType_{0};
};

}

namespace fmt {
template <>
struct formatter<graphs::Vertex> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const graphs::Vertex &v, FormatContext &ctx) {
        std::stringstream ss;
        bool first {true};
        for (const auto neighbor : v.neighbors()) {
            if(!first) {
                ss << ",";
            }
            ss << neighbor->particleIndex;
            first = false;
        }
        return format_to(ctx.out(), "Vertex[particleIndex: {}, neighbors=[{}]]", v.particleIndex, ss.str());
    }
};
}
