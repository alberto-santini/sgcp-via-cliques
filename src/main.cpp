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

    if(parser["problem-type"].get().string == "unweighted") {
        const auto zero_time = high_resolution_clock::now();

        std::cout << "Reading graph from file...\n";

        const auto cgraph = sgcp_cliques::read_clustered_graph(graph_file);

        std::cout << "Graph read from file (" << duration_cast<duration<float>>(high_resolution_clock::now() - zero_time).count() << " s)\n";
        std::cout << "Preparing Clique graph...\n";

        const auto working_graph = as::graph::complementary(cgraph);
        const auto clique_graph = sgcp_cliques::complementary_sandwich_line_graph(working_graph);

        std::cout << "Clique graph ready (" << duration_cast<duration<float>>(high_resolution_clock::now() - zero_time).count() << " s)\n";
        std::cout << "Launching the Clique solver...\n";

        const auto start_time = high_resolution_clock::now();
        const auto max_clique = as::max_clique::solve_with_mip(clique_graph, cplex_timeout);
        const auto end_time = high_resolution_clock::now();
        const auto elapsed = duration_cast<duration<float>>(end_time - start_time).count();
        const auto chromatic_n = sgcp_cliques::number_of_partitions(cgraph) - max_clique.size();
        const auto instance = fs::path{graph_file}.stem().string();

        ofs << instance << "," << chromatic_n << "," << elapsed << "\n";
    } else if(parser["problem-type"].get().string == "weighted") {
        const auto zero_time = high_resolution_clock::now();

        std::cout << "Reading graph from file...\n";

        const auto cwgraph = smwgcp_cliques::read_clustered_weighted_graph(graph_file);

        std::cout << "Graph read from file (" << duration_cast<duration<float>>(high_resolution_clock::now() - zero_time).count() << " s)\n";
        std::cout << "Preparing Max-Weight Clique graph...\n";

        const auto working_graph = as::graph::complementary(cwgraph);
        const auto clique_graph = smwgcp_cliques::complementary_sandwich_line_graph(working_graph);

        std::cout << "Max-Weight Clique graph ready (" << duration_cast<duration<float>>(high_resolution_clock::now() - zero_time).count() << " s)\n";
        std::cout << "Launching the Max-Weight Clique solver...\n";

        const auto start_time_clique = high_resolution_clock::now();
        const auto max_clique = as::max_clique::solve_with_mip(clique_graph, cplex_timeout);
        const auto end_time_clique = high_resolution_clock::now();
        const auto elapsed_clique = duration_cast<duration<float>>(end_time_clique - start_time_clique).count();

        std::cout << "Clique solver finished (" << elapsed_clique << " s)\n";

        const auto max_clique_weight = std::accumulate(max_clique.begin(), max_clique.end(), 0.0f,
           [&] (float acc, const auto& vertex) -> float {
               return acc + clique_graph[vertex].weight;
           }
        );
        const auto weighted_chromatic_n = smwgcp_cliques::sum_of_weights(cwgraph) - max_clique_weight;

        std::cout << "Clique solver result: " << weighted_chromatic_n << " (" << smwgcp_cliques::sum_of_weights(cwgraph) << " - " << max_clique_weight << ")\n";
        std::cout << "Launching the MIP solver...\n";

        const auto start_time_mip = high_resolution_clock::now();
        const auto mip_result = smwgcp_cliques::solve_with_mip(cwgraph);
        const auto end_time_mip = high_resolution_clock::now();
        const auto elapsed_mip = duration_cast<duration<float>>(end_time_mip - start_time_mip).count();

        std::cout << "MIP solver finished (" << elapsed_mip << " s)\n";
        std::cout << "MIP solver result: LB = " << mip_result.first << ", UB: " << mip_result.second << "\n";

        const auto instance = fs::path{graph_file}.stem().string();
        ofs << instance << "," << weighted_chromatic_n << "," << elapsed_clique << "," << mip_result.first << "," << mip_result.second << "," << elapsed_mip << "\n";
    } else {
        std::cerr << "Wrong problem type: " << parser["problem-type"].get().string << "\n";
        return 1;
    }
    
    return 0;
}