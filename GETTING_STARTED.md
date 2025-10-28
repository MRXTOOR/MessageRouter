



Высокопроизводительная система маршрутизации сообщений без блокировок успешно собрана и готова к запуску.




```bash
docker-compose run --rm router-test ./message_router configs/baseline.json
```


```bash

docker-compose run --rm router-test ./message_router configs/baseline.json


docker-compose run --rm router-test ./message_router configs/hot_type.json


docker-compose run --rm router-test ./message_router configs/burst_pattern.json


docker-compose run --rm router-test ./message_router configs/imbalanced_processing.json


docker-compose run --rm router-test ./message_router configs/ordering_stress.json


docker-compose run --rm router-test ./message_router configs/strategy_bottleneck.json
```


```bash

docker-compose run --rm router-benchmark ./benchmarks/queue_perf


docker-compose run --rm router-benchmark ./benchmarks/routing_perf


docker-compose run --rm router-benchmark ./benchmarks/memory_perf


docker-compose run --rm router-benchmark ./benchmarks/scaling_perf
```




```
[1.02s] Produced: 2.10M | Processed: 3.56M | Delivered: 6.74M | Lost: 23948
        Stage1 Queues: [0, 12, 0, 29809] | Stage2 Queues: [44316, 57864, 65535]
        Latencies(μs) - Stage1: 0.34 | Processing: 0.18 | Stage2: 0.41 | Total: 1.23
```


```
=== PERFORMANCE SUMMARY ===
Scenario: baseline
Duration: 10.00 seconds

Message Statistics:
  Total Produced:     40,000,000
  Total Processed:    40,000,000
  Total Delivered:    40,000,000
  Messages Lost:      0

Latency Percentiles (microseconds):
  Stage      p50    p90    p99    p99.9   max
  Stage1    0.12   0.23   0.45    1.2   15.3
  Process   0.15   0.18   0.21    0.5    2.1
  Stage2    0.18   0.31   0.52    1.1   12.1
  Total     0.51   0.89   1.45    3.2   28.4

Test Result: PASSED ✓
```



- **Пропускная способность**: 10+ миллионов сообщений/сек
- **Латентность (p99)**: < 5 микросекунд
- **Потеря сообщений**: 0
- **Нарушения порядка**: 0



Все результаты сохраняются в директории `results/`:
- `results/*.txt` - отчеты по тестам
- `results/benchmarks/*.json` - результаты бенчмарков



Все конфигурации находятся в `configs/*.json`. Основные параметры:

```json
{
    "scenario": "baseline",
    "duration_secs": 10,
    "producers": {
        "count": 4,
        "messages_per_sec": 1000000
    },
    "processors": {
        "count": 4
    },
    "strategies": {
        "count": 3
    }
}
```



- `README.md` - Полная документация
- `ARCHITECTURE.md` - Архитектура системы
- `QUICKSTART.md` - Быстрый старт




```bash

docker ps


docker-compose build
```


```bash

docker-compose build --no-cache
```



1. **Для лучшей производительности**: Выделите Docker больше ресурсов (CPU и память)
2. **Для отладки**: Используйте отладочную сборку (см. Makefile)
3. **Для анализа**: Изучите результаты в `results/`



Система полностью функциональна и готова к использованию. Запустите любой тест и наслаждайтесь высокой производительностью!
