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

### Verifying Installation

After installation, verify that the module is properly installed:

```bash
# Check library installation
ls /usr/local/lib/libgnuradio-rake_receiver.so*

# Check Python module
python3 -c "from gnuradio import rake_receiver; print('Module installed successfully')"

# Check GRC block
ls /usr/local/share/gnuradio/grc/blocks/rake_receiver_rake_receiver_cc.block.yml

# Check headers
ls /usr/local/include/gnuradio/rake_receiver/*.h
```

If all commands succeed, the module is properly installed.

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

**Basic Parameters:**
- **num_fingers** (int): Number of RAKE fingers, must be between 1 and 5 (default: 4)
- **delays** (vector<int>): Delay values for each finger in samples (default: [0, 10, 20, 30])
- **gains** (vector<float>): Gain values for combining each finger output (default: [1.0, 0.8, 0.6, 0.4])
- **pattern_length** (int): Length of the correlation pattern (default: 42 chips)

**Adaptive Parameters (Recommended Defaults):**
- **gps_speed** (float): GPS speed in km/h for adaptive mode. Set to -1 to disable (default: -1.0)
- **path_search_rate** (float): Path search rate in Hz (default: 20.0 Hz)
- **tracking_bandwidth** (float): Tracking bandwidth in Hz (default: 120.0 Hz)
- **path_detection_threshold** (float): Path detection threshold as fraction of peak correlation (default: 0.5)
- **lock_threshold** (float): Lock detector threshold correlation value (default: 0.7)
- **reassignment_period** (float): Reassignment period in seconds (default: 1.0 s)
- **adaptive_mode** (bool): Enable adaptive mode based on GPS speed (default: False)

**GPS Source Configuration:**
- **gps_source** (string): GPS source type - "none", "serial", or "gpsd" (default: "none")
- **serial_device** (string): Serial device path for GPS (default: "/dev/ttyUSB0")
- **serial_baud_rate** (int): Serial baud rate for GPS (default: 4800)
- **gpsd_host** (string): GPSD hostname or IP address (default: "localhost")
- **gpsd_port** (int): GPSD port number (default: 2947)

### Methods

**Basic Methods:**
- `set_delays(delays)`: Update the delay values for each finger
- `set_gains(gains)`: Update the gain values for each finger
- `set_pattern(pattern)`: Set the correlation pattern (complex vector)
- `num_fingers()`: Get the current number of fingers

**Adaptive Methods:**
- `set_gps_speed(speed_kmh)`: Set GPS speed for adaptive parameter adjustment
- `gps_speed()`: Get current GPS speed setting
- `set_path_search_rate(rate_hz)`: Set path search rate
- `path_search_rate()`: Get current path search rate
- `set_tracking_bandwidth(bandwidth_hz)`: Set tracking bandwidth
- `tracking_bandwidth()`: Get current tracking bandwidth
- `set_path_detection_threshold(threshold)`: Set path detection threshold
- `path_detection_threshold()`: Get current path detection threshold
- `set_lock_threshold(threshold)`: Set lock detector threshold
- `lock_threshold()`: Get current lock threshold
- `set_reassignment_period(period_s)`: Set reassignment period
- `reassignment_period()`: Get current reassignment period
- `set_adaptive_mode(enable)`: Enable or disable adaptive mode
- `adaptive_mode()`: Check if adaptive mode is enabled

**GPS Parsing Methods:**
- `parse_gps_data(gps_data)`: Parse GPS data from NMEA0183 or GPSD format (auto-detects)
- `parse_nmea0183(nmea_message)`: Parse NMEA0183 message and update GPS speed
- `parse_gpsd(gpsd_json)`: Parse GPSD JSON message and update GPS speed

### Adaptive RAKE Parameters Based on GPS Speed

The RAKE receiver can automatically adjust its parameters based on GPS speed to optimize performance for different mobility scenarios. Parameters are **interpolated smoothly** between speed categories to provide continuous adaptation:

**Stationary (0-5 km/h):**
- Path search rate: 5 Hz (200ms updates)
- Tracking bandwidth: 50 Hz
- Number of fingers: 3
- Reassignment period: 2 seconds
- *Reasoning: Environment stable, save processing power*

**Pedestrian (5-15 km/h):**
- Path search rate: 10 Hz (100ms updates)
- Tracking bandwidth: 100 Hz
- Number of fingers: 3
- Reassignment period: 1 second
- *Reasoning: Slow multipath changes*
- *Interpolation: Smoothly transitions from Stationary to Pedestrian parameters*

**Vehicle Low-Speed (15-60 km/h) - Default:**
- Path search rate: 20 Hz (50ms updates)
- Tracking bandwidth: 120 Hz
- Number of fingers: 4
- Reassignment period: 1 second
- *Reasoning: Moderate multipath dynamics, urban driving*
- *Interpolation: Smoothly transitions from Pedestrian to Low-Speed parameters*

**Vehicle High-Speed (60-120 km/h):**
- Path search rate: 50 Hz (20ms updates)
- Tracking bandwidth: 200 Hz
- Number of fingers: 4
- Reassignment period: 0.5 seconds
- *Reasoning: Rapid environment changes, highway speeds*
- *Interpolation: Smoothly transitions from Low-Speed to High-Speed parameters*

**Vehicle Very High-Speed (>120 km/h):**
- Path search rate: 100 Hz (10ms updates)
- Tracking bandwidth: 300 Hz
- Number of fingers: 4
- Reassignment period: 0.25 seconds
- *Reasoning: Very fast multipath evolution, maximum Doppler*
- *Interpolation: Smoothly transitions from High-Speed to Very High-Speed parameters (capped at 200 km/h)*

**Interpolation Behavior:**
The implementation uses linear interpolation between adjacent speed categories. For example:
- At 10 km/h (midway between 5-15 km/h): Parameters are halfway between Stationary and Pedestrian
- At 37.5 km/h (midway between 15-60 km/h): Parameters are halfway between Pedestrian and Low-Speed
- At 90 km/h (midway between 60-120 km/h): Parameters are halfway between Low-Speed and High-Speed

This provides smooth, continuous parameter adjustment rather than discrete jumps, resulting in better performance during speed transitions.

### Using Adaptive Mode

```python
from gnuradio import rake_receiver
import numpy as np

# Create RAKE receiver with default parameters
rake = rake_receiver.rake_receiver_cc(
    num_fingers=4,
    delays=[0, 10, 20, 30],
    gains=[1.0, 0.8, 0.6, 0.4],
    pattern_length=42
)

# Enable adaptive mode
rake.set_adaptive_mode(True)

# Update GPS speed - parameters will adjust automatically
rake.set_gps_speed(45.0)  # Vehicle low-speed mode
rake.set_gps_speed(100.0)  # Vehicle high-speed mode
rake.set_gps_speed(0.0)    # Stationary mode
```

### NMEA0183 and GPSD Support

The RAKE receiver includes built-in parsers for NMEA0183 and GPSD formats, allowing automatic GPS speed extraction from GPS receivers.

#### NMEA0183 Parsing

The module supports parsing NMEA0183 messages, particularly:
- **GPRMC** (Recommended Minimum Course): Extracts speed from field 7 (speed in knots)
- **GPVTG** (Track Made Good and Ground Speed): Extracts speed from field 7 (speed in km/h)

```python
from gnuradio import rake_receiver

rake = rake_receiver.rake_receiver_cc(4, [0, 10, 20, 30], [1.0, 0.8, 0.6, 0.4], 42)
rake.set_adaptive_mode(True)

# Parse GPRMC message (speed in knots)
gprmc = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A"
rake.parse_nmea0183(gprmc)  # Automatically extracts 22.4 knots = 41.5 km/h

# Parse GPVTG message (speed in km/h)
gpvtg = "$GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48"
rake.parse_nmea0183(gpvtg)  # Automatically extracts 10.2 km/h
```

#### GPSD Parsing

The module supports parsing GPSD JSON messages, extracting speed from TPV (Time-Position-Velocity) messages:

```python
from gnuradio import rake_receiver

rake = rake_receiver.rake_receiver_cc(4, [0, 10, 20, 30], [1.0, 0.8, 0.6, 0.4], 42)
rake.set_adaptive_mode(True)

# Parse GPSD TPV message (speed in m/s)
gpsd_tpv = '{"class":"TPV","device":"/dev/ttyUSB0","time":"2024-01-01T12:00:00.000Z",' \
           '"lat":48.123,"lon":11.456,"speed":12.5}'
rake.parse_gpsd(gpsd_tpv)  # Automatically extracts 12.5 m/s = 45.0 km/h
```

#### Auto-Detection

The `parse_gps_data()` method automatically detects the format and parses accordingly:

```python
# Automatically detects and parses NMEA0183 or GPSD
rake.parse_gps_data("$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A")
rake.parse_gps_data('{"class":"TPV","speed":10.0}')
```

#### Integration with GPS Receivers

The RAKE receiver block includes configurable GPS source parameters and a **message input port** named `gps` that automatically parses incoming GPS data. You can configure the GPS source directly in the block parameters.

**Example: Configuring Serial GPS Device:**

```python
from gnuradio import rake_receiver

# Create RAKE receiver
rake = rake_receiver.rake_receiver_cc(4, [0, 10, 20, 30], [1.0, 0.8, 0.6, 0.4], 42)
rake.set_adaptive_mode(True)

# Configure serial GPS source
rake.set_gps_source("serial")
rake.set_serial_device("/dev/ttyUSB0")  # Your GPS device
rake.set_serial_baud_rate(4800)  # GPS baud rate

# Now use the configured parameters with your serial reading code
# The block stores these settings for reference
print(f"GPS configured: {rake.serial_device()} at {rake.serial_baud_rate()} baud")
```

**Example: Configuring GPSD:**

```python
from gnuradio import rake_receiver

# Create RAKE receiver
rake = rake_receiver.rake_receiver_cc(4, [0, 10, 20, 30], [1.0, 0.8, 0.6, 0.4], 42)
rake.set_adaptive_mode(True)

# Configure GPSD source
rake.set_gps_source("gpsd")
rake.set_gpsd_host("localhost")  # GPSD hostname or IP
rake.set_gpsd_port(2947)  # GPSD port

# Now use the configured parameters with your GPSD connection code
print(f"GPSD configured: {rake.gpsd_host()}:{rake.gpsd_port()}")
```

**Example: Using message port with serial GPS device (NMEA0183):**

```python
from gnuradio import gr, blocks, rake_receiver
import pmt
import serial
import threading

# Create flowgraph
fg = gr.top_block()

# Create RAKE receiver
rake = rake_receiver.rake_receiver_cc(4, [0, 10, 20, 30], [1.0, 0.8, 0.6, 0.4], 42)
rake.set_adaptive_mode(True)

# Connect signal processing chain
fg.connect(signal_source, rake)
fg.connect(rake, signal_sink)

# Function to read GPS data and send to message port
def gps_reader():
    # Use configured serial parameters
    gps_serial = serial.Serial(rake.serial_device(), rake.serial_baud_rate(), timeout=1)
    while True:
        line = gps_serial.readline().decode('ascii', errors='ignore')
        if line.startswith('$GPRMC') or line.startswith('$GPVTG'):
            # Send to RAKE receiver's message port
            msg = pmt.string_to_symbol(line.strip())
            rake.message_port_pub(pmt.intern("gps"), msg)

# Start GPS reader thread
gps_thread = threading.Thread(target=gps_reader, daemon=True)
gps_thread.start()

fg.start()
fg.wait()
```

**Note:** The message port automatically detects and parses NMEA0183 or GPSD format, so you don't need to call `parse_nmea0183()` or `parse_gpsd()` manually when using the message port.

**Example: Using message port with GPSD:**

```python
from gnuradio import gr, blocks, rake_receiver
import pmt
import socket
import threading

# Create flowgraph
fg = gr.top_block()

rake = rake_receiver.rake_receiver_cc(4, [0, 10, 20, 30], [1.0, 0.8, 0.6, 0.4], 42)
rake.set_adaptive_mode(True)

def gpsd_reader():
    # Use configured GPSD parameters
    gpsd_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    gpsd_socket.connect((rake.gpsd_host(), rake.gpsd_port()))
    gpsd_socket.send(b'?WATCH={"enable":true,"json":true}\n')
    
    while True:
        data = gpsd_socket.recv(1024).decode('utf-8')
        if '"class":"TPV"' in data:
            # Send to RAKE receiver's message port
            msg = pmt.string_to_symbol(data.strip())
            rake.message_port_pub(pmt.intern("gps"), msg)

# Start GPSD reader thread
gpsd_thread = threading.Thread(target=gpsd_reader, daemon=True)
gpsd_thread.start()

fg.start()
fg.wait()
```

**Message Port Details:**
- Port name: `gps`
- Accepts: PMT string messages containing NMEA0183 or GPSD JSON data
- Auto-detection: Automatically detects format and parses speed
- Thread-safe: Can be called from any thread

**Alternative: Manual parsing (without message port):**

If you prefer to parse GPS data manually, you can still use the parsing methods directly:

```python
import serial
from gnuradio import rake_receiver

rake = rake_receiver.rake_receiver_cc(4, [0, 10, 20, 30], [1.0, 0.8, 0.6, 0.4], 42)
rake.set_adaptive_mode(True)

# Open serial connection to GPS
gps_serial = serial.Serial('/dev/ttyUSB0', 4800, timeout=1)

while True:
    line = gps_serial.readline().decode('ascii', errors='ignore')
    if line.startswith('$GPRMC') or line.startswith('$GPVTG'):
        rake.parse_nmea0183(line.strip())
```

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