#pragma once

#include "message.h"
#include "lockfree_queue.h"
#include "config.h"
#include <atomic>
#include <thread>
#include <memory>
#include <vector>
#include <map>

namespace MessageRouter {

class Strategy {
public:
    Strategy(StrategyId id, const SystemConfig* config, 
             std::shared_ptr<MessageQueue> input_queue);
    ~Strategy();
    
    void start();
    void stop();
    bool is_running() const { return running_.load(); }
    
    uint64_t get_messages_delivered() const { return messages_delivered_.load(); }
    uint64_t get_ordering_violations() const { return ordering_violations_.load(); }
    
private:
    void strategy_loop();
    void process_message(const Message& message);
    void simulate_strategy_processing();
    
    StrategyId strategy_id_;
    const SystemConfig* config_;
    std::shared_ptr<MessageQueue> input_queue_;
    
    std::atomic<bool> running_;
    std::unique_ptr<std::thread> strategy_thread_;
    
    std::atomic<uint64_t> messages_delivered_;
    std::atomic<uint64_t> ordering_violations_;
    
    std::map<std::pair<ProducerId, MessageType>, SequenceNumber> expected_sequence_;
    
    friend class StrategyManager;
};

class StrategyManager {
public:
    StrategyManager(const SystemConfig* config,
                   const std::vector<std::shared_ptr<MessageQueue>>& input_queues);
    ~StrategyManager();
    
    void start_all();
    void stop_all();
    void wait_for_completion();
    
    uint64_t get_total_messages_delivered() const;
    uint64_t get_total_ordering_violations() const;
    
private:
    const SystemConfig* config_;
    std::vector<std::shared_ptr<MessageQueue>> input_queues_;
    std::vector<std::unique_ptr<Strategy>> strategies_;
};

} // namespace MessageRouter