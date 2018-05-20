//
// Created by alberto on 08/05/18.
//

#include "graph.h"
#include <as/graph.h>
#include <as/max_clique.h>
#include <fstream>
#include <chrono>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

int main(int argc, char* argv[]) {
    using namespace sgcp_cliques;
    using namespace std::chrono;

    if(argc == 3) {
        const auto cgraph = read_clustered_graph(argv[1]);
        const auto working_graph = as::graph::complementary(cgraph);
        const auto clique_graph = complementary_sandwich_line_graph(working_graph);

        const auto start_time = high_resolution_clock::now();
        const auto max_clique = as::max_clique::solve_with_mip(clique_graph);
        const auto end_time = high_resolution_clock::now();
        const auto elapsed = duration_cast<duration<float>>(end_time - start_time).count();
        const auto chromatic_n = cgraph[boost::graph_bundle] - max_clique.size();
        const auto instance = fs::path{argv[1]}.stem().string();

        std::ofstream ofs;
        ofs.open(argv[2], std::ios_base::app);

        if(!ofs.fail()) {
            ofs << instance << "," << chromatic_n << "," << elapsed << "\n";
        }

        return 0;
    } else {
        std::cerr << "Usage: " << argv[0] << " <graph> <results_file>\n";
        return 1;
    }
}