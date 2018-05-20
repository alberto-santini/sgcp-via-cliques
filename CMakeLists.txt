cmake_minimum_required(VERSION 3.7.1)
project(sgcp_cliques VERSION 0.0.1 LANGUAGES CXX)

set(SOURCES src/graph.cpp src/graph.h src/main.cpp)
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(sgcp_cliques ${SOURCES})

set(WARNING_OPTIONS      -Wall -Wextra -Werror -Wduplicated-cond -Wlogical-op -Wuseless-cast -Wrestrict)
set(ARCH_OPTIONS         -mtune=native -march=native)

set(MATH_OPTIONS         -fassociative-math -freciprocal-math -fno-signed-zeros -fno-trapping-math)
set(OPTIMISATION_OPTIONS -frename-registers -funroll-loops -Ofast)
set(RELEASE_OPTIONS      ${MATH_OPTIONS} ${OPTIMISATION_OPTIONS})

set(DEBUG_OPTIONS        -O0 -ggdb)

set(LINKER_OPTIONS       -flto -Wl,--no-as-needed)

find_package(Boost REQUIRED)
find_package(AS REQUIRED)
find_package(Threads REQUIRED)
find_package(Cplex REQUIRED)

target_include_directories( sgcp_cliques PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(    sgcp_cliques PRIVATE cxx_std_17)
target_compile_options(     sgcp_cliques PRIVATE ${WARNING_OPTIONS} ${ARCH_OPTIONS})
target_compile_options(     sgcp_cliques PRIVATE $<$<CONFIG:RELEASE>:${RELEASE_OPTIONS}>)
target_compile_options(     sgcp_cliques PRIVATE $<$<CONFIG:DEBUG>:${DEBUG_OPTIONS}>)
target_link_libraries(      sgcp_cliques PRIVATE ${LINKER_OPTIONS})
target_link_libraries(      sgcp_cliques PRIVATE Boost::boost)
target_link_libraries(      sgcp_cliques PRIVATE Cplex::cplex)
target_link_libraries(      sgcp_cliques PRIVATE AS::AS)
target_link_libraries(      sgcp_cliques PRIVATE Threads::Threads)
target_link_libraries(      sgcp_cliques PRIVATE stdc++fs)
target_link_libraries(      sgcp_cliques PRIVATE dl)