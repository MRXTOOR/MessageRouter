#include <benchmark/benchmark.h>
#include "../include/lockfree_queue.h"
#include "../include/message.h"
#include <vector>
#include <memory>

using namespace MessageRouter;

static void BM_QueueMemoryAllocation(benchmark::State& state) {
    for (auto _ : state) {
        MessageQueue queue;
        
        Message message(0, 0, 0, 0);
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

static void BM_MessageMemoryAllocation(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<Message> messages;
        messages.reserve(1000);
        
        for (int i = 0; i < 1000; ++i) {
            messages.emplace_back(i % 4, i, i, i);
        }
        
        messages.clear();
    }
    
    state.SetItemsProcessed(state.iterations() * 1000);
    state.SetBytesProcessed(state.iterations() * 1000 * sizeof(Message));
}

static void BM_SharedPtrMemoryAllocation(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<std::shared_ptr<MessageQueue>> queues;
        queues.reserve(100);
        
        for (int i = 0; i < 100; ++i) {
            queues.push_back(std::make_shared<MessageQueue>());
        }
        
        queues.clear();
    }
    
    state.SetItemsProcessed(state.iterations() * 100);
    state.SetBytesProcessed(state.iterations() * 100 * sizeof(std::shared_ptr<MessageQueue>));
}

static void BM_MessageCopying(benchmark::State& state) {
    Message original(0, 0, 0, 0);
    
    for (auto _ : state) {
        Message copy = original;
        
        copy.msg_type = 1;
        copy.producer_id = 1;
        copy.sequence_number = 1;
        copy.timestamp = 1;
    }
    
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * sizeof(Message));
}

static void BM_MessageMoving(benchmark::State& state) {
    for (auto _ : state) {
        Message message(0, 0, 0, 0);
        
        Message moved = std::move(message);
        
        moved.msg_type = 1;
    }
    
    state.SetItemsProcessed(state.iterations());
    state.SetBytesProcessed(state.iterations() * sizeof(Message));
}

BENCHMARK(BM_QueueMemoryAllocation)->UseRealTime();
BENCHMARK(BM_MessageMemoryAllocation)->UseRealTime();
BENCHMARK(BM_SharedPtrMemoryAllocation)->UseRealTime();
BENCHMARK(BM_MessageCopying)->UseRealTime();
BENCHMARK(BM_MessageMoving)->UseRealTime();

BENCHMARK_MAIN();