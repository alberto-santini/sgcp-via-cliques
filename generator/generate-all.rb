require_relative 'generate-sgcp'

(5 .. 200).step(5) do |vertices|
    (0.1 .. 0.9).step(0.1) do |density|
        (0.05 .. 0.3).step(0.05) do |part_sz|
            f = "graphs/g-#{vertices}-#{density.round(1)}-#{part_sz.round(2)}.txt"

            options = {
                vertices: vertices,
                density: density,
                part_sz: part_sz,
                file: f
            }

            generate_instance(options)
        end
    end
end
