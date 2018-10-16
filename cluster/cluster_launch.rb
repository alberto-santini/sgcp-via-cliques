require 'optparse'

home = File.join('/', 'homes', 'users', 'asantini')
exe = File.join(home, 'local', 'src', 'sgcp-via-cliques', 'build', 'sgcp_cliques')
libraries = [
    File.join(home, 'local', 'lib'),
    File.join(home, 'local', 'lib64')
]
base_script = <<~EOF
#!/bin/bash
#SBATCH --partition=normal
#SBATCH --time=10:00:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --mem-per-cpu=4096
EOF

@options = {}

OptionParser.new do |opts|
    opts.on('-i INSTANCE', '--instance', 'Instance file') do |instance|
        @options[:instance] = instance
    end

    opts.on('-t TIMEOUT', '--timeout', 'Cplex max clique timeout in seconds') do |timeout|
        @options[:timeout] = timeout.to_i
    end

    opts.on('-w', '--[no-]weighted', 'Tell whether we are solving the weighted version') do |weighted|
        @options[:weighted] = weighted
    end
end.parse!

basename = File.basename(@options[:instance])
results_file = "results-#{basename}"
stdout_file = "out-#{basename}"
stderr_file = "err-#{basename}"
cls_file = "launch-#{basename}.sh"

cmd =  "LD_LIBRARY_PATH=#{libraries.join(':')} #{exe} "
cmd += "-g #{File.expand_path(@options[:instance])} "
cmd += "-t #{@options[:timeout]} "

if @options[:weighted]
    cmd += "-o #{results_file} "
    cmd += '-y weighted-clique'

    cmd_mip = cmd.gsub('weighted-clique', 'weighted-mip')
    cmd_mip = cmd_mip.gsub('results-', 'results-mip-')

    cmd += "\n#{cmd_mip}"
else
    cmd += "-o #{results_file} "
    cmd += '-y unweighted'
end

script = <<~EOF
    #{base_script}
    #SBATCH -o #{stdout_file}
    #SBATCH -e #{stderr_file}
    #{cmd}
EOF

File.write(cls_file, script)