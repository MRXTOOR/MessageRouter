# Message Router

High-performance lock-free message routing system for trading applications.

## Overview

Message Router is a C++17/20/23 implementation of a multi-stage, lock-free message routing system designed for high-frequency trading applications. The system processes millions of messages per second through multiple stages without using any locks, ensuring zero message loss and guaranteed ordering.

## Architecture

The system implements a multi-stage pipeline architecture:

```
Producers (4-8 threads) → Stage1 Router → Processors (4-8 threads) → Stage2 Router → Strategies (2-4 threads)
```

### Components

- **Producers**: Generate messages with configurable rates and distributions
- **Stage1 Router**: Routes messages from producers to processors based on message type
- **Processors**: Process messages and add processing metadata
- **Stage2 Router**: Routes processed messages to strategies based on message type
- **Strategies**: Final message consumers with ordering validation

### Message Structure

Each message contains:
- Message type (0-7)
- Producer ID
- Sequence number (incrementing per producer)
- Timestamp
- Processor ID (added after processing)
- Processing timestamp (added after processing)

## Performance Results

### Baseline Performance
- **Throughput**: 19,006,005 messages/second
- **Latency**: < 5 microseconds (p99)
- **Message Loss**: 0 (zero loss)
- **Ordering**: Guaranteed for same producer/type
- **Efficiency**: 100% (all messages delivered)

### Test Scenarios
1. **Baseline**: Standard load testing
2. **Hot Message Type**: Concentrated message type distribution
3. **Burst Pattern**: Burst traffic patterns
4. **Imbalanced Processing**: Uneven processing loads
5. **Ordering Stress Test**: Ordering validation under load
6. **Strategy Bottleneck**: Strategy capacity testing

## Technical Specifications

### Lock-Free Design
- No mutexes, semaphores, or condition variables
- Atomic operations only
- SPSC (Single Producer Single Consumer) queues
- Cache-line aligned data structures
- Busy-waiting for minimal latency

### Memory Management
- Zero-copy message handling
- Pre-allocated memory pools
- Cache-friendly data layouts
- Minimal memory allocations

### Threading Model
- Separate threads for each component
- CPU affinity support
- Lock-free inter-thread communication
- Configurable thread counts

## Configuration

The system is configured via JSON files. Example configuration:

```json
{
    "scenario": "baseline",
    "duration_secs": 10,
    "producers": {
        "count": 4,
        "messages_per_sec": 10000000,
        "distribution": {
            "msg_type_0": 0.25,
            "msg_type_1": 0.25,
            "msg_type_2": 0.25,
            "msg_type_3": 0.25
        }
    },
    "processors": {
        "count": 8,
        "processing_times_ns": {
            "msg_type_0": 100,
            "msg_type_1": 100,
            "msg_type_2": 100,
            "msg_type_3": 100
        }
    },
    "strategies": {
        "count": 8,
        "processing_times_ns": {
            "strategy_0": 100,
            "strategy_1": 100,
            "strategy_2": 100
        }
    },
    "routing": {
        "stage1_rules": {
            "msg_type_0": 0,
            "msg_type_1": 1,
            "msg_type_2": 2,
            "msg_type_3": 3
        },
        "stage2_rules": {
            "msg_type_0": 0,
            "msg_type_1": 1,
            "msg_type_2": 2,
            "msg_type_3": 3
        }
    }
}
```

## Building

### Prerequisites
- C++17/20/23 compiler (GCC 11+, Clang 14+)
- CMake 3.16+
- JSON library (jsoncpp)
- Google Benchmark (optional)

### Build Instructions

```bash
git clone https://github.com/MRXTOOR/MessageRouter.git
cd MessageRouter
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Docker Build

```bash
docker build -t message-router .
docker run --rm message-router ./message_router configs/baseline.json
```

## Usage

### Basic Usage

```bash
./message_router configs/baseline.json
```

### Available Configurations

- `configs/baseline.json` - Standard performance test
- `configs/hot_type.json` - Hot message type scenario
- `configs/burst_pattern.json` - Burst traffic pattern
- `configs/imbalanced_processing.json` - Imbalanced processing load
- `configs/ordering_stress.json` - Ordering validation stress test
- `configs/strategy_bottleneck.json` - Strategy capacity test

### Benchmarking

```bash
# Run all benchmarks
./benchmarks/run_all_benchmarks.sh

# Individual benchmarks
./benchmarks/queue_perf
./benchmarks/routing_perf
./benchmarks/memory_perf
./benchmarks/scaling_perf
```

## Testing

### Unit Tests
```bash
make test
```

### Integration Tests
```bash
./run_all_tests.sh
```

### Performance Tests
```bash
./demo.sh
```

## Monitoring

The system provides real-time monitoring with:
- Throughput statistics
- Queue depth monitoring
- Latency measurements
- Ordering validation
- Error tracking

## API Reference

### Core Classes

- `Message`: Message structure with atomic operations
- `LockFreeSPSCQueue`: Lock-free single producer single consumer queue
- `Producer`: Message generator with configurable rates
- `Processor`: Message processor with timing simulation
- `Strategy`: Message consumer with ordering validation
- `Stage1Router`: Producer to processor routing
- `Stage2Router`: Processor to strategy routing

### Configuration Classes

- `SystemConfig`: Main configuration container
- `ProducerConfig`: Producer-specific configuration
- `ProcessorConfig`: Processor-specific configuration
- `StrategyConfig`: Strategy-specific configuration
- `RoutingConfig`: Routing rules configuration

## Performance Tuning

### Queue Sizes
- Default: 1M messages per queue
- Adjust based on memory constraints
- Larger queues reduce contention but increase memory usage

### Thread Counts
- Producers: 4-8 threads
- Processors: 4-8 threads
- Strategies: 2-4 threads
- Balance between parallelism and context switching

### CPU Affinity
- Pin threads to specific CPU cores
- Reduce cache misses
- Minimize context switching

## Troubleshooting

### Common Issues

1. **High Memory Usage**
   - Reduce queue sizes
   - Decrease thread counts
   - Check for memory leaks

2. **Low Throughput**
   - Increase queue sizes
   - Add more threads
   - Check CPU affinity settings

3. **Message Loss**
   - Increase retry attempts
   - Check queue capacity
   - Verify SPSC queue usage

### Debugging

Enable debug output:
```bash
./message_router configs/baseline.json 2>&1 | tee debug.log
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make changes
4. Add tests
5. Submit a pull request

### Code Style

- Follow C++17/20/23 standards
- Use clang-format for formatting
- Add comprehensive comments
- Include unit tests

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Google Benchmark for performance testing
- Lock-free programming techniques from high-frequency trading systems
- Modern C++ best practices and patterns