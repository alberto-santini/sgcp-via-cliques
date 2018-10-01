//
// Created by alberto on 01/10/18.
//

#include "graph_weighted.h"
#include <fstream>
#include <as/repeat.h>
#include <as/graph.h>

namespace smwgcp_cliques {
    namespace {
        void add_partition_cliques(ClusteredWeightedGraph& graph) {
            for(const auto& cluster : graph[boost::graph_bundle].clusters) {
                for(auto i = 0u; i < cluster.size(); ++i) {
                    for(auto j = i + 1; j < cluster.size(); ++j) {
                        auto vi = cluster[i], vj = cluster[j];
                        if(!boost::edge(vi, vj, graph).second) {
                            boost::add_edge(vi, vj, graph);
                        }
                    }
                }
            }
        }

        bool is_simplicial_pair(std::size_t v1, std::size_t v2, std::size_t w1, std::size_t w2, const DirectedGraph& dgraph) {
            const auto [e, e_present] = boost::edge(
                std::min(v1, v2), std::max(v1, v2), dgraph
            );

            const auto [f, f_present] = boost::edge(
                std::min(w1, w2), std::max(w1, w2), dgraph
            );

            (void) e_present, (void) f_present;
            assert(e_present && f_present);

            if(boost::source(e, dgraph) == boost::source(f, dgraph)) {
                const auto et = boost::target(e, dgraph);
                const auto ft = boost::target(f, dgraph);

                if(boost::edge(et, ft, dgraph).second || boost::edge(ft, et, dgraph).second) {
                    return true;
                }
            }

            return false;
        }
    }

    ClusteredWeightedGraph read_clustered_weighted_graph(std::string graph_file) {
        std::ifstream ifs(graph_file);

        if(ifs.fail()) {
            std::fprintf(stderr, "Cannot read graph file!\n");
            std::exit(1);
        }

        std::size_t num_vertices, num_edges, num_clusters;

        if(!(ifs >> num_vertices)) {
            std::fprintf(stderr, "Cannot read the number of vertices.\n");
            std::exit(1);
        }

        if(!(ifs >> num_edges)) {
            std::fprintf(stderr, "Cannot read the number of edges.\n");
            std::exit(1);
        }

        if(!(ifs >> num_clusters)) {
            std::fprintf(stderr, "Cannot read the number of clusters.\n");
            std::exit(1);
        }

        ClusteredWeightedGraph graph;
        std::vector<std::vector<std::size_t>> clusters(num_clusters);

        as::repeat(num_vertices, [&] () {
            float weight;

            if(!(ifs >> weight)) {
                std::fprintf(stderr, "Cannot read a vertex weight.\n");
                std::exit(1);
            }

            boost::add_vertex(ClusteredVertexProperties(weight), graph);
        });

        as::repeat(num_edges, [&] () {
            std::size_t source, target;

            if(!(ifs >> source >> target)) {
                std::fprintf(stderr, "Cannot read an edge.\n");
                std::exit(1);
            }

            boost::add_edge(source, target, graph);
        });

        std::stringstream ss;
        std::string line;

        ifs >> std::ws;

        for(auto cluster = 0u; cluster < num_clusters; ++cluster) {
            std::getline(ifs, line);
            ss.str(line);

            std::size_t vertex;

            while(ss >> vertex) {
                graph[vertex].cluster = cluster;
                clusters[cluster].push_back(vertex);
            }

            ss.str("");
            ss.clear();
        }

        graph[boost::graph_bundle] = { num_clusters, clusters };
        add_partition_cliques(graph);

        return graph;
    }

    LineGraph line_graph(const ClusteredWeightedGraph &cwgraph) {
        LineGraph lgraph(boost::num_edges(cwgraph));

        std::size_t lvertex = 0u;
        for(const auto& edge : as::graph::edges(cwgraph)) {
            lgraph[lvertex++] = std::make_pair(
                boost::target(edge, cwgraph),
                boost::source(edge, cwgraph)
            );
        }

        for(auto e = 0u; e < boost::num_vertices(lgraph); ++e) {
            // Clusters of the endpoints of the first edge.
            const auto cl_e1 = cwgraph[lgraph[e].first].cluster;
            const auto cl_e2 = cwgraph[lgraph[e].second].cluster;

            for(auto f = e + 1; f < boost::num_vertices(lgraph); ++f) {
                // Clusters of the endpoints of the second edge.
                const auto cl_f1 = cwgraph[lgraph[f].first].cluster;
                const auto cl_f2 = cwgraph[lgraph[f].second].cluster;

                // If they have two endpoints in one same cluster, add the edge.
                if(cl_e1 == cl_f1 || cl_e1 == cl_f2 || cl_e2 == cl_f1 || cl_e2 == cl_f2) {
                    boost::add_edge(e, f, lgraph);
                }
            }
        }

        return lgraph;
    }

    DirectedGraph directed_acyclic(const ClusteredWeightedGraph& cwgraph) {
        // True iff the first vertex has lower weight than the second vertex.
        const auto vertex_order = [&cwgraph] (const auto& v1, const auto& v2) -> bool {
            return cwgraph[v1].weight < cwgraph[v2].weight;
        };

        return as::graph::acyclic_orientation(cwgraph, vertex_order);
    }

    LineGraph sandwich_line_graph(const ClusteredWeightedGraph& cwgraph) {
        const auto lgraph = line_graph(cwgraph);
        const auto dgraph = directed_acyclic(cwgraph);
        LineGraph slgraph(boost::num_vertices(lgraph));

        for(auto e = 0u; e < boost::num_vertices(slgraph); ++e) {
            slgraph[e] = lgraph[e];

            // The two endpoints of the first edge.
            const auto e_vertex_1 = lgraph[e].first;
            const auto e_vertex_2 = lgraph[e].second;

            for(auto f = e + 1; f < boost::num_vertices(slgraph); ++f) {
                // The two endpoints of the second edge.
                const auto f_vertex_1 = lgraph[f].first;
                const auto f_vertex_2 = lgraph[f].second;

                // Add an edge if there was an edge in the line graph, and
                // the pair is not simplicial.
                if( boost::edge(e, f, lgraph).second &&
                    ! is_simplicial_pair(e_vertex_1, e_vertex_2, f_vertex_1, f_vertex_2, dgraph)
                ) {
                    boost::add_edge(e, f, slgraph);
                }
            }
        }

        return slgraph;
    }

    LineGraph complementary_sandwich_line_graph(const ClusteredWeightedGraph &cwgraph) {
        const auto slgraph = sandwich_line_graph(cwgraph);
        return as::graph::complementary(slgraph);
    }
}