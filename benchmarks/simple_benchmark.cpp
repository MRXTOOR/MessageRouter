#include <benchmark/benchmark.h>
#include "../include/lockfree_queue.h"
#include "../include/message.h"
#include <vector>

using namespace MessageRouter;


static void BM_QueuePushPop(benchmark::State& state) {
    MessageQueue queue;
    Message message(0, 0, 0, 0);
    
    for (auto _ : state) {
        if (queue.try_push(message)) {
            Message received;
            if (queue.try_pop(received)) {
                
            }
        }
    }
    
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * sizeof(Message));
}


static void BM_QueueFill(benchmark::State& state) {
    MessageQueue queue;
    Message message(0, 0, 0, 0);
    
    for (auto _ : state) {
        
        for (int i = 0; i < 1000; ++i) {
            queue.try_push(message);
        }
        
        
        Message received;
        while (queue.try_pop(received)) {
            
        }
    }
    
    state.SetItemsProcessed(state.iterations() * 1000);
    state.SetBytesProcessed(state.iterations() * 1000 * sizeof(Message));
}


static void BM_MessageCreation(benchmark::State& state) {
    for (auto _ : state) {
        Message message(0, 0, 0, 0);
        benchmark::DoNotOptimize(message);
    }
    
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * sizeof(Message));
}


BENCHMARK(BM_QueuePushPop)->Range(1, 100000);
BENCHMARK(BM_QueueFill)->Range(1, 1000);
BENCHMARK(BM_MessageCreation)->Range(1, 1000000);

BENCHMARK_MAIN();
