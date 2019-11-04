//
// Created by mho on 11/3/19.
//

#pragma once

#include "../Vertex.h"

namespace graphs {

template<typename ParticleTypeId>
inline Vertex<ParticleTypeId>::Vertex() = default;

template<typename ParticleTypeId>
inline Vertex<ParticleTypeId>::Vertex(std::size_t particleIndex, ParticleTypeId particleType)
    : _particleIndex(particleIndex), _particleType(particleType), visited(false) {}

template<typename ParticleTypeId>
inline bool Vertex<ParticleTypeId>::operator==(const Vertex &rhs) const {
    return _particleIndex == rhs._particleIndex;
}

template<typename ParticleTypeId>
inline std::ostream &operator<<(std::ostream &os, const Vertex<ParticleTypeId> &vertex) {
    os << fmt::format("{}", vertex);
    return os;
}

template<typename ParticleTypeId>
inline bool Vertex<ParticleTypeId>::operator!=(const Vertex &rhs) const {
    return !(rhs == *this);
}

template<typename ParticleTypeId>
inline const std::vector<typename Vertex<ParticleTypeId>::VertexPtr> &Vertex<ParticleTypeId>::neighbors() const {
    return _neighbors;
}

template<typename ParticleTypeId>
inline const ParticleTypeId &Vertex<ParticleTypeId>::particleType() const {
    return _particleType;
}

template<typename ParticleTypeId>
inline void Vertex<ParticleTypeId>::setParticleType(ParticleTypeId typeId) {
    _particleType = typeId;
}

template<typename ParticleTypeId>
inline std::size_t Vertex<ParticleTypeId>::particleIndex() const {
    return _particleIndex;
}

template<typename ParticleTypeId>
inline void Vertex<ParticleTypeId>::setParticleIndex(std::size_t particleIndex) {
    _particleIndex = particleIndex;
}

template<typename ParticleTypeId>
template<auto debug>
inline void Vertex<ParticleTypeId>::addNeighbor(VertexPtr neighbor) {
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

template<typename ParticleTypeId>
template<auto debug>
inline void Vertex<ParticleTypeId>::removeNeighbor(VertexPtr neighbor) {
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

template<typename ParticleTypeId>
inline Vertex<ParticleTypeId>::~Vertex() = default;

template<typename ParticleTypeId>
inline Vertex<ParticleTypeId> &Vertex<ParticleTypeId>::operator=(Vertex &&rhs) noexcept = default;

template<typename ParticleTypeId>
inline Vertex<ParticleTypeId>::Vertex(Vertex &&other) noexcept = default;

template<typename ParticleTypeId>
inline Vertex<ParticleTypeId>::Vertex(const graphs::Vertex<ParticleTypeId> &) = default;

template<typename ParticleTypeId>
inline Vertex<ParticleTypeId>& Vertex<ParticleTypeId>::operator=(const Vertex&) = default;

}

namespace fmt {
template <typename ParticleTypeId>
struct formatter<graphs::Vertex<ParticleTypeId>> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const graphs::Vertex<ParticleTypeId> &v, FormatContext &ctx) {
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
