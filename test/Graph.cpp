//
// Created by mho on 10/28/19.
//

#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

#include <catch2/catch.hpp>

#include <graphs/graphs.h>

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

template<typename T1, typename T2>
std::size_t nTupleOccurrences(const std::vector<T1> &v, const T2 &t) {
    std::size_t n = 0;
    auto tReverse = reverse<T2>(t);
    for(auto it = std::begin(v); it != std::end(v); ++it) {
        if(*it == t || *it == tReverse) ++n;
    }
    return n;
}

template<typename T1, typename T2>
bool containsTupleXOR(const std::vector<T1> &v, const T2 &t) {
    return nTupleOccurrences(v, t) == 1;
}

graphs::DefaultGraph fullyConnectedGraph(std::size_t size) {
    graphs::DefaultGraph graph;
    for(std::size_t i = 0; i < size; ++i) {
        graph.addVertex(i);
    }
    for(std::size_t i = 0; i < graph.vertices().size(); ++i) {
        for(std::size_t j = i+1; j < graph.vertices().size(); ++j) {
            graph.addEdge(i, j);
        }
    }
    return graph;
}

template<std::size_t K>
auto quadruplesFullyConnectedPrimitive() {
    std::vector<std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>> quads;

    for(std::size_t i = 0; i < K; ++i){
        for(std::size_t j = 0; j < K; ++j){
            for(std::size_t k = 0; k < K; ++k){
                for(std::size_t l = 0; l < K; ++l){
                    std::array<std::size_t, 4> arr {i, j, k, l};
                    std::sort(std::begin(arr), std::end(arr));
                    auto pos = std::adjacent_find(std::begin(arr), std::end(arr));
                    if( pos == std::end(arr) ) {
                        // only true if no duplicates
                        auto tup = std::make_tuple(i, j, k, l);
                        if (!containsTupleXOR(quads, tup)) {
                            quads.push_back(tup);
                        }
                    }
                }
            }
        }
    }

    return quads;
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
        const auto &[pairs, triples, quadruples] = graph.findNTuples();
        THEN("The number of unique pairs should be 0.5n(n-1)") {
            REQUIRE(pairs.size() == static_cast<std::size_t>(K * (K - 1) / 2));
            for(std::size_t i = 0; i < graph.vertices().size(); ++i) {
                for (std::size_t j = i+1; j < graph.vertices().size(); ++j) {
                    REQUIRE(containsTupleXOR(pairs, std::make_tuple(i, j)));
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
            auto quadsPrimitive = quadruplesFullyConnectedPrimitive<K>();
            REQUIRE(quadsPrimitive.size() == 12*n_choose_k(K, 4));
            REQUIRE(quadruples.size() == 12*n_choose_k(K, 4));

            for(const auto &quad : quadruples) {
                const auto& [v1, v2, v3, v4] = quad;
                CAPTURE(quad);
                REQUIRE(nTupleOccurrences(quadruples, quad) == 1);
                REQUIRE(nTupleOccurrences(quadsPrimitive, std::make_tuple(v1, v2, v3, v4)) == 1);
            }
        }
    }
}

SCENARIO("Testing graphs basic functionality", "[graphs]") {

    GIVEN("Some tuples (A, B, C, D), (D, C, B, A), (A, C, B, D)") {
        auto t1 = std::make_tuple(0, 1, 2, 3);
        auto t2 = std::make_tuple(3, 2, 1, 0);
        auto t3 = std::make_tuple(0, 2, 1, 3);
        auto vec = std::vector{t1, t2, t3};
        THEN("the number of occurrences is 2, 2, 1") {
            REQUIRE(nTupleOccurrences(vec, t1) == 2);
            REQUIRE(nTupleOccurrences(vec, t2) == 2);
            REQUIRE(nTupleOccurrences(vec, t3) == 1);
        }
        THEN("containsTupleXOR is false, false, true") {
            REQUIRE_FALSE(containsTupleXOR(vec, t1));
            REQUIRE_FALSE(containsTupleXOR(vec, t2));
            REQUIRE(containsTupleXOR(vec, t3));
        }
    }

    GIVEN("A graph with two vertices") {
        graphs::Graph<graphs::Vertex<std::size_t>> graph;
        graph.addVertex(0);
        graph.addVertex(1);
        WHEN("connecting the two vertices") {
            graph.addEdge(0, 1);
            THEN("this should be reflected in the neighbors structure accessed by particle indices") {
                REQUIRE(graph.isConnected());
                REQUIRE(graph.vertices().size() == 2);
                REQUIRE(graph.vertices().at(0).neighbors().size() == 1);
                REQUIRE(graph.vertices().at(1).neighbors().size() == 1);
                REQUIRE(graph.vertices().at(0).neighbors()[0] == 1);
                REQUIRE(graph.vertices().at(1).neighbors()[0] == 0);
            }
            WHEN("removing the first particle") {
                graph.removeVertex(0);
                THEN("the size of the graph is 1") {
                    REQUIRE(graph.vertices().size() == 1);
                    REQUIRE(graph.vertices().at(1).neighbors().empty());
                    REQUIRE(graph.vertices().at(1).data() == std::make_tuple(1));
                    REQUIRE(graph.nEdges() == 0);
                }
            }
        }
        WHEN("Adding a third vertex, connecting (0 -- 1), (2)") {
            graph.addVertex(2);

            graph.addEdge(0, 1);

            REQUIRE_FALSE(graph.isConnected());

            auto subGraphs = graph.connectedComponents();
            THEN("There should be two connected components") {
                REQUIRE(subGraphs.size() == 2);
            }
            THEN("The two connected components should be (0 -- 1) and (2)") {
                {
                    REQUIRE(subGraphs[0].vertices().size() == 2);
                    REQUIRE(subGraphs[0].vertices().begin()->data() == std::make_tuple(0));
                    REQUIRE((++subGraphs[0].vertices().begin())->data() == std::make_tuple(1));
                }
                {
                    REQUIRE(subGraphs[1].vertices().size() == 1);
                    REQUIRE(subGraphs[1].vertices().begin()->data() == std::make_tuple(2));
                }
            }
        }
        WHEN("Adding two vertices and connecting (0 -- 1 -- 2 -- 3 -- 0)") {
            graph.addVertex(2);
            graph.addVertex(3);

            graph.addEdge(0, 1);
            graph.addEdge(1, 2);
            graph.addEdge(2, 3);
            graph.addEdge(3, 0);

            REQUIRE(graph.isConnected());

            auto n_tuples = graph.findNTuples();
            const auto& tuples = std::get<0>(n_tuples);
            const auto& triples = std::get<1>(n_tuples);
            const auto& quadruples = std::get<2>(n_tuples);

            THEN("expect 4 unique tuples") {
                REQUIRE(tuples.size() == 4);

                REQUIRE(containsTupleXOR(tuples, std::make_tuple(0, 1)));
                REQUIRE(containsTupleXOR(tuples, std::make_tuple(1, 2)));
                REQUIRE(containsTupleXOR(tuples, std::make_tuple(2, 3)));
                REQUIRE(containsTupleXOR(tuples, std::make_tuple(3, 0)));
            }
            THEN("expect 4 unique triples") {
                REQUIRE(triples.size() == 4);

                REQUIRE(containsTupleXOR(triples, std::make_tuple(0, 1, 2)));
                REQUIRE(containsTupleXOR(triples, std::make_tuple(1, 2, 3)));
                REQUIRE(containsTupleXOR(triples, std::make_tuple(2, 3, 0)));
                REQUIRE(containsTupleXOR(triples, std::make_tuple(3, 0, 1)));
            }
            THEN("expect 4 unique quadruples") {
                REQUIRE(quadruples.size() == 4);

                REQUIRE(containsTupleXOR(quadruples, std::make_tuple(3, 0, 1, 2)));
                REQUIRE(containsTupleXOR(quadruples, std::make_tuple(0, 1, 2, 3)));
                REQUIRE(containsTupleXOR(quadruples, std::make_tuple(1, 2, 3, 0)));
                REQUIRE(containsTupleXOR(quadruples, std::make_tuple(2, 3, 0, 1)));
            }
        }

        WHEN("Adding one vertex and connecting (0 -- 1 -- 2 -- 0)") {
            graph.addVertex(2);

            auto a = 0;
            auto b = 1;
            auto c = 2;

            graph.addEdge(a, b);
            graph.addEdge(b, c);
            graph.addEdge(c, a);

            auto n_tuples = graph.findNTuples();
            const auto& pairs = std::get<0>(n_tuples);
            const auto& triples = std::get<1>(n_tuples);
            const auto& quadruples = std::get<2>(n_tuples);

            REQUIRE(graph.isConnected());

            THEN("Expect 3 unique pairs") {
                REQUIRE(pairs.size() == 3);
                REQUIRE(containsTupleXOR(pairs, std::make_tuple(a, b)));
                REQUIRE(containsTupleXOR(pairs, std::make_tuple(b, c)));
                REQUIRE(containsTupleXOR(pairs, std::make_tuple(c, a)));
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
    testFullyConnected<8>();

    GIVEN("A fully connected graph g1 of size 5 and a fully connected graph g2 of size 4") {
        auto g1 = fullyConnectedGraph(5);
        auto g1Copy = g1;
        auto g2 = fullyConnectedGraph(4);

        THEN("All graphs are connected") {
            REQUIRE(g1.isConnected());
            REQUIRE(g1Copy.isConnected());
            REQUIRE(g2.isConnected());
        }

        WHEN("Appending g2 to g1") {
            g1.append(g2);
            THEN("g1 becomes disconnected, while g2 is still connected") {
                REQUIRE_FALSE(g1.isConnected());
                REQUIRE(g2.isConnected());
            }
            THEN("The number of vertices in g1 is 5+4") {
                REQUIRE(g1.vertices().size() == 9);
            }
            THEN("The number of edges is the sum of the number of edges in g1 and g2") {
                REQUIRE(g1.nEdges() == g1Copy.nEdges() + g2.nEdges());
            }
            AND_WHEN("taking the connected components of the joint graph") {
                auto components = g1.connectedComponents();
                THEN("we get back the original g1 and g2") {
                    REQUIRE(components.size() == 2);
                    REQUIRE((components[0].vertices().size() == 5 ^ components[0].vertices().size() == 4) == true);
                    REQUIRE((components[1].vertices().size() == 5 ^ components[1].vertices().size() == 4) == true);
                    if(components[0].vertices().size() == 5) {
                        // components[0] is g1
                        REQUIRE(components[0].nEdges() == g1Copy.nEdges());
                        REQUIRE(components[1].nEdges() == g2.nEdges());
                    } else {
                        // components[0] is g2
                        REQUIRE(components[0].nEdges() == g2.nEdges());
                        REQUIRE(components[1].nEdges() == g1Copy.nEdges());
                    }
                }
            }
        }
        WHEN("Removing a vertex from graph g1") {
            g1.removeVertex(3);
            THEN("The graph is a fully connected graph with 4 vertices") {
                REQUIRE(g1.vertices().size() == 4);
                REQUIRE(g1.nEdges() == g2.nEdges());
                for(const auto&[v1, v2] : g1.edges()) {
                    REQUIRE(!g1.vertices().at(v1).deactivated());
                    REQUIRE(!g1.vertices().at(v2).deactivated());
                    REQUIRE(v1 != v2);
                }
            }

            AND_WHEN("Removing a second vertex from g1") {
                g1.removeVertex(2);
                THEN("The graph is a fully connected graph with 3 vertices") {
                    REQUIRE(g1.vertices().size() == 3);
                    REQUIRE(g1.nEdges() == g2.nEdges() - 3);
                }

                AND_WHEN("Removing a third vertex from g1") {
                    g1.removeVertex(1);
                    THEN("The graph is a fully connected graph with 2 vertices") {
                        REQUIRE(g1.vertices().size() == 2);
                        REQUIRE(g1.nEdges() == g2.nEdges() - 3 - 2);
                        for(auto [i1, i2] : g1.edges()) {
                            const auto &v1 = g1.vertices().at(i1);
                            const auto &v2 = g1.vertices().at(i2);
                            REQUIRE(!v1.deactivated());
                            REQUIRE(!v2.deactivated());
                            REQUIRE(std::find(v1.neighbors().begin(), v1.neighbors().end(), i2) != v1.neighbors().end());
                            REQUIRE(std::find(v2.neighbors().begin(), v2.neighbors().end(), i1) != v2.neighbors().end());
                        }
                    }
                }

                AND_WHEN("Appending g2 to g2 forming an edge between 0 and 3") {
                    auto mapping = g1.append(g2, 0, 3);
                    THEN("the resulting graph is connected") {
                        REQUIRE(g1.isConnected());
                    }
                    AND_THEN("the graph has the edge [0, mapping(3)]") {
                        g1.containsEdge(0, mapping.at(3));
                    }
                    AND_WHEN("Removing the edge [0, mapping(3)]") {
                        g1.removeEdge(0, mapping.at(3));
                        THEN("There are two connected components") {
                            REQUIRE(!g1.isConnected());
                            REQUIRE(g1.connectedComponents().size() == 2);
                        }
                        AND_THEN("One component is fully connected with 3 vertices, one with 4 vertices") {
                            auto components = g1.connectedComponents();
                            const auto &gg1 = components.at(0).vertices().size() == 3 ? components.at(0)
                                    : components.at(1);
                            const auto &gg2 = components.at(0).vertices().size() == 3 ? components.at(1)
                                    : components.at(0);
                            REQUIRE(gg1.vertices().size() == 3);
                            REQUIRE(gg1.nEdges() == static_cast<std::size_t>(3 * (3 - 1) / 2));
                            REQUIRE(gg2.vertices().size() == 4);
                            REQUIRE(gg2.nEdges() == g2.nEdges());
                        }
                    }
                }
            }

            AND_WHEN("Appending g2") {
                g1.append(g2);
                THEN("There are two connected components, fully connected graphs with 4 vertices each") {
                    REQUIRE(g1.nEdges() == 2*g2.nEdges());
                    auto components = g1.connectedComponents();
                    REQUIRE(components.size() == 2);
                    REQUIRE(components[0].vertices().size() == 4);
                    REQUIRE(components[1].vertices().size() == 4);
                    REQUIRE(components[0].nEdges() == g2.nEdges());
                    REQUIRE(components[1].nEdges() == g2.nEdges());

                    AND_THEN("The edges list should be consistent with the vertex neighbors") {
                        for(const auto &component : components) {
                            std::size_t nActive = 0;
                            for(std::size_t i = 0; i < component.vertices().size(); ++i) {
                                if(!component.vertices().at(i).deactivated()) {

                                    for(auto neighborIndex : component.vertices().at(i).neighbors()) {
                                        REQUIRE(!component.vertices().at(neighborIndex).deactivated());
                                        auto it = std::find_if(std::begin(component.edges()),
                                                std::end(component.edges()), [i, neighborIndex](const auto& edge) {
                                                    auto [i1, i2] = edge;
                                                    return (i1 == i && i2 == neighborIndex) ||
                                                        (i1 == neighborIndex && i2 == i);
                                        });
                                        REQUIRE(it != component.edges().end());
                                    }

                                    ++nActive;
                                }
                            }
                            REQUIRE(nActive == 4);
                        }
                    }
                }
            }
        }
    }
}
