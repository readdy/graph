//
// Created by mho on 11/20/19.
//

#include <catch2/catch.hpp>
#include <graphs/IndexPersistentVector.h>
#include <set>

class A {
public:

    explicit A(int x) : x(x) {}

    void deactivate() {
        active = false;
    }

    bool deactivated() { return !active; }

    [[nodiscard]] auto val() const { return x; }

    bool operator==(const A &other) const {
        return other.active == active && other.x == x;
    }

    bool operator<(const A &other) const {
        return other.x < x;
    }

private:
    bool active {true};
    int x;
};

bool randomBool() {
    static auto gen = std::bind(std::uniform_int_distribution<>(0,1),std::default_random_engine());
    return gen();
}

TEST_CASE("IPV usage test", "[ipv]") {
    std::vector<A> elems;
    graphs::IndexPersistentVector<A> v;

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 600000);

    for(int i = 0; i < 10000; ++i) {
        REQUIRE(elems.size() == v.size_active());
        if(randomBool()) {
            // add a random element
            auto n = dist(rng);
            v.push_back(A(n));
            elems.emplace_back(n);
        } else {
            if(!elems.empty()) {
                std::uniform_int_distribution<std::mt19937::result_type> draw(0, elems.size()-1);
                auto it = v.begin_active() + draw(rng);
                auto val = it->val();
                v.erase(it);
                auto findit = std::find_if(elems.begin(), elems.end(), [val](auto a) { return a.val() == val; });
                REQUIRE(findit != elems.end());
                elems.erase(findit);
            }
        }

        for(auto a : elems) {
            REQUIRE(std::find(v.begin_active(), v.end_active(), a) != v.end_active());
        }
        for(auto it = v.begin_active(); it != v.end_active(); ++it) {
            REQUIRE(std::find(elems.begin(), elems.end(), *it) != elems.end());
        }
    }
}

SCENARIO("Test ipv active iterator", "[ipv]") {
    GIVEN("A IPV with five elements") {
        graphs::IndexPersistentVector<A> v;
        v.push_back(A(5));
        v.push_back(A(1));
        v.push_back(A(7));
        v.push_back(A(8));
        v.push_back(A(3));
        THEN("The distance between active begin and active end is 5") {
            REQUIRE(std::distance(v.begin_active(), v.end_active()) == 5);
        }

        WHEN("Removing element number 2") {
            v.erase(v.begin() + 2);
            THEN("The active size is 4") {
                REQUIRE(std::distance(v.begin_active(), v.end_active()) == 4);
                REQUIRE(v.size_active() == 4);
            }
            AND_THEN("the active iterator skips the element with number 7") {
                WHEN("Accessing it through random access") {
                    std::vector<std::size_t> mapping{0, 1, 3, 4};
                    auto ix = 0;
                    for (auto it = v.begin_active(); it != v.end_active(); ++it, ++ix) {
                        REQUIRE(it->val() == v.at(mapping[ix]).val());
                    }
                }
                WHEN("Accessing it through plus operator") {
                    REQUIRE((*v.begin_active()).val() == 5);
                    REQUIRE((*(v.begin_active() + 1)).val() == 1);
                    REQUIRE((*(v.begin_active() + 2)).val() == 8);
                    REQUIRE((*(v.begin_active() + 3)).val() == 3);
                }
                WHEN("Accessing it through minus operator") {
                    REQUIRE((v.end_active() - 1)->val() == 3);
                    REQUIRE((v.end_active() - 2)->val() == 8);
                    REQUIRE((v.end_active() - 3)->val() == 1);
                    REQUIRE((v.end_active() - 4)->val() == 5);
                }
                WHEN("Accessing it through stepwise increase") {
                    auto itBegin = v.begin_active();
                    REQUIRE((itBegin++)->val() == 5);
                    REQUIRE((itBegin++)->val() == 1);
                    REQUIRE((itBegin++)->val() == 8);
                    REQUIRE((itBegin++)->val() == 3);
                    REQUIRE(itBegin == v.end_active());
                }
                WHEN("Accessing it through stepwise decrease") {
                    auto itEnd = v.end_active();
                    REQUIRE((--itEnd)->val() == 3);
                    REQUIRE((--itEnd)->val() == 8);
                    REQUIRE((--itEnd)->val() == 1);
                    REQUIRE((--itEnd)->val() == 5);
                    REQUIRE(itEnd == v.begin_active());
                }
            }
            AND_WHEN("Removing element number 1") {
                v.erase(v.begin() + 1);
                THEN("The active size is 3") {
                    REQUIRE(std::distance(v.begin_active(), v.end_active()) == 3);
                    REQUIRE(v.size_active() == 3);
                }
                AND_THEN("the active iterator skips the elements with numbers 7, 1") {
                    WHEN("Accessing it through random access") {
                        std::vector<std::size_t> mapping{0, 3, 4};
                        auto ix = 0;
                        for (auto it = v.begin_active(); it != v.end_active(); ++it, ++ix) {
                            REQUIRE(it->val() == v.at(mapping[ix]).val());
                        }
                    }
                    WHEN("Accessing it through plus operator") {
                        REQUIRE((*v.begin_active()).val() == 5);
                        REQUIRE((*(v.begin_active() + 1)).val() == 8);
                        REQUIRE((*(v.begin_active() + 2)).val() == 3);
                    }
                    WHEN("Accessing it through minus operator") {
                        REQUIRE((v.end_active() - 1)->val() == 3);
                        REQUIRE((v.end_active() - 2)->val() == 8);
                        REQUIRE((v.end_active() - 3)->val() == 5);
                    }
                    WHEN("Accessing it through stepwise increase") {
                        auto it = v.begin_active();
                        REQUIRE((it++)->val() == 5);
                        REQUIRE((it++)->val() == 8);
                        REQUIRE((it++)->val() == 3);
                        REQUIRE(it == v.end_active());
                    }
                    WHEN("Accessing it through stepwise decrease") {
                        auto itEnd = v.end_active();
                        REQUIRE((--itEnd)->val() == 3);
                        REQUIRE((--itEnd)->val() == 8);
                        REQUIRE((--itEnd)->val() == 5);
                        REQUIRE(itEnd == v.begin_active());
                    }
                }

                AND_WHEN("Removing element number 0") {
                    v.erase(v.begin());
                    THEN("The active size is 2") {
                        REQUIRE(std::distance(v.begin_active(), v.end_active()) == 2);
                        REQUIRE(v.size_active() == 2);
                    }

                    AND_THEN("the active iterator skips the elements with numbers 7, 1, 5") {
                        std::vector<std::size_t> mapping {3, 4};
                        auto ix = 0;
                        for(auto it = v.begin_active(); it != v.end_active(); ++it, ++ix) {
                            REQUIRE(it->val() == v.at(mapping[ix]).val());
                        }
                        REQUIRE((*(v.begin_active()+0)).val() == 8);
                        REQUIRE((*(v.begin_active()+1)).val() == 3);
                    }

                    AND_WHEN("Removing element 4") {
                        v.erase(v.begin() + 4);
                        THEN("The active size is 1") {
                            REQUIRE(std::distance(v.begin_active(), v.end_active()) == 1);
                            REQUIRE(v.size_active() == 1);
                        }
                        AND_THEN("the active iterator skips the elements with numbers 7, 1, 5, 3") {
                            std::vector<std::size_t> mapping {3};
                            auto ix = 0;
                            for(auto it = v.begin_active(); it != v.end_active(); ++it, ++ix) {
                                REQUIRE(it->val() == v.at(mapping[ix]).val());
                            }
                            REQUIRE((*(v.begin_active()+0)).val() == 8);
                        }

                        AND_WHEN("Removing element 3") {
                            v.erase(v.begin() + 3);
                            THEN("The vector is empty") {
                                REQUIRE(v.empty_active());
                                REQUIRE(v.size_active() == 0);
                            }
                            AND_THEN("the active iterator skips all the elements") {
                                REQUIRE(v.begin_active() == v.end_active());
                            }
                        }

                        AND_WHEN("Adding a new element 100") {
                            v.push_back(A{100});
                            THEN("The active size is 2") {
                                REQUIRE(v.size_active() == 2);
                                REQUIRE(std::distance(v.begin_active(), v.end_active()) == 2);
                            }
                            AND_THEN("The vector contains {8, 100}") {
                                auto it8 = std::find_if(v.begin_active(), v.end_active(),
                                                        [](auto a) { return a.val() == 8; });
                                REQUIRE(!it8->deactivated());
                                REQUIRE(it8 != v.end_active());
                                auto it100 = std::find_if(v.begin_active(), v.end_active(),
                                                         [](auto a) { return a.val() == 100; });
                                REQUIRE(!it100->deactivated());
                                REQUIRE(it100 != v.end_active());
                            }
                        }
                    }
                }
            }
        }
    }
}
