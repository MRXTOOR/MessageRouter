#pragma once

#include "message.h"
#include "lockfree_queue.h"
#include "config.h"
#include <atomic>
#include <thread>
#include <memory>
#include <vector>

namespace MessageRouter {

class Processor {
public:
    Processor(ProcessorId id, const SystemConfig* config, 
              std::shared_ptr<MessageQueue> input_queue,
              std::shared_ptr<MessageQueue> output_queue);
    ~Processor();
    
    void start();
    void stop();
    bool is_running() const { return running_.load(); }
    
    uint64_t get_messages_processed() const { return messages_processed_.load(); }
    
private:
    void processor_loop();
    void simulate_processing(Message& message);
    
    ProcessorId processor_id_;
    const SystemConfig* config_;
    std::shared_ptr<MessageQueue> input_queue_;
    std::shared_ptr<MessageQueue> output_queue_;
    
    std::atomic<bool> running_;
    std::unique_ptr<std::thread> processor_thread_;
    
    std::atomic<uint64_t> messages_processed_;
    
    friend class ProcessorManager;
};

class ProcessorManager {
public:
    ProcessorManager(const SystemConfig* config,
                    const std::vector<std::shared_ptr<MessageQueue>>& input_queues,
                    const std::vector<std::shared_ptr<MessageQueue>>& output_queues);
    ~ProcessorManager();
    
    void start_all();
    void stop_all();
    void wait_for_completion();
    
    uint64_t get_total_messages_processed() const;
    
private:
    const SystemConfig* config_;
    std::vector<std::shared_ptr<MessageQueue>> input_queues_;
    std::vector<std::shared_ptr<MessageQueue>> output_queues_;
    std::vector<std::unique_ptr<Processor>> processors_;
};

} // namespace MessageRouter