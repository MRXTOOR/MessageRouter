#include <benchmark/benchmark.h>
#include "../include/lockfree_queue.h"
#include "../include/message.h"
#include "../include/producer.h"
#include "../include/processor.h"
#include "../include/strategy.h"
#include "../include/config.h"
#include <thread>
#include <vector>
#include <memory>
#include <atomic>

using namespace MessageRouter;

static void BM_ProducerScaling(benchmark::State& state) {
    int num_producers = state.range(0);
    
    auto config = std::make_unique<SystemConfig>();
    config->producers.count = num_producers;
    config->producers.messages_per_sec = 1000000;
    config->processors.count = 4;
    config->strategies.count = 3;
    config->duration_secs = 1;
    
    std::vector<std::shared_ptr<MessageQueue>> producer_queues;
    for (int i = 0; i < num_producers; ++i) {
        producer_queues.push_back(std::make_shared<MessageQueue>());
    }
    
    ProducerManager producer_manager(config.get(), producer_queues);
    
    producer_manager.start_all();
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    producer_manager.stop_all();
    producer_manager.wait_for_completion();
    
    state.SetItemsProcessed(producer_manager.get_total_messages_produced());
    state.SetBytesProcessed(producer_manager.get_total_messages_produced() * sizeof(Message));
}

static void BM_ProcessorScaling(benchmark::State& state) {
    int num_processors = state.range(0);
    
    auto config = std::make_unique<SystemConfig>();
    config->producers.count = 4;
    config->producers.messages_per_sec = 1000000;
    config->processors.count = num_processors;
    config->strategies.count = 3;
    config->duration_secs = 1;
    
    std::vector<std::shared_ptr<MessageQueue>> producer_queues;
    std::vector<std::shared_ptr<MessageQueue>> processor_queues;
    
    for (int i = 0; i < 4; ++i) {
        producer_queues.push_back(std::make_shared<MessageQueue>());
    }
    
    for (int i = 0; i < num_processors; ++i) {
        processor_queues.push_back(std::make_shared<MessageQueue>());
    }
    
    ProcessorManager processor_manager(config.get(), producer_queues, processor_queues);
    
    processor_manager.start_all();
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    processor_manager.stop_all();
    processor_manager.wait_for_completion();
    
    state.SetItemsProcessed(processor_manager.get_total_messages_processed());
    state.SetBytesProcessed(processor_manager.get_total_messages_processed() * sizeof(Message));
}

static void BM_StrategyScaling(benchmark::State& state) {
    int num_strategies = state.range(0);
    
    auto config = std::make_unique<SystemConfig>();
    config->producers.count = 4;
    config->producers.messages_per_sec = 1000000;
    config->processors.count = 4;
    config->strategies.count = num_strategies;
    config->duration_secs = 1;
    
    std::vector<std::shared_ptr<MessageQueue>> processor_queues;
    for (int i = 0; i < 4; ++i) {
        processor_queues.push_back(std::make_shared<MessageQueue>());
    }
    
    StrategyManager strategy_manager(config.get(), processor_queues);
    
    strategy_manager.start_all();
    
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    strategy_manager.stop_all();
    strategy_manager.wait_for_completion();
    
    state.SetItemsProcessed(strategy_manager.get_total_messages_delivered());
    state.SetBytesProcessed(strategy_manager.get_total_messages_delivered() * sizeof(Message));
}

static void BM_QueueScaling(benchmark::State& state) {
    int num_queues = state.range(0);
    std::vector<std::shared_ptr<MessageQueue>> queues;
    
    for (int i = 0; i < num_queues; ++i) {
        queues.push_back(std::make_shared<MessageQueue>());
    }
    
    Message message(0, 0, 0, 0);
    uint64_t total_operations = 0;
    
    for (auto _ : state) {
        int queue_idx = state.iterations() % num_queues;
        if (queues[queue_idx]->try_push(message)) {
            Message received;
            if (queues[queue_idx]->try_pop(received)) {
                total_operations++;
            }
        }
    }
    
    state.SetItemsProcessed(total_operations);
    state.SetBytesProcessed(total_operations * sizeof(Message));
}

BENCHMARK(BM_ProducerScaling)->Range(1, 16)->UseRealTime();
BENCHMARK(BM_ProcessorScaling)->Range(1, 16)->UseRealTime();
BENCHMARK(BM_StrategyScaling)->Range(1, 16)->UseRealTime();
BENCHMARK(BM_QueueScaling)->Range(1, 32)->UseRealTime();

BENCHMARK_MAIN();