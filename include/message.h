#pragma once

#include <atomic>
#include <cstdint>
#include <cstring>

namespace MessageRouter {

using MessageType = uint8_t;
using ProducerId = uint32_t;
using ProcessorId = uint32_t;
using StrategyId = uint32_t;
using SequenceNumber = uint64_t;

struct alignas(64) Message {
    MessageType msg_type;
    ProducerId producer_id;
    SequenceNumber sequence_number;
    uint64_t timestamp;
    
    ProcessorId processor_id;
    uint64_t processing_timestamp;
    
    Message() = default;
    
    Message(MessageType type, ProducerId prod_id, SequenceNumber seq, uint64_t ts)
        : msg_type(type), producer_id(prod_id), sequence_number(seq), timestamp(ts)
        , processor_id(0), processing_timestamp(0) {}
    
    Message(const Message& other) = default;
    
    Message& operator=(const Message& other) = default;
    
    ~Message() = default;
};

} // namespace MessageRouter