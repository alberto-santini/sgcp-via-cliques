//
// Created by alberto on 08/05/18.
//

#include "graph.h"
#include "graph_weighted.h"
#include <as/graph.h>
#include <as/max_clique.h>
#include <as/and_die.h>
#include <ProgramOptions.hxx>
#include <fstream>
#include <chrono>
#include <optional>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

int main(int argc, char* argv[]) {
    using namespace std::chrono;
    using as::and_die;
    namespace fs = std::experimental::filesystem;

    po::parser parser;

    parser["graph"]
        .abbreviation('g')
        .description("File containing the graph. Mandatory.")
        .type(po::string);

    parser["output"]
        .abbreviation('o')
        .description("File where we append the results. Mandatory.")
        .type(po::string);

    parser["cplex-timeout"]
        .abbreviation('t')
        .description("Timeout when solving the max-clique problem with CPLEX.")
        .type(po::f32);

    parser["problem-type"]
        .abbreviation('y')
        .description("Type of problem to solve. It can be either weighted or unweighted (default).")
        .type(po::string)
        .fallback("unweighted");

    parser["help"]
        .abbreviation('h')
        .description("Prints this help text.")
        .callback([&] () { std::cout << parser << "\n"; });

    parser(argc, argv);

    if(!parser["graph"].was_set()) {
        std::cerr << "You need to specify a graph file!" << and_die();
    }

    if(!parser["output"].was_set()) {
        std::cerr << "You need to specify an output file!" << and_die();
    }

    std::optional<float> cplex_timeout = std::nullopt;

    if(parser["cplex-timeout"].was_set()) {
        cplex_timeout = parser["cplex-timeout"].get().f32;
    }

    std::string graph_file = parser["graph"].get().string;
    std::string output_file = parser["output"].get().string;

    if(!fs::exists(graph_file)) {
        std::cerr << "Graph file does not exist: " << graph_file << and_die();
    }

    std::ofstream ofs;
    ofs.open(output_file, std::ios_base::app);

    if(ofs.fail()) {
        std::cerr << "Cannot access output file: " << output_file << and_die();
    }

    auto run_clique_algorithm = [&] (const auto& clique_graph, const auto& n_partitions) {
        const auto start_time = high_resolution_clock::now();
        const auto max_clique = as::max_clique::solve_with_mip(clique_graph, cplex_timeout);
        const auto end_time = high_resolution_clock::now();
        const auto elapsed = duration_cast<duration<float>>(end_time - start_time).count();
        const auto chromatic_n = n_partitions - max_clique.size();
        const auto instance = fs::path{graph_file}.stem().string();

        ofs << instance << "," << chromatic_n << "," << elapsed << "\n";
    };

    if(parser["problem-type"].get().string == "unweighted") {
        const auto cgraph = sgcp_cliques::read_clustered_graph(graph_file);
        const auto working_graph = as::graph::complementary(cgraph);
        const auto clique_graph = sgcp_cliques::complementary_sandwich_line_graph(working_graph);
        run_clique_algorithm(clique_graph, sgcp_cliques::number_of_partitions(cgraph));
    } else if(parser["problem-type"].get().string == "weighted") {
        const auto cwgraph = smwgcp_cliques::read_clustered_weighted_graph(graph_file);
        const auto working_graph = as::graph::complementary(cwgraph);
        const auto clique_graph = smwgcp_cliques::complementary_sandwich_line_graph(working_graph);
        run_clique_algorithm(clique_graph, smwgcp_cliques::number_of_partitions(cwgraph));
    } else {
        std::cerr << "Wrong problem type: " << parser["problem-type"].get().string << "\n";
        return 1;
    }
    
    return 0;
}