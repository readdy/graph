//
// Created by mho on 11/3/19.
//

#pragma once

#include "../Vertex.h"

namespace graphs {

inline Vertex::Vertex() = default;

inline Vertex::Vertex(std::size_t particleIndex, ParticleTypeId particleType)
    : _particleIndex(particleIndex), _particleType(particleType), visited(false) {}

inline bool Vertex::operator==(const Vertex &rhs) const {
    return _particleIndex == rhs._particleIndex;
}

inline std::ostream &operator<<(std::ostream &os, const Vertex &vertex) {
    os << fmt::format("{}", vertex);
    return os;
}

inline bool Vertex::operator!=(const Vertex &rhs) const {
    return !(rhs == *this);
}

inline const std::vector<Vertex::VertexPtr> &Vertex::neighbors() const {
    return _neighbors;
}

inline const ParticleTypeId &Vertex::particleType() const {
    return _particleType;
}

inline void Vertex::setParticleType(ParticleTypeId typeId) {
    _particleType = typeId;
}

inline std::size_t Vertex::particleIndex() const {
    return _particleIndex;
}

inline void Vertex::setParticleIndex(std::size_t particleIndex) {
    _particleIndex = particleIndex;
}

template<auto debug>
inline void Vertex::addNeighbor(Vertex::VertexPtr neighbor) {
    auto newNeighbor = std::find(_neighbors.begin(), _neighbors.end(), neighbor) == _neighbors.end();
    if(newNeighbor) {
        _neighbors.push_back(neighbor);
    }
    if constexpr (debug != nullptr) {
        if(!newNeighbor) {
            (*debug)(fmt::format("tried to add an already existing edge ({} - {})",
                                 _particleIndex, neighbor->_particleIndex));
        }
    }

}

template<auto debug>
inline void Vertex::removeNeighbor(Vertex::VertexPtr neighbor) {
    auto it = std::find(_neighbors.begin(), _neighbors.end(), neighbor);
    if (it != _neighbors.end()) {
        _neighbors.erase(it);
    } else {
        if constexpr (debug != nullptr) {
            (*debug)(fmt::format("tried to remove a non existing edge {} - {}",
                                 _particleIndex, neighbor->_particleIndex));
        }
    }
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
            ss << neighbor->particleIndex();
            first = false;
        }
        return format_to(ctx.out(), "Vertex[particleIndex: {}, neighbors=[{}]]", v.particleIndex(), ss.str());
    }
};
}
