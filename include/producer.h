#pragma once

#include "message.h"
#include "lockfree_queue.h"
#include "config.h"
#include <atomic>
#include <thread>
#include <memory>
#include <vector>

namespace MessageRouter {

class Producer {
public:
    Producer(ProducerId id, const SystemConfig* config, std::shared_ptr<MessageQueue> output_queue);
    ~Producer();
    
    void start();
    void stop();
    bool is_running() const { return running_.load(); }
    
    uint64_t get_messages_produced() const { return messages_produced_.load(); }
    
private:
    void producer_loop();
    
    ProducerId producer_id_;
    const SystemConfig* config_;
    std::shared_ptr<MessageQueue> output_queue_;
    
    std::atomic<bool> running_;
    std::unique_ptr<std::thread> producer_thread_;
    
    std::atomic<uint64_t> messages_produced_;
    SequenceNumber next_sequence_;
    
    friend class ProducerManager;
};

class ProducerManager {
public:
    ProducerManager(const SystemConfig* config, 
                   const std::vector<std::shared_ptr<MessageQueue>>& output_queues);
    ~ProducerManager();
    
    void start_all();
    void stop_all();
    void wait_for_completion();
    
    uint64_t get_total_messages_produced() const;
    
private:
    const SystemConfig* config_;
    std::vector<std::shared_ptr<MessageQueue>> output_queues_;
    std::vector<std::unique_ptr<Producer>> producers_;
};

} // namespace MessageRouter