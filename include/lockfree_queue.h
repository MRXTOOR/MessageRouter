#pragma once

#include "message.h"
#include <atomic>
#include <memory>

namespace MessageRouter {

// Simple SPSC (Single Producer Single Consumer) queue
template<typename T, size_t Size>
class LockFreeSPSCQueue {
private:
    static_assert((Size & (Size - 1)) == 0, "Size must be power of 2");
    
    struct alignas(64) Node {
        T data;
        std::atomic<bool> ready{false};
    };
    
    alignas(64) std::unique_ptr<Node[]> buffer_;
    alignas(64) std::atomic<size_t> head_{0};
    alignas(64) std::atomic<size_t> tail_{0};
    
public:
    LockFreeSPSCQueue() : buffer_(std::make_unique<Node[]>(Size)) {}
    
    bool try_push(const T& item) {
        const size_t current_tail = tail_.load(std::memory_order_relaxed);
        const size_t next_tail = (current_tail + 1) & (Size - 1);
        
        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false; // Queue is full
        }
        
        buffer_[current_tail].data = item;
        buffer_[current_tail].ready.store(true, std::memory_order_release);
        tail_.store(next_tail, std::memory_order_release);
        
        return true;
    }
    
    bool try_pop(T& item) {
        const size_t current_head = head_.load(std::memory_order_relaxed);
        
        if (current_head == tail_.load(std::memory_order_acquire)) {
            return false; // Queue is empty
        }
        
        if (!buffer_[current_head].ready.load(std::memory_order_acquire)) {
            return false; // Data not ready yet
        }
        
        item = buffer_[current_head].data;
        buffer_[current_head].ready.store(false, std::memory_order_release);
        head_.store((current_head + 1) & (Size - 1), std::memory_order_release);
        
        return true;
    }
    
    size_t size() const {
        const size_t current_tail = tail_.load(std::memory_order_relaxed);
        const size_t current_head = head_.load(std::memory_order_relaxed);
        return (current_tail - current_head) & (Size - 1);
    }
    
    bool empty() const {
        return head_.load(std::memory_order_relaxed) == tail_.load(std::memory_order_relaxed);
    }
    
    static constexpr size_t capacity() {
        return Size - 1;
    }
};

// Aliases for convenience
using MessageQueue = LockFreeSPSCQueue<Message, 1048576>; // 1M messages

} // namespace MessageRouter