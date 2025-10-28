#include "../include/producer.h"
#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <memory>
#include <string>
#include <cstdlib>
#include <cstring>

namespace MessageRouter {

Producer::Producer(ProducerId id, const SystemConfig* config, std::shared_ptr<MessageQueue> output_queue)
    : producer_id_(id)
    , config_(config)
    , output_queue_(output_queue)
    , running_(false)
    , messages_produced_(0)
    , next_sequence_(1) {
}

Producer::~Producer() {
    stop();
    if (producer_thread_ && producer_thread_->joinable()) {
        producer_thread_->join();
    }
}

void Producer::start() {
    if (running_.load()) {
        return;
    }
    running_.store(true);
    producer_thread_ = std::make_unique<std::thread>(&Producer::producer_loop, this);
}

void Producer::stop() {
    if (!running_.load()) {
        return;
    }
    running_.store(false);
    if (producer_thread_ && producer_thread_->joinable()) {
        producer_thread_->join();
    }
}

void Producer::producer_loop() {
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<MessageType> msg_type_dist(0, 3);
    
    
    double messages_per_sec = config_->producers.messages_per_sec;
    double interval_ns = 1e9 / messages_per_sec;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    auto next_message_time = start_time;
    
    while (running_.load()) {
        auto current_time = std::chrono::high_resolution_clock::now();
        
        if (current_time >= next_message_time) {
            
            MessageType msg_type = msg_type_dist(gen);
            uint64_t timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                current_time.time_since_epoch()).count();
            
            Message message(msg_type, producer_id_, next_sequence_++, timestamp);
            
            
            bool sent = false;
            for (int retry = 0; retry < 100 && !sent; ++retry) {
                if (output_queue_->try_push(message)) {
                    messages_produced_.fetch_add(1, std::memory_order_relaxed);
                    sent = true;
                } else {
                    
                    std::this_thread::yield();
                }
            }
            
            
            next_message_time = current_time + std::chrono::nanoseconds(static_cast<uint64_t>(interval_ns));
        } else {
            
            std::this_thread::yield();
        }
    }
}

ProducerManager::ProducerManager(const SystemConfig* config, 
                               const std::vector<std::shared_ptr<MessageQueue>>& output_queues)
    : config_(config)
    , output_queues_(output_queues) {
    
    
    // Create producers - each gets its own SPSC queue
    for (int i = 0; i < config->producers.count; ++i) {
        auto output_queue = output_queues[i]; // Each producer has its own queue
        producers_.push_back(std::make_unique<Producer>(i, config, output_queue));
    }
}

ProducerManager::~ProducerManager() {
    stop_all();
}

void ProducerManager::start_all() {
    for (auto& producer : producers_) {
        producer->start();
    }
}

void ProducerManager::stop_all() {
    for (auto& producer : producers_) {
        producer->stop();
    }
}

void ProducerManager::wait_for_completion() {
    for (auto& producer : producers_) {
        if (producer->producer_thread_ && producer->producer_thread_->joinable()) {
            producer->producer_thread_->join();
        }
    }
}

uint64_t ProducerManager::get_total_messages_produced() const {
    uint64_t total = 0;
    for (const auto& producer : producers_) {
        total += producer->get_messages_produced();
    }
    return total;
}

} // namespace MessageRouter