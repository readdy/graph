//
// Created by mho on 11/20/19.
//

#include <catch2/catch.hpp>
#include <graphs/IndexPersistentVector.h>

class A {
public:
    void deactivate() {
        active = false;
    }

private:
    bool active {true};
};

TEST_CASE("Test index persistent vector", "[ipv]") {

}

TEST_CASE("Test ipv active iterator", "[ipv]") {
    graphs::IndexPersistentVector<A> v;
    auto it = v.begin_active();
}
