cmake_minimum_required(VERSION 3.9)

if(DEFINED PROJECT_NAME)
    set(GRAPHS_IS_SUBPROJECT ON)
endif()

project(graphs LANGUAGES CXX VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)

set(GRAPHS_LIB_DIR "${CMAKE_CURRENT_LIST_DIR}/libs")

set(FMT_TEST OFF CACHE BOOL "" FORCE)
set(FMT_DOC OFF CACHE BOOL "" FORCE)

add_subdirectory(${GRAPHS_LIB_DIR}/fmt)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})
set(${PROJECT_NAME}_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/graphs/Graph.h
        ${CMAKE_CURRENT_LIST_DIR}/graphs/Vertex.h)
target_sources(${PROJECT_NAME} INTERFACE ${${PROJECT_NAME}_SOURCES})
target_link_libraries(${PROJECT_NAME} INTERFACE fmt::fmt-header-only)

if (NOT GRAPHS_IS_SUBPROJECT)
    option(${PROJECT_NAME}_BUILD_TESTING "Build ${PROJECT_NAME} tests" ON)
endif()

if (${PROJECT_NAME}_BUILD_TESTING)
    if (EXISTS ${GRAPHS_LIB_DIR}/Catch2/CMakeLists.txt)
        add_subdirectory(${GRAPHS_LIB_DIR}/Catch2)
        include(${GRAPHS_LIB_DIR}/Catch2/contrib/Catch.cmake)
    elseif (NOT TARGET Catch2::Catch2)
        find_package(Catch2 QUIET)
        if (CATCH2_FOUND)
            include(Catch)
        else()
            message(SEND_ERROR "Catch2 not found.")
        endif()
    endif ()
    include(CTest)
    add_subdirectory(test)
endif ()

