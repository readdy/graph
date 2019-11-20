add_executable (graphs_test
        main.cpp
        Graph.cpp
        Vertex.cpp
        IndexPersistentVector.cpp)
target_link_libraries(graphs_test graphs Catch2::Catch2)
catch_discover_tests(graphs_test)
