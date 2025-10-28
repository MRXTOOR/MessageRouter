#include "../include/stage2_router.h"
#include <iostream>

namespace MessageRouter {

Stage2Router::Stage2Router(const SystemConfig* config,
                           const std::vector<std::shared_ptr<MessageQueue>>& input_queues,
                           const std::vector<std::shared_ptr<MessageQueue>>& output_queues)
    : config_(config)
    , input_queues_(input_queues)
    , output_queues_(output_queues)
    , running_(false)
    , messages_routed_(0)
    , routing_errors_(0) {
}

Stage2Router::~Stage2Router() {
    stop();
    if (router_thread_ && router_thread_->joinable()) {
        router_thread_->join();
    }
}

void Stage2Router::start() {
    if (running_.load()) {
        return;
    }
    running_.store(true);
    router_thread_ = std::make_unique<std::thread>(&Stage2Router::routing_loop, this);
}

void Stage2Router::stop() {
    if (!running_.load()) {
        return;
    }
    running_.store(false);
    if (router_thread_ && router_thread_->joinable()) {
        router_thread_->join();
    }
}

void Stage2Router::routing_loop() {
    Message message;
    bool found_message = false;
    
    while (running_.load()) {
        found_message = false;
        
        // Check all input queues (from processors)
        for (auto& input_queue : input_queues_) {
            while (input_queue->try_pop(message)) {
                // Determine strategy for this message type
                StrategyId strategy_id = config_->get_strategy_for_message(message.msg_type);
                
                // Send to corresponding strategy with retries
                if (strategy_id < output_queues_.size()) {
                    bool sent = false;
                    for (int retry = 0; retry < 1000 && !sent; ++retry) {
                        if (output_queues_[strategy_id]->try_push(message)) {
                            messages_routed_.fetch_add(1, std::memory_order_relaxed);
                            sent = true;
                        } else {
                            // Queue full - small pause and retry
                            std::this_thread::yield();
                        }
                    }
                    if (!sent) {
                        routing_errors_.fetch_add(1, std::memory_order_relaxed);
                    }
                } else {
                    routing_errors_.fetch_add(1, std::memory_order_relaxed);
                }
                
                found_message = true;
                // НЕ выходим из цикла - обрабатываем все доступные сообщения
            }
        }
        
        if (!found_message) {
            // Все очереди пустые - busy-waiting для минимальной задержки
            std::this_thread::yield();
        }
    }
}

} // namespace MessageRouter