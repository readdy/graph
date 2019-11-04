//
// Created by mho on 11/4/19.
//

#include <catch2/catch.hpp>
#include <graphs/Vertex.h>

TEST_CASE("Test Vertex class", "[vertex]") {
    SECTION("With with default ctor") {
        graphs::Vertex vertex;
        REQUIRE(vertex.particleIndex() == 0);
        REQUIRE(vertex.particleType() == 0);
        REQUIRE(vertex.neighbors().empty());
    }

    SECTION("With non-default ctor") {
        graphs::Vertex vertex2 (5, 9);
        REQUIRE(vertex2.particleIndex() == 5);
        REQUIRE(vertex2.particleType() == 9);
        REQUIRE(vertex2.neighbors().empty());
    }

    SECTION("Vertices with neighbors") {
        std::list<graphs::Vertex> vertices {
                {0, 0}, {1, 0}, {2, 0}
        };
        auto &vertex1 = *vertices.begin();
        REQUIRE(vertex1.neighbors().empty());
        
        auto neighbor1 = std::next(std::begin(vertices));
        auto neighbor2 = std::next(std::begin(vertices), 2);

        WHEN("adding a neighbor") {
            vertex1.addNeighbor(neighbor1);
            THEN("the number of neighbors increases") {
                REQUIRE(vertex1.neighbors().size() == 1);
            }
            WHEN("adding that neighbor again") {
                vertex1.addNeighbor(neighbor1);
                THEN("the number of neighbors does not increase") {
                    REQUIRE(vertex1.neighbors().size() == 1);
                }
            }

            WHEN("adding a second neighbor") {
                vertex1.addNeighbor(neighbor2);
                THEN("the number of neighbors is 2") {
                    REQUIRE(vertex1.neighbors().size() == 2);
                }
                WHEN("removing the first neighbor") {
                    vertex1.removeNeighbor(neighbor1);
                    THEN("only the second neighbor remains") {
                        REQUIRE(vertex1.neighbors().size() == 1);
                        REQUIRE(vertex1.neighbors().front() == neighbor2);
                    }
                    WHEN("removing the first neighbor again") {
                        vertex1.removeNeighbor(neighbor1);
                        THEN("nothing happens") {
                            REQUIRE(vertex1.neighbors().size() == 1);
                            REQUIRE(vertex1.neighbors().front() == neighbor2);
                        }
                    }
                    WHEN("also removing the second neighbor") {
                        vertex1.removeNeighbor(neighbor2);
                        THEN("no neighbors remain") {
                            REQUIRE(vertex1.neighbors().empty());
                        }
                    }
                }
            }
        }
    }

}
