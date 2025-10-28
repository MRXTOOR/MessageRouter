




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



1. **baseline** - Базовый тест (4M сообщений/сек)
2. **hot_type** - Горячий тип (70% сообщений типа 0)
3. **burst_pattern** - Всплески трафика
4. **imbalanced_processing** - Несбалансированная обработка
5. **ordering_stress** - Стресс-тест упорядочивания
6. **strategy_bottleneck** - Узкое место стратегии



Все настройки находятся в файлах `configs/*.json`. Основные параметры:

- `producers.count` - Количество производителей
- `producers.messages_per_sec` - Скорость генерации сообщений
- `processors.count` - Количество процессоров
- `strategies.count` - Количество стратегий
- `duration_secs` - Длительность теста



Система рассчитана на:
- **10+ миллионов сообщений/сек** общей пропускной способности
- **< 5 микросекунд** латентности end-to-end (p99)
- **0 потерь сообщений**
- **0 нарушений порядка**



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

Или посмотрите полную документацию в `README.md`.
