cmake_minimum_required(VERSION 3.9)

if(DEFINED PROJECT_NAME)
    set(GRAPHS_IS_SUBPROJECT ON)
endif()

project(graphs LANGUAGES CXX VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR})
set(${PROJECT_NAME}_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/crampl/MemberComparator.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/NonTypeList.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/PointerRange.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/ForEachMember.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/MultiKeyMap.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/ContainerContainer.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/Index.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/crampl.h)
target_sources(${PROJECT_NAME} INTERFACE ${${PROJECT_NAME}_SOURCES})

if (NOT ${PROJECT_NAME}_IS_SUBPROJECT)
    option(${PROJECT_NAME}_BUILD_TESTING "Build ${PROJECT_NAME} tests" ON)
endif()

if (${PROJECT_NAME}_BUILD_TESTING)
    if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/libs/Catch2/CMakeLists.txt)
        add_subdirectory(libs/Catch2)
        include(${CMAKE_CURRENT_LIST_DIR}/libs/Catch2/contrib/Catch.cmake)
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

