# Multi-Stage Lock-Free Message Router

Высокопроизводительная система маршрутизации сообщений без блокировок, способная обрабатывать миллионы сообщений в секунду через несколько этапов обработки.



Система состоит из трех типов компонентов, соединенных в конвейер:

```
Producers (4-8 потоков) → Stage1 Router → Processors (4-8 потоков) → Stage2 Router → Strategies (2-4 потока)
```



- **Producers** - Генерируют сообщения с высокой скоростью
- **Processors** - Преобразуют сообщения с симуляцией работы
- **Strategies** - Конечные потребители, проверяющие порядок
- **Routers** - Маршрутизируют сообщения между этапами
- **Queues** - Lock-free очереди для передачи сообщений



- Linux (Ubuntu 22+ рекомендуется)
- C++20 или новее
- Clang 19+
- Docker 20+ и Docker Compose 2+
- Google Benchmark library
- JSON library (jsoncpp)





```bash

git clone <your-repo>
cd message-router


docker-compose build


docker-compose run router-test ./message_router configs/baseline.json


docker-compose run router-test


docker-compose run router-benchmark
```



```bash

sudo apt-get update
sudo apt-get install -y build-essential cmake libbenchmark-dev libjsoncpp-dev


mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native" ..
make -j$(nproc)


./message_router ../configs/baseline.json


cd benchmarks
make
./queue_perf
```




- 4 производителя × 1M сообщений/сек = 4M общих/сек
- Равномерное распределение по 4 типам сообщений
- Проверка: Все сообщения доставлены в порядке


- 70% сообщений типа 0 (создание горячей точки)
- 10% для типов 1-3
- Проверка: Система справляется с несбалансированной нагрузкой


- Чередующийся паттерн каждые 2 секунды:
  - 200ms всплеск: 5× нормальная скорость (20M сообщений/сек)
  - 1800ms тихо: 0.5× нормальная скорость (2M сообщений/сек)
- Проверка: Очереди справляются со всплесками без потерь


- Разные времена обработки по типам сообщений:
  - Тип-0: 50 наносекунд (быстро)
  - Тип-1: 500 наносекунд (средне)
  - Тип-2: 2000 наносекунд (медленно - узкое место)
  - Тип-3: 100 наносекунд (быстро)
- Проверка: Медленный процессор не блокирует остальные


- Все производители отправляют только сообщения типа 0
- Все идут через один процессор к одной стратегии
- 8M сообщений/сек общих
- Проверка: Идеальный порядок несмотря на экстремальную конкуренцию


- Стратегия-0 обрабатывает медленно (1000нс на сообщение)
- Стратегии 1-2 обрабатывают быстро (50нс на сообщение)
- Проверка: Обработка обратного давления без потери сообщений



```bash

docker-compose run router-test ./message_router configs/baseline.json
docker-compose run router-test ./message_router configs/hot_type.json
docker-compose run router-test ./message_router configs/burst_pattern.json
docker-compose run router-test ./message_router configs/imbalanced_processing.json
docker-compose run router-test ./message_router configs/ordering_stress.json
docker-compose run router-test ./message_router configs/strategy_bottleneck.json


docker-compose run router-test
```





1. **queue_perf** - Производительность очередей
2. **routing_perf** - Задержка маршрутизации
3. **memory_perf** - Выделение памяти
4. **scaling_perf** - Масштабирование



```bash

docker-compose run router-benchmark ./benchmarks/queue_perf
docker-compose run router-benchmark ./benchmarks/routing_perf
docker-compose run router-benchmark ./benchmarks/memory_perf
docker-compose run router-benchmark ./benchmarks/scaling_perf


docker-compose run router-benchmark-all


cd benchmarks
make
./queue_perf
./routing_perf
./memory_perf
./scaling_perf
```



Результаты сохраняются в директории `results/`:

- `results/baseline_summary.txt` - Результаты базового теста
- `results/hot_type_summary.txt` - Результаты теста горячего типа
- `results/benchmarks/` - Результаты бенчмарков в JSON формате



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

Ordering Validation:
  Producer 0: 10,000,000 messages - IN ORDER ✓
  Producer 1: 10,000,000 messages - IN ORDER ✓
  Producer 2: 10,000,000 messages - IN ORDER ✓
  Producer 3: 10,000,000 messages - IN ORDER ✓
  
Test Result: PASSED
```





- **Общая пропускная способность**: 10+ миллионов сообщений/сек
- **Латентность end-to-end (p99)**: < 5 микросекунд
- **Потеря сообщений**: 0
- **Нарушения порядка**: 0



- Lock-free очереди (SPSC/MPSC/MPMC)
- Выравнивание по кэш-линиям
- Минимизация выделения памяти на горячем пути
- Busy waiting для минимальной латентности
- Атомарные операции для синхронизации



### Lock-Free Design

- Никаких мьютексов, семафоров или условных переменных
- Только атомарные операции
- Lock-free очереди для передачи сообщений



- Предварительное выделение буферов
- Выравнивание структур по кэш-линиям
- Минимизация копирования данных



- Неблокирующие операции записи в очереди
- Отслеживание потери сообщений
- Мониторинг глубины очередей



```
message-router/
├── Dockerfile              
├── docker-compose.yml      
├── CMakeLists.txt          
├── configs/                
│   ├── baseline.json
│   ├── hot_type.json
│   ├── burst_pattern.json
│   ├── imbalanced_processing.json
│   ├── ordering_stress.json
│   └── strategy_bottleneck.json
├── include/                
│   ├── message.h
│   ├── lockfree_queue.h
│   ├── producer.h
│   ├── processor.h
│   ├── strategy.h
│   ├── monitor.h
│   └── config.h
├── src/                    
│   ├── main.cpp
│   ├── message.cpp
│   ├── lockfree_queue.cpp
│   ├── producer.cpp
│   ├── processor.cpp
│   ├── strategy.cpp
│   ├── monitor.cpp
│   └── config.cpp
├── benchmarks/             
│   ├── CMakeLists.txt
│   ├── queue_performance.cpp
│   ├── routing_latency.cpp
│   ├── memory_allocation.cpp
│   ├── scaling_benchmark.cpp
│   ├── run_all_benchmarks.cpp
│   └── run_all_benchmarks.sh
└── results/                
    └── benchmarks/
```





1. Создайте новый JSON файл в `configs/`
2. Добавьте логику в `main.cpp` для обработки сценария
3. Обновите `docker-compose.yml` при необходимости



1. Создайте новый файл в `benchmarks/`
2. Добавьте исполняемый файл в `benchmarks/CMakeLists.txt`
3. Обновите `run_all_benchmarks.sh`



```bash

cmake -DCMAKE_BUILD_TYPE=Debug ..
make


./message_router configs/baseline.json --debug


perf record ./message_router configs/baseline.json
perf report
```



MIT License



Message Router Development Team
