cmake_minimum_required(VERSION 3.9)

if(DEFINED PROJECT_NAME)
    set(CRAMPL_IS_SUBPROJECT ON)
endif()

project(crampl LANGUAGES CXX VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)

add_library(crampl INTERFACE)
target_include_directories(crampl INTERFACE ${CMAKE_CURRENT_LIST_DIR})
set(CRAMPL_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/crampl/MemberComparator.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/NonTypeList.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/PointerRange.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/ForEachMember.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/MultiKeyMap.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/ContainerContainer.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/Index.h
        ${CMAKE_CURRENT_LIST_DIR}/crampl/crampl.h)
target_sources(crampl INTERFACE ${CRAMPL_SOURCES})

if (NOT CRAMPL_IS_SUBPROJECT)
    option(CRAMPL_BUILD_TESTING "Build crampl tests" ON)
endif()

if (CRAMPL_BUILD_TESTING)
    if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/contrib/Catch2/CMakeLists.txt)
        add_subdirectory(contrib/Catch2)
        include(${CMAKE_CURRENT_LIST_DIR}/contrib/Catch2/contrib/Catch.cmake)
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

