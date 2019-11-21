//
// Created by mho on 11/20/19.
//

#include <catch2/catch.hpp>
#include <graphs/IndexPersistentVector.h>

class A {
public:

    explicit A(int x) : x(x) {}

    void deactivate() {
        active = false;
    }

    bool deactivated() { return !active; }

    [[nodiscard]] auto val() const { return x; }

private:
    bool active {true};
    int x;
};

TEST_CASE("Test index persistent vector", "[ipv]") {

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
                std::vector<std::size_t> mapping {0, 1, 3, 4};
                auto ix = 0;
                for(auto it = v.begin_active(); it != v.end_active(); ++it, ++ix) {
                    REQUIRE(it->val() == v.at(mapping[ix]).val());
                }
                REQUIRE((*v.begin_active()).val() == 5);
                REQUIRE((*(v.begin_active()+1)).val() == 1);
                REQUIRE((*(v.begin_active()+2)).val() == 8);
                REQUIRE((*(v.begin_active()+3)).val() == 3);
            }
            AND_WHEN("Removing element number 1") {
                v.erase(v.begin() + 1);
                THEN("The active size is 3") {
                    REQUIRE(std::distance(v.begin_active(), v.end_active()) == 3);
                    REQUIRE(v.size_active() == 3);
                }
                AND_THEN("the active iterator skips the elements with numbers 7, 1") {
                    std::vector<std::size_t> mapping {0, 3, 4};
                    auto ix = 0;
                    for(auto it = v.begin_active(); it != v.end_active(); ++it, ++ix) {
                        REQUIRE(it->val() == v.at(mapping[ix]).val());
                    }
                    REQUIRE((*v.begin_active()).val() == 5);
                    REQUIRE((*(v.begin_active()+1)).val() == 8);
                    REQUIRE((*(v.begin_active()+2)).val() == 3);
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
                            THEN("The active size is 3") {
                                REQUIRE(std::distance(v.begin_active(), v.end_active()) == 3);
                                REQUIRE(v.size_active() == 3);
                            }
                            AND_THEN("The vector contains {8, 100}") {
                                REQUIRE(*v.begin_active() == )
                            }
                        }
                    }
                }
            }
        }
    }
}
