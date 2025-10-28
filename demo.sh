#!/bin/bash


echo "=== ДЕМОНСТРАЦИЯ MESSAGE ROUTER ==="
echo "Высокопроизводительная система маршрутизации сообщений без блокировок"
echo ""


if ! command -v docker &> /dev/null; then
    echo "❌ Docker не найден. Установите Docker для запуска демонстрации."
    exit 1
fi

if ! command -v docker-compose &> /dev/null; then
    echo "❌ Docker Compose не найден. Установите Docker Compose для запуска демонстрации."
    exit 1
fi

echo "✅ Docker и Docker Compose найдены"
echo ""


mkdir -p results

echo "🔨 Сборка Docker образа..."
docker-compose build

if [ $? -ne 0 ]; then
    echo "❌ Ошибка сборки Docker образа"
    exit 1
fi

echo "✅ Docker образ собран успешно"
echo ""

echo "🚀 Запуск демонстрации..."
echo ""


echo "=== ТЕСТ 1: Базовый сценарий (4M сообщений/сек) ==="
docker-compose run --rm router-test ./message_router configs/baseline.json
echo ""


echo "=== ТЕСТ 2: Горячий тип сообщений (70% тип-0) ==="
docker-compose run --rm router-test ./message_router configs/hot_type.json
echo ""


echo "=== ТЕСТ 3: Всплески трафика ==="
docker-compose run --rm router-test ./message_router configs/burst_pattern.json
echo ""


echo "=== ТЕСТ 4: Несбалансированная обработка ==="
docker-compose run --rm router-test ./message_router configs/imbalanced_processing.json
echo ""


echo "=== ТЕСТ 5: Стресс-тест упорядочивания ==="
docker-compose run --rm router-test ./message_router configs/ordering_stress.json
echo ""


echo "=== ТЕСТ 6: Узкое место стратегии ==="
docker-compose run --rm router-test ./message_router configs/strategy_bottleneck.json
echo ""

echo "📊 Запуск бенчмарков производительности..."
echo ""


echo "=== БЕНЧМАРК: Производительность очередей ==="
docker-compose run --rm router-benchmark ./benchmarks/queue_perf --benchmark_format=console
echo ""

echo "=== БЕНЧМАРК: Задержка маршрутизации ==="
docker-compose run --rm router-benchmark ./benchmarks/routing_perf --benchmark_format=console
echo ""

echo "=== БЕНЧМАРК: Выделение памяти ==="
docker-compose run --rm router-benchmark ./benchmarks/memory_perf --benchmark_format=console
echo ""

echo "=== БЕНЧМАРК: Масштабирование ==="
docker-compose run --rm router-benchmark ./benchmarks/scaling_perf --benchmark_format=console
echo ""

echo "✅ Демонстрация завершена!"
echo ""
echo "📁 Результаты сохранены в директории results/"
echo "📊 Для просмотра детальных результатов используйте:"
echo "   - cat results/*_summary.txt"
echo "   - ls results/benchmarks/"
echo ""
echo "🎯 Система готова к использованию!"
echo ""
echo "💡 Дополнительные команды:"
echo "   - make help          
echo "   - make test          
echo "   - make benchmark     
echo "   - make docker-test   
echo "   - make docker-benchmark 
