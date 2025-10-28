#pragma once

#include "message.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace MessageRouter {

struct ProducerConfig {
    int count;
    int messages_per_sec;
    std::map<std::string, double> distribution;
};

struct ProcessorConfig {
    int count;
    std::map<std::string, uint64_t> processing_times_ns;
};

struct StrategyConfig {
    int count;
    std::map<std::string, uint64_t> processing_times_ns;
};

struct Stage1Rule {
    MessageType msg_type;
    std::vector<ProcessorId> processors;
};

struct Stage2Rule {
    MessageType msg_type;
    StrategyId strategy;
    bool ordering_required;
};

struct SystemConfig {
    std::string scenario;
    int duration_secs;
    ProducerConfig producers;
    ProcessorConfig processors;
    StrategyConfig strategies;
    std::vector<Stage1Rule> stage1_rules;
    std::vector<Stage2Rule> stage2_rules;
    
    static std::unique_ptr<SystemConfig> load_from_file(const std::string& filename);
    bool validate() const;
    
    ProcessorId get_processor_for_message(MessageType msg_type) const;
    StrategyId get_strategy_for_message(MessageType msg_type) const;
    bool is_ordering_required(MessageType msg_type) const;
};

} // namespace MessageRouter