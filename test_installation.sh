#!/bin/bash
# Test script to verify RAKE receiver module installation

echo "Testing RAKE receiver module installation..."
echo ""

# Check library
echo "1. Checking library installation..."
if ls /usr/local/lib/libgnuradio-rake_receiver.so* 2>/dev/null | grep -q .; then
    echo "   [OK] Library found"
    ls -lh /usr/local/lib/libgnuradio-rake_receiver.so*
else
    echo "   [FAIL] Library not found"
    exit 1
fi

# Check Python module
echo ""
echo "2. Checking Python module..."
if python3 -c "from gnuradio import rake_receiver; print('   [OK] Python module imports successfully')" 2>/dev/null; then
    :
else
    echo "   [FAIL] Python module import failed"
    exit 1
fi

# Check GRC block
echo ""
echo "3. Checking GRC block..."
if [ -f /usr/local/share/gnuradio/grc/blocks/rake_receiver_rake_receiver_cc.block.yml ]; then
    echo "   [OK] GRC block found"
else
    echo "   [FAIL] GRC block not found"
    exit 1
fi

# Check headers
echo ""
echo "4. Checking header files..."
if ls /usr/local/include/gnuradio/rake_receiver/*.h 2>/dev/null | grep -q .; then
    echo "   [OK] Header files found"
    ls /usr/local/include/gnuradio/rake_receiver/*.h
else
    echo "   [FAIL] Header files not found"
    exit 1
fi

echo ""
echo "All installation checks passed!"
