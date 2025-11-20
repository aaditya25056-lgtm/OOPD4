#!/usr/bin/env bash
# demo.sh - Quick demonstration script
# Usage: ./demo.sh [quick|full]

set -e

MODE=${1:-full}

echo "========================================="
echo "  UNIVERSITY ERP SYSTEM - DEMO"
echo "========================================="
echo ""

# Build everything
echo "Step 1: Building the system..."
make build_all
echo ""

if [ "$MODE" = "quick" ]; then
    echo "Step 2: Generating 50 students for quick demo..."
    make gen50
    echo ""
    
    echo "Step 3: Launching ERP system..."
    echo "========================================="
    ./erp students.csv
else
    echo "Step 2: Generating 3000 students..."
    make gen
    echo ""
    
    echo "Step 3: Launching ERP system..."
    echo "========================================="
    ./erp students.csv
fi

echo ""
echo "Demo completed!"