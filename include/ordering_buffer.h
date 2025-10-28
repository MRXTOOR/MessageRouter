#pragma once

#include "message.h"
#include "lockfree_queue.h"
#include <unordered_map>
#include <atomic>
#include <memory>

namespace MessageRouter {

struct PairHash {
    std::size_t operator()(const std::pair<ProducerId, MessageType>& p) const {
        return std::hash<ProducerId>()(p.first) ^ (std::hash<MessageType>()(p.second) << 1);
    }
};

class OrderingBuffer {
public:
    OrderingBuffer(size_t max_buffer_size = 1000);
    
    bool add_message(const Message& message, std::shared_ptr<MessageQueue> output_queue);
    
    void flush_all(std::shared_ptr<MessageQueue> output_queue);
    
    size_t get_buffer_size() const;
    uint64_t get_messages_buffered() const { return messages_buffered_.load(); }
    uint64_t get_messages_sent() const { return messages_sent_.load(); }

private:
    struct MessageNode {
        Message message;
        std::unique_ptr<MessageNode> next;
        
        MessageNode(const Message& msg) : message(msg), next(nullptr) {}
    };
    
    std::unordered_map<std::pair<ProducerId, MessageType>, 
                      std::unique_ptr<MessageNode>, PairHash> buffers_;
    
    std::unordered_map<std::pair<ProducerId, MessageType>, 
                      SequenceNumber, PairHash> expected_sequence_;
    
    size_t max_buffer_size_;
    std::atomic<uint64_t> messages_buffered_;
    std::atomic<uint64_t> messages_sent_;
    
    bool try_send_next(const std::pair<ProducerId, MessageType>& key, 
                      std::shared_ptr<MessageQueue> output_queue);
};

} // namespace MessageRouter
