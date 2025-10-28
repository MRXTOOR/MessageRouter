#include "../include/config.h"
#include <fstream>
#include <iostream>
#include <json/json.h>
#include <cmath>
#include <string>
#include <cstdlib>
#include <cstring>
#include <cstdio>

namespace MessageRouter {

std::unique_ptr<SystemConfig> SystemConfig::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open config file: " + filename);
    }
    
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errors;
    
    if (!Json::parseFromStream(builder, file, &root, &errors)) {
        throw std::runtime_error("JSON parse error: " + errors);
    }
    
    auto config = std::make_unique<SystemConfig>();
    
    
    config->scenario = root["scenario"].asString();
    config->duration_secs = root["duration_secs"].asInt();
    
    
    const auto& producers = root["producers"];
    config->producers.count = producers["count"].asInt();
    config->producers.messages_per_sec = producers["messages_per_sec"].asInt();
    
    const auto& distribution = producers["distribution"];
    for (const auto& key : distribution.getMemberNames()) {
        config->producers.distribution[key] = distribution[key].asDouble();
    }
    
    
    const auto& processors = root["processors"];
    config->processors.count = processors["count"].asInt();
    
    const auto& proc_times = processors["processing_times_ns"];
    for (const auto& key : proc_times.getMemberNames()) {
        config->processors.processing_times_ns[key] = proc_times[key].asUInt64();
    }
    
    
    const auto& strategies = root["strategies"];
    config->strategies.count = strategies["count"].asInt();
    
    const auto& strat_times = strategies["processing_times_ns"];
    for (const auto& key : strat_times.getMemberNames()) {
        config->strategies.processing_times_ns[key] = strat_times[key].asUInt64();
    }
    
    
    const auto& stage1_rules = root["stage1_rules"];
    for (const auto& rule : stage1_rules) {
        Stage1Rule r;
        r.msg_type = rule["msg_type"].asUInt();
        for (const auto& proc : rule["processors"]) {
            r.processors.push_back(proc.asUInt());
        }
        config->stage1_rules.push_back(r);
    }
    
    
    const auto& stage2_rules = root["stage2_rules"];
    for (const auto& rule : stage2_rules) {
        Stage2Rule r;
        r.msg_type = rule["msg_type"].asUInt();
        r.strategy = rule["strategy"].asUInt();
        r.ordering_required = rule["ordering_required"].asBool();
        config->stage2_rules.push_back(r);
    }
    
    return config;
}

bool SystemConfig::validate() const {
    if (producers.count <= 0 || processors.count <= 0 || strategies.count <= 0) {
        return false;
    }
    
    if (duration_secs <= 0) {
        return false;
    }
    
    return true;
}

ProcessorId SystemConfig::get_processor_for_message(MessageType msg_type) const {
    for (const auto& rule : stage1_rules) {
        if (rule.msg_type == msg_type && !rule.processors.empty()) {
            return rule.processors[0]; 
        }
    }
    return 0; 
}

StrategyId SystemConfig::get_strategy_for_message(MessageType msg_type) const {
    for (const auto& rule : stage2_rules) {
        if (rule.msg_type == msg_type) {
            return rule.strategy;
        }
    }
    return 0; 
}

bool SystemConfig::is_ordering_required(MessageType msg_type) const {
    for (const auto& rule : stage2_rules) {
        if (rule.msg_type == msg_type) {
            return rule.ordering_required;
        }
    }
    return false;
}

} // namespace MessageRouter