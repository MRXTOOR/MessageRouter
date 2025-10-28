# Benchmark executables
cmake_minimum_required(VERSION 3.16)

# Find required packages
find_package(benchmark REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP REQUIRED jsoncpp)

# Include directories
include_directories(../include)

# Queue performance benchmark
add_executable(queue_perf queue_performance.cpp)
target_link_libraries(queue_perf 
    message_router_lib
    ${JSONCPP_LIBRARIES}
    benchmark::benchmark
    Threads::Threads
    atomic
)

# Routing latency benchmark
add_executable(routing_perf routing_latency.cpp)
target_link_libraries(routing_perf 
    message_router_lib
    ${JSONCPP_LIBRARIES}
    benchmark::benchmark
    Threads::Threads
)

# Memory allocation benchmark
add_executable(memory_perf memory_allocation.cpp)
target_link_libraries(memory_perf 
    message_router_lib
    ${JSONCPP_LIBRARIES}
    benchmark::benchmark
    Threads::Threads
)

# Scaling benchmark
add_executable(scaling_perf scaling_benchmark.cpp)
target_link_libraries(scaling_perf 
    message_router_lib
    ${JSONCPP_LIBRARIES}
    benchmark::benchmark
    Threads::Threads
)

# All benchmarks runner
add_executable(run_all_benchmarks run_all_benchmarks.cpp)
target_link_libraries(run_all_benchmarks 
    message_router_lib
    ${JSONCPP_LIBRARIES}
    benchmark::benchmark
    Threads::Threads
)
