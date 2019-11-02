//
// Created by mho on 10/28/19.
//

#include <catch2/catch.hpp>
#include <graphs/Graph.h>
#include <graphs/Vertex.h>
#include <iostream>

auto debug = [](const std::string &str) {
    std::cerr << "DEBUG: " << str << std::endl;
};

TEST_CASE("dummy") {
    graphs::Graph<graphs::Vertex> g;
    g.addVertex(0, 0);
    g.addVertex(1, 0);
    CHECK_FALSE(g.isConnected());
    g.addVertexNeighbor<&debug>(*g.firstVertex(), g.lastVertex());
    CHECK(g.isConnected());
    g.addVertexNeighbor<&debug>(*g.firstVertex(), g.lastVertex());
}
