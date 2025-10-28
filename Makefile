


BUILD_DIR = build
BENCHMARK_DIR = benchmarks
CONFIG_DIR = configs
RESULTS_DIR = results


CXX = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -O3 -march=native -DNDEBUG
LDFLAGS = -lbenchmark -ljsoncpp -lpthread


.PHONY: all clean build test benchmark docker-build docker-test docker-benchmark help

all: build


build:
	@echo "Сборка Message Router..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="$(CXXFLAGS)" ..
	@cd $(BUILD_DIR) && make -j$$(nproc)
	@echo "Сборка завершена!"


clean:
	@echo "Очистка..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(RESULTS_DIR)
	@echo "Очистка завершена!"


test: build
	@echo "Запуск тестов..."
	@cd $(BUILD_DIR) && ./run_all_tests.sh


benchmark: build
	@echo "Запуск бенчмарков..."
	@cd $(BUILD_DIR) && ./benchmarks/run_all_benchmarks.sh


docker-build:
	@echo "Сборка Docker образа..."
	@docker-compose build


docker-test:
	@echo "Запуск тестов в Docker..."
	@docker-compose run router-test


docker-benchmark:
	@echo "Запуск бенчмарков в Docker..."
	@docker-compose run router-benchmark


docker-test-all:
	@echo "Запуск всех тестов в Docker..."
	@docker-compose run router-test ./message_router configs/baseline.json
	@docker-compose run router-test ./message_router configs/hot_type.json
	@docker-compose run router-test ./message_router configs/burst_pattern.json
	@docker-compose run router-test ./message_router configs/imbalanced_processing.json
	@docker-compose run router-test ./message_router configs/ordering_stress.json
	@docker-compose run router-test ./message_router configs/strategy_bottleneck.json


test-baseline: build
	@echo "Запуск базового теста..."
	@cd $(BUILD_DIR) && ./message_router ../configs/baseline.json

test-hot-type: build
	@echo "Запуск теста горячего типа..."
	@cd $(BUILD_DIR) && ./message_router ../configs/hot_type.json

test-burst: build
	@echo "Запуск теста всплесков..."
	@cd $(BUILD_DIR) && ./message_router ../configs/burst_pattern.json

test-imbalanced: build
	@echo "Запуск теста несбалансированной обработки..."
	@cd $(BUILD_DIR) && ./message_router ../configs/imbalanced_processing.json

test-ordering: build
	@echo "Запуск теста упорядочивания..."
	@cd $(BUILD_DIR) && ./message_router ../configs/ordering_stress.json

test-bottleneck: build
	@echo "Запуск теста узкого места..."
	@cd $(BUILD_DIR) && ./message_router ../configs/strategy_bottleneck.json


install-deps:
	@echo "Установка зависимостей..."
	@sudo apt-get update
	@sudo apt-get install -y build-essential cmake clang libbenchmark-dev libjsoncpp-dev


check-deps:
	@echo "Проверка зависимостей..."
	@which clang++ || (echo "Ошибка: clang++ не найден" && exit 1)
	@which cmake || (echo "Ошибка: cmake не найден" && exit 1)
	@pkg-config --exists benchmark || (echo "Ошибка: libbenchmark не найден" && exit 1)
	@pkg-config --exists jsoncpp || (echo "Ошибка: libjsoncpp не найден" && exit 1)
	@echo "Все зависимости найдены!"


debug:
	@echo "Сборка в режиме отладки..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug ..
	@cd $(BUILD_DIR) && make -j$$(nproc)
	@echo "Отладочная сборка завершена!"


profile: build
	@echo "Запуск анализа производительности..."
	@cd $(BUILD_DIR) && perf record ./message_router ../configs/baseline.json
	@cd $(BUILD_DIR) && perf report


help:
	@echo "Доступные команды:"
	@echo "  build          - Сборка проекта"
	@echo "  clean          - Очистка файлов сборки"
	@echo "  test           - Запуск всех тестов"
	@echo "  benchmark      - Запуск бенчмарков"
	@echo "  docker-build   - Сборка Docker образа"
	@echo "  docker-test    - Запуск тестов в Docker"
	@echo "  docker-benchmark - Запуск бенчмарков в Docker"
	@echo "  test-<name>    - Запуск конкретного теста"
	@echo "  install-deps   - Установка зависимостей"
	@echo "  check-deps     - Проверка зависимостей"
	@echo "  debug          - Отладочная сборка"
	@echo "  profile        - Анализ производительности"
	@echo "  help           - Показать эту справку"
