#!/bin/bash


echo "Запуск всех тестовых сценариев Message Router..."


mkdir -p results


run_test() {
    local test_name=$1
    local config_file=$2
    local output_file="results/${test_name}_summary.txt"
    
    echo "Запуск теста: ${test_name}..."
    echo "Конфигурация: ${config_file}"
    
    if [ -f "${config_file}" ]; then
        ./message_router ${config_file} > ${output_file} 2>&1
        echo "Результаты сохранены в ${output_file}"
        
        
        if grep -q "Test Result: PASSED" ${output_file}; then
            echo "✓ ${test_name}: PASSED"
        else
            echo "✗ ${test_name}: FAILED"
        fi
    else
        echo "Ошибка: файл конфигурации ${config_file} не найден"
        return 1
    fi
}


if [ ! -f "./message_router" ]; then
    echo "Ошибка: исполняемый файл message_router не найден"
    echo "Сначала соберите проект: mkdir build && cd build && cmake .. && make"
    exit 1
fi


run_test "baseline" "configs/baseline.json"
run_test "hot_type" "configs/hot_type.json"
run_test "burst_pattern" "configs/burst_pattern.json"
run_test "imbalanced_processing" "configs/imbalanced_processing.json"
run_test "ordering_stress" "configs/ordering_stress.json"
run_test "strategy_bottleneck" "configs/strategy_bottleneck.json"

echo ""
echo "Все тесты завершены!"
echo "Результаты доступны в директории results/"


echo "Создание сводного отчета..."
cat > results/test_summary.txt << EOF
=== СВОДНЫЙ ОТЧЕТ ТЕСТОВ ===
Дата: $(date)
Версия: Message Router v1.0

Запущенные тесты:
1. baseline - Базовый тест (4M сообщений/сек)
2. hot_type - Тест горячего типа (70% тип-0)
3. burst_pattern - Тест всплесков трафика
4. imbalanced_processing - Тест несбалансированной обработки
5. ordering_stress - Тест стресса упорядочивания
6. strategy_bottleneck - Тест узкого места стратегии

Результаты:
EOF


for test in baseline hot_type burst_pattern imbalanced_processing ordering_stress strategy_bottleneck; do
    if [ -f "results/${test}_summary.txt" ]; then
        echo "" >> results/test_summary.txt
        echo "=== ${test} ===" >> results/test_summary.txt
        grep -E "(Test Result:|Total Produced:|Total Processed:|Total Delivered:|Messages Lost:|Total Ordering Violations:)" results/${test}_summary.txt >> results/test_summary.txt
    fi
done

echo "Сводный отчет создан: results/test_summary.txt"
