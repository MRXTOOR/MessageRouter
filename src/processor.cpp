#include "../include/processor.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include <string>
#include <cstdlib>
#include <cstring>

namespace MessageRouter {

Processor::Processor(ProcessorId id, const SystemConfig* config,
                    std::shared_ptr<MessageQueue> input_queue,
                    std::shared_ptr<MessageQueue> output_queue)
    : processor_id_(id)
    , config_(config)
    , input_queue_(input_queue)
    , output_queue_(output_queue)
    , running_(false)
    , messages_processed_(0) {
}

Processor::~Processor() {
    stop();
    if (processor_thread_ && processor_thread_->joinable()) {
        processor_thread_->join();
    }
}

void Processor::start() {
    if (running_.load()) {
        return;
    }
    running_.store(true);
    processor_thread_ = std::make_unique<std::thread>(&Processor::processor_loop, this);
}

void Processor::stop() {
    if (!running_.load()) {
        return;
    }
    running_.store(false);
    if (processor_thread_ && processor_thread_->joinable()) {
        processor_thread_->join();
    }
}

void Processor::processor_loop() {
    Message message;
    
    while (running_.load()) {
        // Обрабатываем ВСЕ доступные сообщения
        while (input_queue_->try_pop(message)) {
            // Обрабатываем сообщение
            simulate_processing(message);
            
            // Отправляем обработанное сообщение с повторными попытками
            bool sent = false;
            for (int retry = 0; retry < 1000 && !sent; ++retry) {
                if (output_queue_->try_push(message)) {
                    messages_processed_.fetch_add(1, std::memory_order_relaxed);
                    sent = true;
                } else {
                    // Очередь полная - небольшая пауза и повторная попытка
                    std::this_thread::yield();
                }
            }
        }
        
        // Очередь пустая - busy-waiting для минимальной задержки
        std::this_thread::yield();
    }
}

void Processor::simulate_processing(Message& message) {
    // Заполняем поля процессора
    message.processor_id = processor_id_;
    message.processing_timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Минимальная обработка для максимальной производительности
    // В реальной системе здесь была бы логика обработки
}

ProcessorManager::ProcessorManager(const SystemConfig* config,
                                 const std::vector<std::shared_ptr<MessageQueue>>& input_queues,
                                 const std::vector<std::shared_ptr<MessageQueue>>& output_queues)
    : config_(config)
    , input_queues_(input_queues)
    , output_queues_(output_queues) {
    
    // Create processors - each gets its own SPSC queue
    for (int i = 0; i < config->processors.count; ++i) {
        auto input_queue = input_queues[i]; // Each processor has its own input queue
        auto output_queue = output_queues[i]; // Each processor has its own output queue
        processors_.push_back(std::make_unique<Processor>(i, config, input_queue, output_queue));
    }
}

ProcessorManager::~ProcessorManager() {
    stop_all();
}

void ProcessorManager::start_all() {
    for (auto& processor : processors_) {
        processor->start();
    }
}

void ProcessorManager::stop_all() {
    for (auto& processor : processors_) {
        processor->stop();
    }
}

void ProcessorManager::wait_for_completion() {
    for (auto& processor : processors_) {
        if (processor->processor_thread_ && processor->processor_thread_->joinable()) {
            processor->processor_thread_->join();
        }
    }
}

uint64_t ProcessorManager::get_total_messages_processed() const {
    uint64_t total = 0;
    for (const auto& processor : processors_) {
        total += processor->get_messages_processed();
    }
    return total;
}

} // namespace MessageRouter