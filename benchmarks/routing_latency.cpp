#include <benchmark/benchmark.h>
#include "../include/lockfree_queue.h"
#include "../include/message.h"
#include "../include/config.h"
#include <thread>
#include <vector>
#include <atomic>
#include <memory>

using namespace MessageRouter;

static void BM_RoutingLatency(benchmark::State& state) {
    auto config = std::make_unique<SystemConfig>();
    config->producers.count = 4;
    config->processors.count = 4;
    config->strategies.count = 3;
    
    config->stage1_rules = {
        {0, {0}}, {1, {1}}, {2, {2}}, {3, {3}}
    };
    config->stage2_rules = {
        {0, 0, true}, {1, 1, true}, {2, 2, true}, {3, 0, true}
    };
    
    std::vector<std::shared_ptr<MessageQueue>> producer_queues;
    std::vector<std::shared_ptr<MessageQueue>> processor_queues;
    
    for (int i = 0; i < 4; ++i) {
        producer_queues.push_back(std::make_shared<MessageQueue>());
        processor_queues.push_back(std::make_shared<MessageQueue>());
    }
    
    Message message(0, 0, 0, 0);
    
    for (auto _ : state) {
        auto start = std::chrono::high_resolution_clock::now();
        
        // Stage 1: Producer -> Processor
        if (producer_queues[0]->try_push(message)) {
            Message received;
            if (producer_queues[0]->try_pop(received)) {
                if (processor_queues[0]->try_push(received)) {
                    Message final;
                    if (processor_queues[0]->try_pop(final)) {
                        auto end = std::chrono::high_resolution_clock::now();
                        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                        state.SetIterationTime(elapsed.count() / 1e9);
                    }
                }
            }
        }
    }
}

static void BM_RoutingThroughput(benchmark::State& state) {
    MessageQueue queue1, queue2;
    std::atomic<uint64_t> messages_processed{0};
    
    std::thread router([&]() {
        Message message;
        while (messages_processed.load() < state.iterations()) {
            if (queue1.try_pop(message)) {
                if (queue2.try_push(message)) {
                    messages_processed.fetch_add(1);
                }
            }
        }
    });
    
    Message message(0, 0, 0, 0);
    for (auto _ : state) {
        if (queue1.try_push(message)) {
        }
    }
    
    router.join();
    
    state.SetItemsProcessed(messages_processed.load());
    state.SetBytesProcessed(messages_processed.load() * sizeof(Message));
}

static void BM_RoutingScaling(benchmark::State& state) {
    int num_queues = state.range(0);
    std::vector<std::shared_ptr<MessageQueue>> queues;
    
    for (int i = 0; i < num_queues; ++i) {
        queues.push_back(std::make_shared<MessageQueue>());
    }
    
    Message message(0, 0, 0, 0);
    
    for (auto _ : state) {
        int queue_idx = state.iterations() % num_queues;
        if (queues[queue_idx]->try_push(message)) {
            Message received;
            if (queues[queue_idx]->try_pop(received)) {
            }
        }
    }
    
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * sizeof(Message));
}

BENCHMARK(BM_RoutingLatency)->UseManualTime();
BENCHMARK(BM_RoutingThroughput)->UseRealTime();
BENCHMARK(BM_RoutingScaling)->Range(1, 16)->UseRealTime();

BENCHMARK_MAIN();