//
// Created by mho on 11/3/19.
//

#pragma once

#include "../Vertex.h"

namespace graphs {

inline Vertex::Vertex() = default;

inline Vertex::Vertex(std::size_t particleIndex, ParticleTypeId particleType)
    : particleIndex(particleIndex), particleType_(particleType), visited(false) {}

inline bool Vertex::operator==(const Vertex &rhs) const {
    return particleIndex == rhs.particleIndex;
}

inline std::ostream &operator<<(std::ostream &os, const Vertex &vertex) {
    os << fmt::format("{}", vertex);
    return os;
}

inline bool Vertex::operator!=(const Vertex &rhs) const {
    return !(rhs == *this);
}

inline const std::vector<Vertex::vertex_ptr> &Vertex::neighbors() const {
    return neighbors_;
}

inline const ParticleTypeId &Vertex::particleType() const {
    return particleType_;
}

inline ParticleTypeId &Vertex::particleType() {
    return particleType_;
}

inline Vertex::~Vertex() = default;

inline Vertex &Vertex::operator=(Vertex &&rhs) noexcept = default;

inline Vertex::Vertex(Vertex &&other) noexcept = default;

inline Vertex::Vertex(const graphs::Vertex &) = default;

inline Vertex& Vertex::operator=(const Vertex&) = default;

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
