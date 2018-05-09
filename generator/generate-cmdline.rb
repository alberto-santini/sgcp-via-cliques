require 'optparse'
require_relative 'generate-sgcp'

@options = {
    vertices:  100, # Number of vertices in the graph (larger than 0)
    density:   0.5, # Graph density (between 0.0 and 1.0, extremes excluded)
    part_sz:   0.1, # Average size of a partition as a fraction of the number of vertices (between 0.01 and 0.5)
    file:  'g.txt'  # Output file
}

OptionParser.new do |opts|
    opts.on('-n NUM', '--num-vertices NUM', 'Number of vertices') do |n|
        if n.to_i <= 0
            throw "Invalid number of vertices: #{n}: needs to be > 0"
        end
        @options[:vertices] = n.to_i
    end

    opts.on('-d DENSITY', '--density DENSITY', 'Graph density') do |d|
        if d.to_f <= 0.0 || d.to_f >= 1.0
            throw "Invalid density #{d}: needs to be > 0 and < 1"
        end
        @options[:density] = d.to_f
    end

    opts.on('-p SIZE', '--partition-size SIZE', 'Partition size as a fraction of the vertices') do |s|
        if s.to_f < 0.01 || s.to_f > 0.5
            throw "Invalid partition size #{s}: needs to be >= .01 and <= .5"
        end
        @options[:part_sz] = s.to_f
    end

    opts.on('-f FILE', '--file FILE', 'Output file') do |f|
        @options[:file] = f
    end
end.parse!

generate_instance(@options)