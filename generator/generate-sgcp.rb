def generate_instance(options)
    n = options[:vertices]
    edges = []

    0.upto(n - 1) do |i|
        (i + 1).upto(n - 1) do |j|
            r = rand(0.0 .. 1.0)
            if r <= options[:density]
                edges << [i, j]
            end
        end
    end

    ps = (n.to_f * options[:part_sz]).to_i

    return if ps == 0

    v = 0.upto(n - 1).to_a.shuffle
    partitions = []

    v.each_slice(ps) do |vertices|
        partitions << vertices
    end

    File.open(options[:file], 'w') do |file|
        file.puts options[:vertices]
        file.puts edges.size
        file.puts partitions.size

        edges.each do |e|
            file.puts e.join(' ')
        end

        partitions.each do |p|
            file.puts p.join(' ')
        end
    end
end