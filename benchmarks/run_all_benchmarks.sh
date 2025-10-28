#!/bin/bash


echo "Running all Message Router benchmarks..."


mkdir -p results/benchmarks


run_benchmark() {
    local benchmark_name=$1
    local executable=$2
    local output_file="results/benchmarks/${benchmark_name}.json"
    
    echo "Running ${benchmark_name}..."
    
    if [ -f "./${executable}" ]; then
        ./${executable} --benchmark_format=json --benchmark_out=${output_file}
        echo "Results saved in ${output_file}"
    else
        echo "Error: executable file ${executable} not found"
        return 1
    fi
}


run_benchmark "queue_performance" "queue_perf"
run_benchmark "routing_latency" "routing_perf"
run_benchmark "memory_allocation" "memory_perf"
run_benchmark "scaling_benchmark" "scaling_perf"

echo "All benchmarks completed!"
echo "Results available in results/benchmarks/ directory"


echo "Creating summary report..."
cat > results/benchmarks/summary.txt << EOF
=== BENCHMARK SUMMARY REPORT ===
Date: $(date)
Version: Message Router v1.0

Benchmarks run:
1. queue_performance.json - Queue performance
2. routing_latency.json - Routing latency
3. memory_allocation.json - Memory allocation
4. scaling_benchmark.json - Scaling

To view results use:
- JSON files for detailed analysis
- This file for quick overview

EOF

echo "Summary report created: results/benchmarks/summary.txt"
