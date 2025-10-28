




```bash
cd MessageRouter
```


```bash
./demo.sh
```


```bash
ls results/
cat results/*_summary.txt
```




```bash

docker-compose build


docker-compose run router-test


docker-compose run router-benchmark


docker-compose run router-test ./message_router configs/baseline.json
```


```bash

sudo apt-get install -y build-essential cmake clang libbenchmark-dev libjsoncpp-dev


make build


make test


make benchmark
```



1. **baseline** - Baseline test (4M messages/sec)
2. **hot_type** - Hot type (70% type-0 messages)
3. **burst_pattern** - Traffic bursts
4. **imbalanced_processing** - Imbalanced processing
5. **ordering_stress** - Ordering stress test
6. **strategy_bottleneck** - Strategy bottleneck



All settings are in `configs/*.json` files. Main parameters:

- `producers.count` - Number of producers
- `producers.messages_per_sec` - Message generation rate
- `processors.count` - Number of processors
- `strategies.count` - Number of strategies
- `duration_secs` - Test duration



The system is designed for:
- **10+ million messages/sec** total throughput
- **< 5 microseconds** end-to-end latency (p99)
- **0 message loss**
- **0 ordering violations**



```bash

make debug


make profile


make check-deps
```



```
results/
├── baseline_summary.txt
├── hot_type_summary.txt
├── burst_pattern_summary.txt
├── imbalanced_processing_summary.txt
├── ordering_stress_summary.txt
├── strategy_bottleneck_summary.txt
└── benchmarks/
    ├── queue_performance.json
    ├── routing_latency.json
    ├── memory_allocation.json
    └── scaling_benchmark.json
```



```bash
make help
```

Or see the full documentation in `README.md`.
