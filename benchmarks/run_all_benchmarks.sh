#!/bin/bash


echo "Запуск всех бенчмарков Message Router..."


mkdir -p results/benchmarks


run_benchmark() {
    local benchmark_name=$1
    local executable=$2
    local output_file="results/benchmarks/${benchmark_name}.json"
    
    echo "Запуск ${benchmark_name}..."
    
    if [ -f "./${executable}" ]; then
        ./${executable} --benchmark_format=json --benchmark_out=${output_file}
        echo "Результаты сохранены в ${output_file}"
    else
        echo "Ошибка: исполняемый файл ${executable} не найден"
        return 1
    fi
}


run_benchmark "queue_performance" "queue_perf"
run_benchmark "routing_latency" "routing_perf"
run_benchmark "memory_allocation" "memory_perf"
run_benchmark "scaling_benchmark" "scaling_perf"

echo "Все бенчмарки завершены!"
echo "Результаты доступны в директории results/benchmarks/"


echo "Создание сводного отчета..."
cat > results/benchmarks/summary.txt << EOF
=== СВОДНЫЙ ОТЧЕТ БЕНЧМАРКОВ ===
Дата: $(date)
Версия: Message Router v1.0

Запущенные бенчмарки:
1. queue_performance.json - Производительность очередей
2. routing_latency.json - Задержка маршрутизации
3. memory_allocation.json - Выделение памяти
4. scaling_benchmark.json - Масштабирование

Для просмотра результатов используйте:
- JSON файлы для детального анализа
- Этот файл для быстрого обзора

EOF

echo "Сводный отчет создан: results/benchmarks/summary.txt"
