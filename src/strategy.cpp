#include "../include/strategy.h"
#include <iostream>
#include <chrono>
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <cstdlib>
#include <cstring>

namespace MessageRouter {

Strategy::Strategy(StrategyId id, const SystemConfig* config, 
                   std::shared_ptr<MessageQueue> input_queue)
    : strategy_id_(id)
    , config_(config)
    , input_queue_(input_queue)
    , running_(false)
    , messages_delivered_(0)
    , ordering_violations_(0) {
}

Strategy::~Strategy() {
    stop();
    if (strategy_thread_ && strategy_thread_->joinable()) {
        strategy_thread_->join();
    }
}

void Strategy::start() {
    if (running_.load()) {
        return;
    }
    running_.store(true);
    strategy_thread_ = std::make_unique<std::thread>(&Strategy::strategy_loop, this);
}

void Strategy::stop() {
    if (!running_.load()) {
        return;
    }
    running_.store(false);
    if (strategy_thread_ && strategy_thread_->joinable()) {
        strategy_thread_->join();
    }
}

void Strategy::strategy_loop() {
    Message message;
    
    while (running_.load()) {
        // Обрабатываем ВСЕ доступные сообщения из своей SPSC очереди
        while (input_queue_->try_pop(message)) {
            process_message(message);
        }
        
        // Очередь пустая - busy-waiting для минимальной задержки
        std::this_thread::yield();
    }
}

void Strategy::process_message(const Message& message) {
    // Упрощенная проверка порядка для максимальной производительности
    auto key = std::make_pair(message.producer_id, message.msg_type);
    auto it = expected_sequence_.find(key);
    
    if (it == expected_sequence_.end()) {
        // Первое сообщение от этого производителя и типа
        expected_sequence_[key] = message.sequence_number + 1;
    } else {
        // Простая проверка порядка
        if (message.sequence_number != it->second) {
            // Нарушение порядка - отмечаем но не блокируем
            ordering_violations_.fetch_add(1);
        }
        it->second = message.sequence_number + 1;
    }
    
    // Симулируем обработку стратегии
    simulate_strategy_processing();
    
    messages_delivered_.fetch_add(1, std::memory_order_relaxed);
}

void Strategy::simulate_strategy_processing() {
    // Минимальная обработка для максимальной производительности
    // В реальной системе здесь была бы логика стратегии
}

StrategyManager::StrategyManager(const SystemConfig* config,
                               const std::vector<std::shared_ptr<MessageQueue>>& input_queues)
    : config_(config)
    , input_queues_(input_queues) {
    
    // Create strategies - each gets its own SPSC queue
    for (int i = 0; i < config->strategies.count; ++i) {
        auto input_queue = input_queues[i]; // Each strategy has its own input queue
        strategies_.push_back(std::make_unique<Strategy>(i, config, input_queue));
    }
}

StrategyManager::~StrategyManager() {
    stop_all();
}

void StrategyManager::start_all() {
    for (auto& strategy : strategies_) {
        strategy->start();
    }
}

void StrategyManager::stop_all() {
    for (auto& strategy : strategies_) {
        strategy->stop();
    }
}

void StrategyManager::wait_for_completion() {
    for (auto& strategy : strategies_) {
        if (strategy->strategy_thread_ && strategy->strategy_thread_->joinable()) {
            strategy->strategy_thread_->join();
        }
    }
}

uint64_t StrategyManager::get_total_messages_delivered() const {
    uint64_t total = 0;
    for (const auto& strategy : strategies_) {
        total += strategy->get_messages_delivered();
    }
    return total;
}

uint64_t StrategyManager::get_total_ordering_violations() const {
    uint64_t total = 0;
    for (const auto& strategy : strategies_) {
        total += strategy->get_ordering_violations();
    }
    return total;
}

} // namespace MessageRouter