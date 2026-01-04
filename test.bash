#!/bin/bash
# Test suite for jftt-compiler

set -e

# Check arguments
if [ $# -ne 1 ]; then
    echo "Usage: $0 <stage>"
    echo "Example: $0 1"
    exit 1
fi

stage=$1
stage_dir="stage_${stage}"

# 1. Compile the compiler

curr_dir=$(pwd)
cd ~/jftt-compiler || exit 1

mkdir -p build
cmake --build build --parallel

# 2. Compile test programs

test_dir="tests/${stage_dir}"

if [ ! -d "$test_dir" ]; then
    echo "Error: Test directory $test_dir does not exist"
    exit 1
fi

echo "Running tests for stage ${stage}..."
echo "=================================="

# Create output directory
mkdir -p "build/test_output/${stage_dir}"

# Compile and run each test file
for test_file in "$test_dir"/*.sil; do
    [ -e "$test_file" ] || continue
    
    base_name=$(basename "$test_file" .sil)
    asm_file="build/test_output/${stage_dir}/${base_name}.asm"
    
    echo ""
    echo "Testing: $base_name"
    echo "-------------------"
    
    ./build/compiler "$test_file" "$asm_file"
    ./mw2025/maszyna-wirtualna "$asm_file"
done

echo ""
echo "=================================="
echo "Tests completed for stage ${stage}"

cd "$curr_dir" || exit 1


