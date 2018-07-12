class VcpGraph
    attr_accessor :num_vertices, :num_edges

    def initialize(file)
        @num_vertices = 0
        @num_edges = 0
        @adj_matrix = Array.new

        read_from file
    end

    def edge(i, j)
        return @adj_matrix[i][j] == 1
    end

    def read_from(file)
        edge_section = false

        File.readlines(file).each do |line|
            next if line[0] == 'c'

            if line[0] == 'p'
                letter, marker, nv, ne = line.split(' ')

                if letter != 'p' or (marker != 'edge' and marker != 'edges')
                    throw "Invalid line: #{line}"
                end

                @num_vertices = nv.to_i
                @num_edges = ne.to_i
                @adj_matrix = Array.new(@num_vertices) { Array.new(@num_vertices) { 0 } }
                edge_section = true
                next
            end

            if edge_section
                letter, orig, dest = line.split(' ')

                if letter != 'e'
                    throw "Invalid line: #{line}"
                end

                orig = orig.to_i - 1
                dest = dest.to_i - 1

                @adj_matrix[orig][dest] = 1
                @adj_matrix[dest][orig] = 1
            end
        end
    end
end

class SvcpGraph
    attr_accessor :num_vertices, :num_edges, :num_clusters

    def initialize(vcp_instance, growth_factor, share_of_edges_to_keep = 1.0)
        @num_vertices = vcp_instance.num_vertices * growth_factor
        @num_edges = 0
        @num_clusters = vcp_instance.num_vertices
        @clusters = Array.new(@num_clusters) { Array.new(growth_factor) { 0 } }
        @adjacency_matrix = Array.new(@num_vertices) { Array.new(@num_vertices) { 0 } }
        @edges_share = share_of_edges_to_keep

        build_from(vcp_instance, growth_factor)
    end

    def cluster(i)
        return @clusters[i]
    end

    def cluster_of(v)
        @clusters.each_index.detect do |cls_id|
            @clusters[cls_id].include? v
        end
    end

    def edge(v, w)
        return @adjacency_matrix[v][w] == 1
    end

    def to_file(filename)
        File.open(filename, 'w') do |f|
            f.puts(@num_vertices)
            f.puts(@num_edges)
            f.puts(@num_clusters)
            0.upto(@num_vertices - 1) do |v|
                (v + 1).upto(@num_vertices - 1) do |w|
                    if edge(v, w)
                        f.puts("#{v} #{w}")
                    end
                end
            end
            @clusters.each do |cls|
                f.puts(cls.join(' '))
            end
        end
    end

    def build_from(vcp_instance, growth_factor)
        0.upto(@num_clusters - 1) do |i|
            @clusters[i] = ((i * growth_factor)..((i + 1) * growth_factor - 1)).to_a
        end

        0.upto(@num_clusters - 1) do |i|
            (i + 1).upto(@num_clusters - 1) do |j|
                if vcp_instance.edge(i, j)
                    add_all_cluster_edges_prob(i, j)
                end
            end
        end
    end

    def add_all_cluster_edges_prob(i, j)
        @clusters[i].each do |v|
            @clusters[j].each do |w|
                add_edge_prob(v, w)
            end
        end
    end

    def add_edge_prob(v, w)
        if rand() <= @edges_share
            @adjacency_matrix[v][w] = 1
            @adjacency_matrix[w][v] = 1
            @num_edges += 1
        end
    end
end

# Dir.glob('graphs/vcp-instances/*.col') do |file|
    file = ARGV[0]
    puts "Reading VCP instance #{file}"
    vcp = VcpGraph.new(file)
    puts 'Writing SVCP instances'
    ins_file = File.basename(file, '.col')

    [2,3,4].each do |growth_factor|
        [0.9, 0.95, 1.0].each do |edge_prob|
            out_file = "graphs/from-vcp/growth-#{growth_factor}/#{edge_prob}-#{ins_file}.col"
            svcp = SvcpGraph.new(vcp, growth_factor, edge_prob)
            svcp.to_file(out_file)
        end
    end
# end