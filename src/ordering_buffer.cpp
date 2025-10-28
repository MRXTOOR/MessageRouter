#include "ordering_buffer.h"
#include <iostream>
#include <algorithm>

namespace MessageRouter {

OrderingBuffer::OrderingBuffer(size_t max_buffer_size)
    : max_buffer_size_(max_buffer_size)
    , messages_buffered_(0)
    , messages_sent_(0) {
}

bool OrderingBuffer::add_message(const Message& message, std::shared_ptr<MessageQueue> output_queue) {
    auto key = std::make_pair(message.producer_id, message.msg_type);

    
    auto it = expected_sequence_.find(key);
    if (it == expected_sequence_.end()) {
        
        expected_sequence_[key] = message.sequence_number + 1;
        messages_sent_.fetch_add(1);
        return output_queue->try_push(message);
    }

    if (message.sequence_number == it->second) {
        
        it->second = message.sequence_number + 1;
        messages_sent_.fetch_add(1);
        bool sent = output_queue->try_push(message);

        
        if (sent) {
            try_send_next(key, output_queue);
        }

        return sent;
    } else if (message.sequence_number > it->second) {
        
        
        it->second = message.sequence_number + 1;
        messages_sent_.fetch_add(1);
        return output_queue->try_push(message);
    } else {
        
        return true;
    }
}

void OrderingBuffer::flush_all(std::shared_ptr<MessageQueue> output_queue) {
    
    for (auto& [key, buffer] : buffers_) {
        if (buffer) {
            
            std::vector<Message> messages;
            MessageNode* current = buffer.get();
            while (current) {
                messages.push_back(current->message);
                current = current->next.get();
            }
            
            std::sort(messages.begin(), messages.end(), 
                     [](const Message& a, const Message& b) {
                         return a.sequence_number < b.sequence_number;
                     });
            
            for (const auto& msg : messages) {
                output_queue->try_push(msg);
                messages_sent_.fetch_add(1);
            }
        }
    }
    
    buffers_.clear();
    messages_buffered_.store(0);
}

size_t OrderingBuffer::get_buffer_size() const {
    return messages_buffered_.load();
}

bool OrderingBuffer::try_send_next(const std::pair<ProducerId, MessageType>& key, 
                                  std::shared_ptr<MessageQueue> output_queue) {
    auto buffer_it = buffers_.find(key);
    if (buffer_it == buffers_.end() || !buffer_it->second) {
        return false;
    }
    
    auto& buffer = buffer_it->second;
    auto expected_it = expected_sequence_.find(key);
    if (expected_it == expected_sequence_.end()) {
        return false;
    }
    
    SequenceNumber expected_seq = expected_it->second;
    
    
    MessageNode* prev = nullptr;
    MessageNode* current = buffer.get();
    
    while (current) {
        if (current->message.sequence_number == expected_seq) {
            
            bool sent = output_queue->try_push(current->message);
            if (sent) {
                messages_sent_.fetch_add(1);
                messages_buffered_.fetch_sub(1);
                expected_it->second = expected_seq + 1;
                
                
                if (prev) {
                    prev->next = std::move(current->next);
                } else {
                    buffer = std::move(current->next);
                }
                
                
                if (buffer) {
                    try_send_next(key, output_queue);
                } else {
                    buffers_.erase(buffer_it);
                }
                
                return true;
            }
            return false;
        }
        
        prev = current;
        current = current->next.get();
    }
    
    return false;
}

} // namespace MessageRouter
