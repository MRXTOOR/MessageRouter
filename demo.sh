#!/bin/bash


echo "=== MESSAGE ROUTER DEMONSTRATION ==="
echo "High-performance lock-free message routing system"
echo ""


if ! command -v docker &> /dev/null; then
    echo "❌ Docker not found. Install Docker to run the demonstration."
    exit 1
fi

if ! command -v docker-compose &> /dev/null; then
    echo "❌ Docker Compose not found. Install Docker Compose to run the demonstration."
    exit 1
fi

echo "✅ Docker and Docker Compose found"
echo ""


mkdir -p results

echo "🔨 Building Docker image..."
docker-compose build

if [ $? -ne 0 ]; then
    echo "❌ Docker image build error"
    exit 1
fi

echo "✅ Docker image built successfully"
echo ""

echo "🚀 Starting demonstration..."
echo ""


echo "=== TEST 1: Baseline scenario (4M messages/sec) ==="
docker-compose run --rm router-test ./message_router configs/baseline.json
echo ""


echo "=== TEST 2: Hot message type (70% type-0) ==="
docker-compose run --rm router-test ./message_router configs/hot_type.json
echo ""


echo "=== TEST 3: Traffic bursts ==="
docker-compose run --rm router-test ./message_router configs/burst_pattern.json
echo ""


echo "=== TEST 4: Imbalanced processing ==="
docker-compose run --rm router-test ./message_router configs/imbalanced_processing.json
echo ""


echo "=== TEST 5: Ordering stress test ==="
docker-compose run --rm router-test ./message_router configs/ordering_stress.json
echo ""


echo "=== TEST 6: Strategy bottleneck ==="
docker-compose run --rm router-test ./message_router configs/strategy_bottleneck.json
echo ""

echo "📊 Running performance benchmarks..."
echo ""


echo "=== BENCHMARK: Queue performance ==="
docker-compose run --rm router-benchmark ./benchmarks/queue_perf --benchmark_format=console
echo ""

echo "=== BENCHMARK: Routing latency ==="
docker-compose run --rm router-benchmark ./benchmarks/routing_perf --benchmark_format=console
echo ""

echo "=== BENCHMARK: Memory allocation ==="
docker-compose run --rm router-benchmark ./benchmarks/memory_perf --benchmark_format=console
echo ""

echo "=== BENCHMARK: Scaling ==="
docker-compose run --rm router-benchmark ./benchmarks/scaling_perf --benchmark_format=console
echo ""

echo "✅ Demonstration completed!"
echo ""
echo "📁 Results saved in results/ directory"
echo "📊 To view detailed results use:"
echo "   - cat results/*_summary.txt"
echo "   - ls results/benchmarks/"
echo ""
echo "🎯 System ready for use!"
echo ""
echo "💡 Additional commands:"
echo "   - make help          
echo "   - make test          
echo "   - make benchmark     
echo "   - make docker-test   
echo "   - make docker-benchmark 
