//
// Created by mho on 10/28/19.
//

#include <catch2/catch.hpp>
#include <graphs/Graph.h>
#include <graphs/Vertex.h>
#include <iostream>

TEST_CASE("dummy") {
    graphs::Graph<graphs::Vertex> g;
    graphs::Vertex v;
    g.toRef(v);
}
