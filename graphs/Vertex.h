//
// Created by mho on 10/28/19.
//

#pragma once

#include <list>
#include <ostream>
#include <vector>
#include <sstream>

#include <fmt/format.h>

namespace graphs {

template<typename... T>
class Vertex {
public:
    using data_type = std::tuple<T...>;
    using size_type = typename std::vector<Vertex<T...>>::size_type;
    using NeighborList = std::vector<size_type>;

    Vertex(T&&... data);

    Vertex(data_type data);

    Vertex(const Vertex &);

    Vertex &operator=(const Vertex &);

    Vertex(Vertex && other) noexcept;

    Vertex &operator=(Vertex && rhs) noexcept;

    virtual ~Vertex();

    template<typename... T2>
    friend std::ostream &operator<<(std::ostream &os, const Vertex<T2...> &vertex);

    const NeighborList &neighbors() const;

    NeighborList &neighbors();

    template<auto debug=nullptr>
    void addNeighbor(size_type neighbor);

    template<auto debug=nullptr>
    void removeNeighbor(size_type neighbor);

    const auto &data() const;

    void setData(data_type data);

    void deactivate();

    bool deactivated() const;

private:
    NeighborList _neighbors{};
    data_type _data;
    bool _deactivated {false};
};

}

#include "bits/Vertex_detail.h"
