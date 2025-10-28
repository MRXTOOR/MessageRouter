cmake_minimum_required(VERSION 3.16)
project(MessageRouter)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")


find_package(PkgConfig REQUIRED)
find_package(benchmark REQUIRED)
find_package(Threads REQUIRED)
find_package(jsoncpp REQUIRED)


include_directories(include)


set(SOURCES
    src/main.cpp
    src/config.cpp
    src/producer.cpp
    src/processor.cpp
    src/strategy.cpp
    src/stage1_router.cpp
    src/stage2_router.cpp
)


set(HEADERS
    include/message.h
    include/lockfree_queue.h
    include/config.h
    include/producer.h
    include/processor.h
    include/strategy.h
    include/stage1_router.h
    include/stage2_router.h
)


add_library(message_router_lib STATIC ${SOURCES} ${HEADERS})


target_link_libraries(message_router_lib 
    jsoncpp_lib
    Threads::Threads
)


add_executable(message_router src/main.cpp)


target_link_libraries(message_router 
    message_router_lib
    jsoncpp_lib
    Threads::Threads
)


add_executable(queue_perf benchmarks/queue_performance.cpp)
target_link_libraries(queue_perf 
    message_router_lib
    jsoncpp_lib
    benchmark::benchmark
    Threads::Threads
)

add_executable(routing_perf benchmarks/routing_latency.cpp)
target_link_libraries(routing_perf 
    message_router_lib
    jsoncpp_lib
    benchmark::benchmark
    Threads::Threads
)

add_executable(memory_perf benchmarks/memory_allocation.cpp)
target_link_libraries(memory_perf 
    message_router_lib
    jsoncpp_lib
    benchmark::benchmark
    Threads::Threads
)

add_executable(scaling_perf benchmarks/scaling_benchmark.cpp)
target_link_libraries(scaling_perf 
    message_router_lib
    jsoncpp_lib
    benchmark::benchmark
    Threads::Threads
)

add_executable(simple_bench benchmarks/simple_benchmark.cpp)
target_link_libraries(simple_bench 
    message_router_lib
    jsoncpp_lib
    benchmark::benchmark
    Threads::Threads
)


install(TARGETS message_router queue_perf routing_perf memory_perf scaling_perf simple_bench DESTINATION bin)