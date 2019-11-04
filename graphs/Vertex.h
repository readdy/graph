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

template<typename ID, typename... T>
class Vertex;

template<typename Vertex = graphs::Vertex<std::size_t>>
class Graph;

template<typename ID, typename... T>
class Vertex {
public:
    /**
     * edge in the graph (i.e., pointer to neighboring vertex)
     */
    using VertexPtr = typename std::list<Vertex>::iterator;
    using VertexCPtr = typename std::list<Vertex>::const_iterator;
    using id_type = ID;
    using data_type = std::tuple<T...>;

    Vertex();

    Vertex(data_type data);

    Vertex(const Vertex &);

    Vertex &operator=(const Vertex &);

    Vertex(Vertex && other) noexcept;

    Vertex &operator=(Vertex && rhs) noexcept;

    virtual ~Vertex();

    bool operator==(const Vertex &rhs) const;

    template <typename PID>
    friend std::ostream &operator<<(std::ostream &os, const Vertex<PID> &vertex);

    bool operator!=(const Vertex &rhs) const;

    const std::vector<VertexPtr> &neighbors() const;

    template<auto debug=nullptr>
    void addNeighbor(VertexPtr neighbor);

    template<auto debug=nullptr>
    void removeNeighbor(VertexPtr neighbor);

    const data_type &data() const;

    void setData(data_type data);

private:
    friend class graphs::Graph<graphs::Vertex<data_type>>;

    /**
     * the edges (i.e., pointers to neighboring vertices)
     */
    std::vector<VertexPtr> _neighbors{};
    mutable bool visited {false};

    data_type _data{};
};

}

#include "bits/Vertex_detail.h"
