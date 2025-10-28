#include <benchmark/benchmark.h>
#include "../include/lockfree_queue.h"
#include "../include/message.h"
#include <thread>
#include <vector>
#include <atomic>

using namespace MessageRouter;

static void BM_SPSCQueue_PushPop(benchmark::State& state) {
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

static void BM_SPSCQueue_Concurrent(benchmark::State& state) {
    MessageQueue queue;
    std::atomic<bool> running{true};
    std::atomic<uint64_t> messages_sent{0};
    std::atomic<uint64_t> messages_received{0};
    
    std::thread producer([&]() {
        Message message(0, 0, 0, 0);
        while (running.load()) {
            if (queue.try_push(message)) {
                messages_sent.fetch_add(1);
            }
            std::this_thread::yield();
        }
    });
    
    std::thread consumer([&]() {
        Message message;
        while (running.load()) {
            if (queue.try_pop(message)) {
                messages_received.fetch_add(1);
            }
            std::this_thread::yield();
        }
    });
    
    for (auto _ : state) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    running.store(false);
    producer.join();
    consumer.join();
    
    state.SetItemsProcessed(messages_received.load());
    state.SetBytesProcessed(messages_received.load() * sizeof(Message));
}

static void BM_SPSCQueue_Latency(benchmark::State& state) {
    MessageQueue queue;
    Message message(0, 0, 0, 0);
    
    for (auto _ : state) {
        auto start = std::chrono::high_resolution_clock::now();
        
        if (queue.try_push(message)) {
            Message received;
            if (queue.try_pop(received)) {
                auto end = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
                state.SetIterationTime(elapsed.count() / 1e9);
            }
        }
    }
}

BENCHMARK(BM_SPSCQueue_PushPop)->Range(1, 1000000);
BENCHMARK(BM_SPSCQueue_Concurrent)->UseRealTime();
BENCHMARK(BM_SPSCQueue_Latency)->UseManualTime();

BENCHMARK_MAIN();