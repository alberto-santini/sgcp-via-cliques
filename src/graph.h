//
// Created by alberto on 08/05/18.
//

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <cstddef>
#include <string>

#ifndef SGCP_VIA_CLIQUES_GRAPH_H
#define SGCP_VIA_CLIQUES_GRAPH_H

namespace sgcp_cliques {
    using ClusteredGraph = boost::adjacency_list<
            boost::vecS, boost::vecS,
            boost::undirectedS, std::size_t,
            boost::no_property, std::size_t
    >;
    using LineGraph = boost::adjacency_list<
            boost::vecS, boost::vecS,
            boost::undirectedS, std::pair<std::size_t, std::size_t>
    >;
    using DirectedGraph = boost::adjacency_list<
            boost::vecS, boost::vecS,
            boost::directedS, std::size_t,
            boost::no_property, std::size_t
    >;

    ClusteredGraph read_clustered_graph(std::string graph_file);
    DirectedGraph directed_acyclic(const ClusteredGraph& cgraph);
    LineGraph line_graph(const ClusteredGraph& cgraph);
    LineGraph sandwich_line_graph(const ClusteredGraph& cgraph);
    LineGraph complementary_sandwich_line_graph(const ClusteredGraph& cgraph);
}

#endif //SGCP_VIA_CLIQUES_GRAPH_H
