require_relative 'generate-sgcp'

options = {
    vertices: 1000,
    density: 0.99999,
    part_sz: 0.025,
    file: "hardbp.txt"
}

100.times do |i|
    options[:file] = "hardbp-#{i}.txt"
    generate_instance(options)
end