#!/bin/bash
# Test suite for jftt-compiler

set -e

# 1. Compile the compiler

curr_dir=$(pwd)
cd ~/jftt-compiler || exit 1

mkdir -p build
cmake --build build --parallel

# 2. Compile test programs

test_file="example.imp"
    
base_name=$(basename "$test_file" .imp)
asm_file="build/${base_name}.asm"

echo ""
echo "Testing: $base_name"
echo "-------------------"

./build/compiler "$test_file" "$asm_file"
./mw2025/maszyna-wirtualna "$asm_file"

echo ""
echo "=================================="
echo "Tests completed for stage ${stage}"

cd "$curr_dir" || exit 1


