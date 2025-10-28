#!/bin/bash


echo "Running all Message Router test scenarios..."


mkdir -p results


run_test() {
    local test_name=$1
    local config_file=$2
    local output_file="results/${test_name}_summary.txt"
    
    echo "Running test: ${test_name}..."
    echo "Configuration: ${config_file}"
    
    if [ -f "${config_file}" ]; then
        ./message_router ${config_file} > ${output_file} 2>&1
        echo "Results saved in ${output_file}"
        
        
        if grep -q "Test Result: PASSED" ${output_file}; then
            echo "✓ ${test_name}: PASSED"
        else
            echo "✗ ${test_name}: FAILED"
        fi
    else
        echo "Error: configuration file ${config_file} not found"
        return 1
    fi
}


if [ ! -f "./message_router" ]; then
    echo "Error: message_router executable not found"
    echo "First build the project: mkdir build && cd build && cmake .. && make"
    exit 1
fi


run_test "baseline" "configs/baseline.json"
run_test "hot_type" "configs/hot_type.json"
run_test "burst_pattern" "configs/burst_pattern.json"
run_test "imbalanced_processing" "configs/imbalanced_processing.json"
run_test "ordering_stress" "configs/ordering_stress.json"
run_test "strategy_bottleneck" "configs/strategy_bottleneck.json"

echo ""
echo "All tests completed!"
echo "Results available in results/ directory"


echo "Creating summary report..."
cat > results/test_summary.txt << EOF
=== TEST SUMMARY REPORT ===
Date: $(date)
Version: Message Router v1.0

Tests run:
1. baseline - Baseline test (4M messages/sec)
2. hot_type - Hot type test (70% type-0)
3. burst_pattern - Traffic burst test
4. imbalanced_processing - Imbalanced processing test
5. ordering_stress - Ordering stress test
6. strategy_bottleneck - Strategy bottleneck test

Results:
EOF


for test in baseline hot_type burst_pattern imbalanced_processing ordering_stress strategy_bottleneck; do
    if [ -f "results/${test}_summary.txt" ]; then
        echo "" >> results/test_summary.txt
        echo "=== ${test} ===" >> results/test_summary.txt
        grep -E "(Test Result:|Total Produced:|Total Processed:|Total Delivered:|Messages Lost:|Total Ordering Violations:)" results/${test}_summary.txt >> results/test_summary.txt
    fi
done

echo "Summary report created: results/test_summary.txt"
