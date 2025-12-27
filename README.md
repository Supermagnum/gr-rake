# GNU Radio RAKE Receiver Module

This is a GNU Radio Out-of-Tree (OOT) module implementing a RAKE receiver with configurable number of fingers (maximum 5).

## Overview

A RAKE receiver is used in wireless communications to combine multiple delayed versions of a signal to improve reception quality. This implementation allows you to configure:

- Number of fingers (1-5)
- Delay values for each finger (in samples)
- Gain values for each finger (for combining)
- Correlation pattern length

## Building and Installation

### Prerequisites

- GNU Radio 3.10 or later
- CMake 3.16 or later
- C++ compiler with C++11 support
- Python 3 (for Python bindings)
- Boost (for unit tests)
- Valgrind (optional, for memory leak detection)

### Build Instructions

```bash
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
```

### Testing the Installation

After installation, you can test the module in Python:

```python
from gnuradio import rake_receiver
import numpy as np

# Create a RAKE receiver with 3 fingers
num_fingers = 3
delays = [0, 10, 20]  # Delay in samples for each finger
gains = [1.0, 0.8, 0.6]  # Gain for each finger
pattern_length = 16

rake = rake_receiver.rake_receiver_cc(num_fingers, delays, gains, pattern_length)

# Set correlation pattern (optional, defaults to all ones)
pattern = np.ones(pattern_length, dtype=complex)
rake.set_pattern(pattern)
```

## Usage

### In GNU Radio Companion (GRC)

The block is available in the `rake_receiver` category in GRC. You can configure:

- **Number of Fingers**: Select from 1 to 5
- **Delays**: Vector of delay values in samples (e.g., `[0, 10, 20]`)
- **Gains**: Vector of gain values for combining (e.g., `[1.0, 0.8, 0.6]`)
- **Pattern Length**: Length of the correlation pattern

### In Python Flowgraphs

```python
from gnuradio import gr, blocks, rake_receiver
import numpy as np

# Create flowgraph
fg = gr.top_block()

# Create RAKE receiver
rake = rake_receiver.rake_receiver_cc(
    num_fingers=3,
    delays=[0, 10, 20],
    gains=[1.0, 0.8, 0.6],
    pattern_length=16
)

# Set correlation pattern
pattern = np.array([1+0j] * 16, dtype=complex)
rake.set_pattern(pattern)

# Connect blocks (example)
# fg.connect(source, rake)
# fg.connect(rake, sink)
```

### Block Parameters

- **num_fingers** (int): Number of RAKE fingers, must be between 1 and 5
- **delays** (vector<int>): Delay values for each finger in samples
- **gains** (vector<float>): Gain values for combining each finger output
- **pattern_length** (int): Length of the correlation pattern

### Methods

- `set_delays(delays)`: Update the delay values for each finger
- `set_gains(gains)`: Update the gain values for each finger
- `set_pattern(pattern)`: Set the correlation pattern (complex vector)
- `num_fingers()`: Get the current number of fingers

## Implementation Details

The RAKE receiver:

1. Takes the input signal
2. For each finger, applies the specified delay
3. Correlates the delayed signal with the correlation pattern
4. Applies the specified gain to each finger output
5. Combines all finger outputs to produce the final output

The block automatically manages history to ensure sufficient samples are available for correlation at all delays.

## Testing

### Running Tests

To run the test suite:

```bash
cd build
ctest -V
```

### Test Results

#### Unit Tests

**C++ Tests** (`lib/qa_rake_receiver_cc.cc`):
- Test 1: `test_rake_receiver_cc_make` - Block creation and basic configuration
- Test 2: `test_rake_receiver_cc_num_fingers_limits` - Validation of finger count limits (1-5)
- Test 3: `test_rake_receiver_cc_set_delays` - Dynamic delay configuration
- Test 4: `test_rake_receiver_cc_set_gains` - Dynamic gain configuration
- Test 5: `test_rake_receiver_cc_set_pattern` - Correlation pattern setting
- Test 6: `test_rake_receiver_cc_basic_flow` - Basic flowgraph execution
- Test 7: `test_rake_receiver_cc_invalid_delays_size` - Error handling for invalid delay vector size
- Test 8: `test_rake_receiver_cc_invalid_gains_size` - Error handling for invalid gain vector size
- Test 9: `test_rake_receiver_cc_max_fingers` - Maximum finger count (5) validation

**Python Tests** (`python/rake_receiver/qa_rake_receiver_cc.py`):
- Test 1: `test_001_make` - Block creation
- Test 2: `test_002_num_fingers_limits` - Finger count validation
- Test 3: `test_003_set_delays` - Delay configuration
- Test 4: `test_004_set_gains` - Gain configuration
- Test 5: `test_005_set_pattern` - Pattern configuration
- Test 6: `test_006_basic_flow` - Flowgraph execution
- Test 7: `test_007_invalid_delays_size` - Error handling
- Test 8: `test_008_invalid_gains_size` - Error handling
- Test 9: `test_009_max_fingers` - Maximum fingers test
- Test 10: `test_010_pattern_length_mismatch` - Pattern validation

**Test Execution Results:**
```
Test project /home/haaken/github-projects/gr-rake/build
    Start 1: rake_receiver_qa_rake_receiver_cc.cc
1/2 Test #1: rake_receiver_qa_rake_receiver_cc.cc ...   Passed    0.01 sec
    Start 2: qa_rake_receiver_cc
2/2 Test #2: qa_rake_receiver_cc ....................   Passed    0.11 sec

100% tests passed, 0 tests failed out of 2

Total Test time (real) =   0.12 sec
```

**Status: All tests passing (2/2)**

#### Memory Leak Detection (Valgrind)

Valgrind analysis was performed on the C++ test suite:

```bash
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
    ./lib/rake_receiver_qa_rake_receiver_cc.cc
```

**Results:**
- **Definitely lost**: 0 bytes in 0 blocks
- **Indirectly lost**: 0 bytes in 0 blocks
- **Possibly lost**: 336 bytes in 1 block (from GNU Radio internal thread management, not our code)
- **Still reachable**: 32 bytes in 4 blocks (from GNU Radio internal singletons)

**Status: No memory leaks detected in RAKE receiver code**

The reported "possibly lost" and "still reachable" memory allocations are from GNU Radio's internal runtime libraries (libgnuradio-runtime, libthrift) and are not related to the RAKE receiver implementation.

#### Code Quality Checks

**Black (Python Code Formatter):**
```bash
black python/rake_receiver/*.py
```

**Results:**
- All Python files formatted according to Black style guide
- 1 file reformatted, 1 file left unchanged

**Status: All Python code properly formatted**

**Flake8 (Python Linter):**
```bash
flake8 python/rake_receiver/*.py
```

**Results:**
- No style violations found
- All line length requirements met
- Proper import handling

**Status: All style checks passed**

**Bandit (Security Linter):**
```bash
bandit -r python/rake_receiver/
```

**Results:**
```
Test results:
	No issues identified.

Code scanned:
	Total lines of code: 225
	Total lines skipped (#nosec): 0

Run metrics:
	Total issues (by severity):
		Undefined: 0.0
		Low: 0.0
		Medium: 0.0
		High: 0.0
```

**Status: No security issues found**

### Test Coverage

The test suite covers:
- Block creation and initialization
- Parameter validation (finger count, vector sizes)
- Dynamic parameter updates (delays, gains, pattern)
- Basic signal processing flow
- Error handling for invalid inputs
- Edge cases (minimum/maximum finger counts)
- Memory management (no leaks)
- Code style and formatting
- Security best practices

## License

SPDX-License-Identifier: GPL-3.0-or-later

## Contributing

When contributing to this module, please ensure:
1. All tests pass (`ctest`)
2. No memory leaks (run with valgrind)
3. Code follows Black formatting (`black --check`)
4. No flake8 violations (`flake8`)
5. No security issues (bandit)

## References

- GNU Radio Documentation: https://wiki.gnuradio.org/
- RAKE Receiver Theory: https://en.wikipedia.org/wiki/RAKE_receiver