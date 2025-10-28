


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
	@echo "Building Message Router..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="$(CXXFLAGS)" ..
	@cd $(BUILD_DIR) && make -j$$(nproc)
	@echo "Build completed!"


clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(RESULTS_DIR)
	@echo "Cleaning completed!"


test: build
	@echo "Running tests..."
	@cd $(BUILD_DIR) && ./run_all_tests.sh


benchmark: build
	@echo "Running benchmarks..."
	@cd $(BUILD_DIR) && ./benchmarks/run_all_benchmarks.sh


docker-build:
	@echo "Building Docker image..."
	@docker-compose build


docker-test:
	@echo "Running tests in Docker..."
	@docker-compose run router-test


docker-benchmark:
	@echo "Running benchmarks in Docker..."
	@docker-compose run router-benchmark


docker-test-all:
	@echo "Running all tests in Docker..."
	@docker-compose run router-test ./message_router configs/baseline.json
	@docker-compose run router-test ./message_router configs/hot_type.json
	@docker-compose run router-test ./message_router configs/burst_pattern.json
	@docker-compose run router-test ./message_router configs/imbalanced_processing.json
	@docker-compose run router-test ./message_router configs/ordering_stress.json
	@docker-compose run router-test ./message_router configs/strategy_bottleneck.json


test-baseline: build
	@echo "Running baseline test..."
	@cd $(BUILD_DIR) && ./message_router ../configs/baseline.json

test-hot-type: build
	@echo "Running hot type test..."
	@cd $(BUILD_DIR) && ./message_router ../configs/hot_type.json

test-burst: build
	@echo "Running burst test..."
	@cd $(BUILD_DIR) && ./message_router ../configs/burst_pattern.json

test-imbalanced: build
	@echo "Running imbalanced processing test..."
	@cd $(BUILD_DIR) && ./message_router ../configs/imbalanced_processing.json

test-ordering: build
	@echo "Running ordering test..."
	@cd $(BUILD_DIR) && ./message_router ../configs/ordering_stress.json

test-bottleneck: build
	@echo "Running bottleneck test..."
	@cd $(BUILD_DIR) && ./message_router ../configs/strategy_bottleneck.json


install-deps:
	@echo "Installing dependencies..."
	@sudo apt-get update
	@sudo apt-get install -y build-essential cmake clang libbenchmark-dev libjsoncpp-dev


check-deps:
	@echo "Checking dependencies..."
	@which clang++ || (echo "Error: clang++ not found" && exit 1)
	@which cmake || (echo "Error: cmake not found" && exit 1)
	@pkg-config --exists benchmark || (echo "Error: libbenchmark not found" && exit 1)
	@pkg-config --exists jsoncpp || (echo "Error: libjsoncpp not found" && exit 1)
	@echo "All dependencies found!"


debug:
	@echo "Building in debug mode..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug ..
	@cd $(BUILD_DIR) && make -j$$(nproc)
	@echo "Debug build completed!"


profile: build
	@echo "Running performance analysis..."
	@cd $(BUILD_DIR) && perf record ./message_router ../configs/baseline.json
	@cd $(BUILD_DIR) && perf report


help:
	@echo "Available commands:"
	@echo "  build          - Build project"
	@echo "  clean          - Clean build files"
	@echo "  test           - Run all tests"
	@echo "  benchmark      - Run benchmarks"
	@echo "  docker-build   - Build Docker image"
	@echo "  docker-test    - Run tests in Docker"
	@echo "  docker-benchmark - Run benchmarks in Docker"
	@echo "  test-<name>    - Run specific test"
	@echo "  install-deps   - Install dependencies"
	@echo "  check-deps     - Check dependencies"
	@echo "  debug          - Debug build"
	@echo "  profile        - Performance analysis"
	@echo "  help           - Show this help"
