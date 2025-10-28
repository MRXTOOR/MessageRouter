#pragma once

#include "message.h"
#include "lockfree_queue.h"
#include "config.h"
#include <vector>
#include <memory>
#include <atomic>
#include <thread>

namespace MessageRouter {

class Stage2Router {
public:
    Stage2Router(const SystemConfig* config,
                 const std::vector<std::shared_ptr<MessageQueue>>& input_queues,
                 const std::vector<std::shared_ptr<MessageQueue>>& output_queues);

    ~Stage2Router();

    void start();
    void stop();
    bool is_running() const { return running_.load(); }

    uint64_t get_messages_routed() const { return messages_routed_.load(); }
    uint64_t get_routing_errors() const { return routing_errors_.load(); }

private:
    void routing_loop();

    const SystemConfig* config_;
    std::vector<std::shared_ptr<MessageQueue>> input_queues_;
    std::vector<std::shared_ptr<MessageQueue>> output_queues_;

    std::atomic<bool> running_;
    std::unique_ptr<std::thread> router_thread_;

    std::atomic<uint64_t> messages_routed_;
    std::atomic<uint64_t> routing_errors_;
};

} // namespace MessageRouter