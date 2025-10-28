#include <benchmark/benchmark.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>


void runAllBenchmarks() {
    std::cout << "Running all benchmarks..." << std::endl;
    
    
    system("mkdir -p results/benchmarks");
    
    
    std::cout << "Running queue performance benchmark..." << std::endl;
    system("./queue_perf --benchmark_format=json --benchmark_out=results/benchmarks/queue_performance.json");
    
    std::cout << "Running routing latency benchmark..." << std::endl;
    system("./routing_perf --benchmark_format=json --benchmark_out=results/benchmarks/routing_latency.json");
    
    std::cout << "Running memory allocation benchmark..." << std::endl;
    system("./memory_perf --benchmark_format=json --benchmark_out=results/benchmarks/memory_allocation.json");
    
    std::cout << "Running scaling benchmark..." << std::endl;
    system("./scaling_perf --benchmark_format=json --benchmark_out=results/benchmarks/scaling_benchmark.json");
    
    std::cout << "All benchmarks completed. Results saved in results/benchmarks/" << std::endl;
}

int main(int argc, char* argv[]) {
    
    if (argc > 1 && std::string(argv[1]) == "all") {
        runAllBenchmarks();
        return 0;
    }
    
    
    benchmark::Initialize(&argc, argv);
    if (benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
    benchmark::RunSpecifiedBenchmarks();
    benchmark::Shutdown();
    
    return 0;
}
