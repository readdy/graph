//
// Created by mho on 10/28/19.
//

#include <iostream>
#include <tuple>
#include <utility>

#include <catch2/catch.hpp>

#include <graphs/Graph.h>
#include <graphs/Vertex.h>

auto debug = [](const std::string &str) {
    std::cerr << "DEBUG: " << str << std::endl;
};

template<typename T, size_t... I>
auto reverse_impl(T t, std::index_sequence<I...>) {
    return std::make_tuple(std::get<sizeof...(I) - 1 - I>(std::forward<T>(t))...);
}

template<typename T>
auto reverse(T t) {
    return reverse_impl(std::forward<T>(t), std::make_index_sequence<std::tuple_size<T>::value>());
}

std::size_t n_choose_k(std::size_t n, std::size_t k) {
    if(k == 0 || k == n) return 1;
    return n_choose_k(n - 1, k - 1) + n_choose_k(n - 1, k);
}

template<typename Tup>
std::size_t nTupleOccurrences(const std::vector<Tup> &v, const Tup &t) {
    std::size_t n = 0;
    auto tReverse = reverse<Tup>(t);
    for(auto it = std::begin(v); it != std::end(v); ++it) {
        if(*it == t || *it == tReverse) ++n;
    }
    return n;
}

template<typename Tup>
bool containsTupleXOR(const std::vector<Tup> &v, const Tup &t) {
    return nTupleOccurrences(v, t) == 1;
}

graphs::Graph<graphs::Vertex> fullyConnectedGraph(std::size_t size) {
    graphs::Graph<graphs::Vertex> graph;
    for(std::size_t i = 0; i < size; ++i) {
        graph.addVertex(i, 0);
    }
    for(auto i = graph.vertices().begin(); i != graph.vertices().end(); ++i) {
        for(auto j = std::next(i); j != graph.vertices().end(); ++j) {
            graph.addEdge(i, j);
        }
    }
    return graph;
}

template<std::size_t K>
void testFullyConnected() {
    GIVEN("A fully connected graph of size " + std::to_string(K)) {
        auto graph = fullyConnectedGraph(K);
        THEN("The number of vertices should be " + std::to_string(K)) {
            REQUIRE(graph.vertices().size() == K);
        }
        THEN("The number of neighbors per vertex should be " + std::to_string(K - 1)) {
            for(auto it = graph.vertices().begin(); it != graph.vertices().end(); ++it) {
                REQUIRE(it->neighbors().size() == K - 1);
            }
        }
        THEN("The number of edges should be 0.5n(n-1)") {
            REQUIRE(graph.nEdges() == static_cast<std::size_t>(K * (K - 1) / 2));
        }
        const auto &[tuples, triples, quadruples] = graph.findNTuples();
        THEN("The number of unique tuples should be 0.5n(n-1)") {
            REQUIRE(tuples.size() == static_cast<std::size_t>(K * (K - 1) / 2));
            for(auto i = graph.vertices().begin(); i != graph.vertices().end(); ++i) {
                for (auto j = std::next(i); j != graph.vertices().end(); ++j) {
                    REQUIRE(containsTupleXOR(tuples, std::make_tuple(i, j)));
                }
            }
        }
        THEN("The number of unique triples should be 3*(n choose 3)") {
            // Since the graph is fully connected (n choose 3) gives the number of different paths of length 3
            // in the graph. The number is in terms of sets, i.e., a path is defined via its vertices independent of
            // their order. For a set of vertices {v1, v2, v3} there are three different ways of enumeration
            //  - v1 v2 v3
            //  - v1 v3 v2
            //  - v2 v1 v3
            // which give distinct potential configurations.
            REQUIRE(triples.size() == 3*n_choose_k(K, 3));
            for(const auto &triple : triples) {
                CAPTURE(triple);
                REQUIRE(nTupleOccurrences(triples, triple) == 1);
            }
        }

        THEN("The number of unique quadruples should be 12*(n choose 4)") {
            // See number of unique triplets but this time there are 12 ways of enumeration:
            // (n choose 4) different paths of length 4, 4! * (n choose 4) different order dependent paths,
            // 0.5 * 4! * (n choose 4) = 12 * (n choose 4) paths where (v1 v2 v3 v4) == (v4 v3 v2 v1).
            REQUIRE(quadruples.size() == 12*n_choose_k(K, 4));

            for(const auto &quad : quadruples) {
                CAPTURE(quad);
                REQUIRE(nTupleOccurrences(quadruples, quad) == 1);
            }
        }
    }
}

SCENARIO("Testing graphs basic functionality", "[graphs]") {

    GIVEN("A graph with two vertices") {
        graphs::Graph<graphs::Vertex> graph;
        graph.addVertex(0, 0);
        graph.addVertex(1, 0);
        WHEN("connecting the two vertices") {
            graph.addEdge(graph.vertices().begin(), ++graph.vertices().begin());
            THEN("this should be reflected in the neighbors structure accessed by particle indices") {
                REQUIRE(graph.vertices().size() == 2);
                REQUIRE(graph.vertexForParticleIndex(0).particleIndex == 0);
                REQUIRE(graph.vertexForParticleIndex(1).particleIndex == 1);
                REQUIRE(graph.vertexForParticleIndex(0).neighbors().size() == 1);
                REQUIRE(graph.vertexForParticleIndex(1).neighbors().size() == 1);
                REQUIRE(graph.vertexForParticleIndex(0).neighbors()[0]->particleIndex == 1);
                REQUIRE(graph.vertexForParticleIndex(1).neighbors()[0]->particleIndex == 0);
            }
            WHEN("removing the first particle") {
                graph.removeParticle(0);
                THEN("the size of the graph is 1") {
                    REQUIRE(graph.vertices().size() == 1);
                    REQUIRE(graph.vertexForParticleIndex(1).particleIndex == 1);
                    REQUIRE(graph.vertexForParticleIndex(1).neighbors().empty());
                }
            }
        }
        WHEN("Adding a third vertex, connecting (0 -- 1), (2)") {
            graph.addVertex(2, 0);

            auto vertex_ref_0 = graph.vertices().begin();
            auto vertex_ref_1 = ++graph.vertices().begin();
            auto vertex_ref_2 = ++(++graph.vertices().begin());

            auto it = graph.vertices().begin();
            auto it_adv = ++graph.vertices().begin();
            graph.addEdge(it, it_adv);

            auto subGraphs = graph.connectedComponentsDestructive();
            THEN("There should be two connected components") {
                REQUIRE(subGraphs.size() == 2);
            }
            THEN("The two connected components should be (0 -- 1) and (2)") {
                {
                    REQUIRE(subGraphs[0].vertices().size() == 2);
                    REQUIRE(subGraphs[0].vertices().begin() == vertex_ref_0);
                    REQUIRE(++subGraphs[0].vertices().begin() == vertex_ref_1);
                }
                {
                    REQUIRE(subGraphs[1].vertices().size() == 1);
                    REQUIRE(subGraphs[1].vertices().begin() == vertex_ref_2);
                }
            }
        }
        WHEN("Adding two vertices and connecting (0 -- 1 -- 2 -- 3 -- 0)") {
            graph.addVertex(2, 0);
            graph.addVertex(3, 0);

            auto a = graph.firstVertex();
            auto b = std::next(graph.firstVertex());
            auto c = std::next(graph.firstVertex(), 2);
            auto d = std::next(graph.firstVertex(), 3);

            graph.addEdge(a, b);
            graph.addEdge(b, c);
            graph.addEdge(c, d);
            graph.addEdge(d, a);

            auto n_tuples = graph.findNTuples();
            const auto& tuples = std::get<0>(n_tuples);
            const auto& triples = std::get<1>(n_tuples);
            const auto& quadruples = std::get<2>(n_tuples);

            THEN("expect 4 unique tuples") {
                REQUIRE(tuples.size() == 4);

                REQUIRE(containsTupleXOR(tuples, std::make_tuple(a, b)));
                REQUIRE(containsTupleXOR(tuples, std::make_tuple(b, c)));
                REQUIRE(containsTupleXOR(tuples, std::make_tuple(c, d)));
                REQUIRE(containsTupleXOR(tuples, std::make_tuple(d, a)));
            }
            THEN("expect 4 unique triples") {
                REQUIRE(triples.size() == 4);

                REQUIRE(containsTupleXOR(triples, std::make_tuple(a, b, c)));
                REQUIRE(containsTupleXOR(triples, std::make_tuple(b, c, d)));
                REQUIRE(containsTupleXOR(triples, std::make_tuple(c, d, a)));
                REQUIRE(containsTupleXOR(triples, std::make_tuple(d, a, b)));
            }
            THEN("expect 4 unique quadruples") {
                REQUIRE(quadruples.size() == 4);

                REQUIRE(containsTupleXOR(quadruples, std::make_tuple(d, a, b, c)));
                REQUIRE(containsTupleXOR(quadruples, std::make_tuple(a, b, c, d)));
                REQUIRE(containsTupleXOR(quadruples, std::make_tuple(b, c, d, a)));
                REQUIRE(containsTupleXOR(quadruples, std::make_tuple(c, d, a, b)));
            }
        }

        SECTION("Adding one vertex and connecting (0 -- 1 -- 2 -- 0)") {
            graph.addVertex(2, 0);

            auto a = graph.firstVertex();
            auto b = std::next(graph.firstVertex());
            auto c = std::next(std::next(graph.firstVertex()));

            graph.addEdge(a, b);
            graph.addEdge(b, c);
            graph.addEdge(c, a);

            auto n_tuples = graph.findNTuples();
            const auto& tuples = std::get<0>(n_tuples);
            const auto& triples = std::get<1>(n_tuples);
            const auto& quadruples = std::get<2>(n_tuples);

            THEN("Expect 3 unique tuples") {
                REQUIRE(tuples.size() == 3);
                REQUIRE(containsTupleXOR(tuples, std::make_tuple(a, b)));
                REQUIRE(containsTupleXOR(tuples, std::make_tuple(b, c)));
                REQUIRE(containsTupleXOR(tuples, std::make_tuple(c, a)));
            }

            THEN("Expect 3 unique triples") {
                REQUIRE(triples.size() == 3);
                REQUIRE(containsTupleXOR(triples, std::make_tuple(a, b, c)));
                REQUIRE(containsTupleXOR(triples, std::make_tuple(b, c, a)));
                REQUIRE(containsTupleXOR(triples, std::make_tuple(c, a, b)));
            }

            THEN("Expect 0 unique quadruples") {
                REQUIRE(quadruples.empty());
            }
        }
    }

    testFullyConnected<5>();
    testFullyConnected<7>();

}
