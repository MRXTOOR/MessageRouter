#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include <memory>
#include <string>
#include "message.h"
#include "producer.h"
#include "processor.h"
#include "strategy.h"

namespace MessageRouter {

class Monitor {
private:
    const SystemConfig* config_;
    ProducerManager* producer_manager_;
    ProcessorManager* processor_manager_;
    StrategyManager* strategy_manager_;
    
    std::vector<std::shared_ptr<MessageQueue>> stage1_queues_;
    std::vector<std::shared_ptr<MessageQueue>> stage2_queues_;
    
    PerformanceStats stats_;
    
    std::atomic<bool> running_{false};
    std::unique_ptr<std::thread> monitor_thread_;
    
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::steady_clock::time_point last_report_time_;
    
    void monitor_loop();
    
    void update_stats();
    
    void print_report();
    
    void calculate_percentiles(const std::vector<uint64_t>& latencies,
                              uint64_t& p50, uint64_t& p90, uint64_t& p99, uint64_t& p99_9) const;
    
public:
    Monitor(const SystemConfig* config,
            ProducerManager* producer_manager,
            ProcessorManager* processor_manager,
            StrategyManager* strategy_manager,
            const std::vector<std::shared_ptr<MessageQueue>>& stage1_queues,
            const std::vector<std::shared_ptr<MessageQueue>>& stage2_queues);
    
    ~Monitor();
    
    void start();
    
    void stop();
    
    void wait_for_completion();
    
    const PerformanceStats& get_stats() const { return stats_; }
    
    void reset_stats();
    
    void print_final_report();
};

} // namespace MessageRouter
