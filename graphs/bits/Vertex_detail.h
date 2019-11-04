//
// Created by mho on 11/3/19.
//

#pragma once

#include "../Vertex.h"

namespace graphs {

template<typename ID, typename... T>
inline Vertex<ID, T...>::Vertex(id_type id, T&&... data) : _data(std::tuple<T...>(std::forward<T>(data)...)),
                                                           _id(id), visited(false) {}

template<typename ID, typename... T>
inline Vertex<ID, T...>::Vertex(id_type id, Vertex::data_type data) : _id(id), visited(false), _data(std::move(data)) {}

template<typename ID, typename... T>
inline bool Vertex<ID, T...>::operator==(const Vertex &rhs) const {
    return _id == rhs._id;
}

template<typename ID, typename... T>
inline bool Vertex<ID, T...>::operator!=(const Vertex &rhs) const {
    return !(rhs == *this);
}

template<typename ID, typename... T>
inline std::ostream &operator<<(std::ostream &os, const Vertex<ID, T...> &vertex) {
    os << fmt::format("{}", vertex);
    return os;
}

template<typename ID, typename... T>
inline const std::vector<typename Vertex<ID, T...>::VertexPtr> &Vertex<ID, T...>::neighbors() const {
    return _neighbors;
}

template<typename ID, typename... T>
inline const typename Vertex<ID, T...>::data_type &Vertex<ID, T...>::data() const {
    return _data;
}

template<typename ID, typename... T>
inline void Vertex<ID, T...>::setData(data_type data) {
    _data = std::move(data);
}

template<typename ID, typename... T>
inline const typename Vertex<ID, T...>::id_type &Vertex<ID, T...>::id() const {
    return _id;
}

template<typename ID, typename... T>
inline void Vertex<ID, T...>::setId(const id_type &id) {
    _id = std::move(id);
}

template<typename ID, typename... T>
template<auto debug>
inline void Vertex<ID, T...>::addNeighbor(VertexPtr neighbor) {
    auto newNeighbor = std::find(_neighbors.begin(), _neighbors.end(), neighbor) == _neighbors.end();
    if(newNeighbor) {
        _neighbors.push_back(neighbor);
    }
    if constexpr (debug != nullptr) {
        if(!newNeighbor) {
            (*debug)(fmt::format("tried to add an already existing edge ({} - {})", _data, neighbor->_data));
        }
    }
}

template<typename ID, typename... T>
template<auto debug>
inline void Vertex<ID, T...>::removeNeighbor(VertexPtr neighbor) {
    auto it = std::find(_neighbors.begin(), _neighbors.end(), neighbor);
    if (it != _neighbors.end()) {
        _neighbors.erase(it);
    } else {
        if constexpr (debug != nullptr) {
            (*debug)(fmt::format("tried to remove a non existing edge {} - {}", _data, neighbor->_data));
        }
    }
}

template<typename ID, typename... T>
inline Vertex<ID, T...>::~Vertex() = default;

template<typename ID, typename... T>
inline Vertex<ID, T...> &Vertex<ID, T...>::operator=(Vertex &&rhs) noexcept = default;

template<typename ID, typename... T>
inline Vertex<ID, T...>::Vertex(Vertex &&other) noexcept = default;

template<typename ID, typename... T>
inline Vertex<ID, T...>::Vertex(const graphs::Vertex<ID, T...> &) = default;

template<typename ID, typename... T>
inline Vertex<ID, T...>& Vertex<ID, T...>::operator=(const Vertex&) = default;

}

namespace fmt {
template <typename ID, typename... T>
struct formatter<graphs::Vertex<ID, T...>> {
    template <typename ParseContext>
    constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const graphs::Vertex<ID, T...> &v, FormatContext &ctx) {
        std::stringstream ss;
        bool first {true};
        for (const auto neighbor : v.neighbors()) {
            if(!first) {
                ss << ",";
            }
            ss << neighbor->data();
            first = false;
        }
        return format_to(ctx.out(), "Vertex[{}, neighbors=[{}]]", v.id(), ss.str());
    }
};
}
