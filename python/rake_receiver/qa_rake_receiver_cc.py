#!/usr/bin/env python3
#
# Copyright 2024
#
# This file is part of gr-rake_receiver
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

from gnuradio import gr, gr_unittest, blocks, rake_receiver
import numpy as np


class qa_rake_receiver_cc(gr_unittest.TestCase):  # noqa: N801
    def setUp(self):
        self.tb = gr.top_block()

    def tearDown(self):
        self.tb = None

    def test_001_make(self):
        num_fingers = 3
        delays = [0, 10, 20]
        gains = [1.0, 0.8, 0.6]
        pattern_length = 16

        rake = rake_receiver.rake_receiver_cc(
            num_fingers, delays, gains, pattern_length
        )
        self.assertIsNotNone(rake)
        self.assertEqual(rake.num_fingers(), num_fingers)

    def test_002_num_fingers_limits(self):
        delays = [0]
        gains = [1.0]

        with self.assertRaises(Exception):
            rake_receiver.rake_receiver_cc(0, delays, gains, 16)

        delays_5 = [0, 10, 20, 30, 40]
        gains_5 = [1.0, 0.8, 0.6, 0.4, 0.2]
        rake = rake_receiver.rake_receiver_cc(6, delays_5, gains_5, 16)
        self.assertEqual(rake.num_fingers(), 5)

    def test_003_set_delays(self):
        num_fingers = 2
        delays = [0, 10]
        gains = [1.0, 0.8]
        pattern_length = 16

        rake = rake_receiver.rake_receiver_cc(
            num_fingers, delays, gains, pattern_length
        )
        self.assertIsNotNone(rake)

        new_delays = [5, 15]
        rake.set_delays(new_delays)
        self.assertEqual(rake.num_fingers(), num_fingers)

    def test_004_set_gains(self):
        num_fingers = 2
        delays = [0, 10]
        gains = [1.0, 0.8]
        pattern_length = 16

        rake = rake_receiver.rake_receiver_cc(
            num_fingers, delays, gains, pattern_length
        )
        self.assertIsNotNone(rake)

        new_gains = [0.9, 0.7]
        rake.set_gains(new_gains)
        self.assertEqual(rake.num_fingers(), num_fingers)

    def test_005_set_pattern(self):
        num_fingers = 2
        delays = [0, 10]
        gains = [1.0, 0.8]
        pattern_length = 16

        rake = rake_receiver.rake_receiver_cc(
            num_fingers, delays, gains, pattern_length
        )
        self.assertIsNotNone(rake)

        pattern = np.ones(pattern_length, dtype=complex)
        rake.set_pattern(pattern)
        self.assertEqual(rake.num_fingers(), num_fingers)

    def test_006_basic_flow(self):
        num_fingers = 2
        delays = [0, 5]
        gains = [1.0, 0.8]
        pattern_length = 8

        rake = rake_receiver.rake_receiver_cc(
            num_fingers, delays, gains, pattern_length
        )
        self.assertIsNotNone(rake)

        input_data = np.ones(100, dtype=complex)
        source = blocks.vector_source_c(input_data)
        sink = blocks.vector_sink_c()

        self.tb.connect(source, rake)
        self.tb.connect(rake, sink)

        self.tb.start()
        self.tb.wait()
        self.tb.stop()

        output = sink.data()
        self.assertGreater(len(output), 0)

    def test_007_invalid_delays_size(self):
        num_fingers = 2
        delays = [0]
        gains = [1.0, 0.8]
        pattern_length = 16

        with self.assertRaises(Exception):
            rake_receiver.rake_receiver_cc(
                num_fingers, delays, gains, pattern_length
            )

    def test_008_invalid_gains_size(self):
        num_fingers = 2
        delays = [0, 10]
        gains = [1.0]
        pattern_length = 16

        with self.assertRaises(Exception):
            rake_receiver.rake_receiver_cc(
                num_fingers, delays, gains, pattern_length
            )

    def test_009_max_fingers(self):
        num_fingers = 5
        delays = [0, 10, 20, 30, 40]
        gains = [1.0, 0.8, 0.6, 0.4, 0.2]
        pattern_length = 16

        rake = rake_receiver.rake_receiver_cc(
            num_fingers, delays, gains, pattern_length
        )
        self.assertIsNotNone(rake)
        self.assertEqual(rake.num_fingers(), num_fingers)

    def test_010_pattern_length_mismatch(self):
        num_fingers = 2
        delays = [0, 10]
        gains = [1.0, 0.8]
        pattern_length = 16

        rake = rake_receiver.rake_receiver_cc(
            num_fingers, delays, gains, pattern_length
        )
        self.assertIsNotNone(rake)

        wrong_pattern = np.ones(pattern_length + 1, dtype=complex)
        with self.assertRaises(Exception):
            rake.set_pattern(wrong_pattern)

    def test_011_adaptive_parameters(self):
        num_fingers = 4
        delays = [0, 10, 20, 30]
        gains = [1.0, 0.8, 0.6, 0.4]
        pattern_length = 42

        rake = rake_receiver.rake_receiver_cc(
            num_fingers, delays, gains, pattern_length
        )
        self.assertIsNotNone(rake)

        # Test default values
        self.assertAlmostEqual(rake.path_search_rate(), 20.0, places=5)
        self.assertAlmostEqual(rake.tracking_bandwidth(), 120.0, places=5)
        self.assertAlmostEqual(rake.path_detection_threshold(), 0.5, places=5)
        self.assertAlmostEqual(rake.lock_threshold(), 0.7, places=5)
        self.assertAlmostEqual(rake.reassignment_period(), 1.0, places=5)
        self.assertAlmostEqual(rake.gps_speed(), -1.0, places=5)
        self.assertFalse(rake.adaptive_mode())

        # Test setting parameters
        rake.set_path_search_rate(50.0)
        self.assertAlmostEqual(rake.path_search_rate(), 50.0, places=5)

        rake.set_tracking_bandwidth(200.0)
        self.assertAlmostEqual(rake.tracking_bandwidth(), 200.0, places=5)

    def test_012_gps_speed_adaptive(self):
        num_fingers = 4
        delays = [0, 10, 20, 30]
        gains = [1.0, 0.8, 0.6, 0.4]
        pattern_length = 42

        rake = rake_receiver.rake_receiver_cc(
            num_fingers, delays, gains, pattern_length
        )
        self.assertIsNotNone(rake)

        # Enable adaptive mode
        rake.set_adaptive_mode(True)
        self.assertTrue(rake.adaptive_mode())

        # Test vehicle low-speed mode (15-60 km/h) - exact boundary
        rake.set_gps_speed(60.0)
        self.assertAlmostEqual(rake.gps_speed(), 60.0, places=5)
        self.assertAlmostEqual(rake.path_search_rate(), 20.0, places=1)
        self.assertAlmostEqual(rake.tracking_bandwidth(), 120.0, places=1)

        # Test interpolation at 37.5 km/h (midway between 15-60)
        rake.set_gps_speed(37.5)
        self.assertAlmostEqual(rake.path_search_rate(), 15.0, places=1)  # (10+20)/2
        self.assertAlmostEqual(rake.tracking_bandwidth(), 110.0, places=1)  # (100+120)/2

        # Test vehicle high-speed mode (60-120 km/h) - exact boundary
        rake.set_gps_speed(120.0)
        self.assertAlmostEqual(rake.path_search_rate(), 50.0, places=1)
        self.assertAlmostEqual(rake.tracking_bandwidth(), 200.0, places=1)

        # Test interpolation at 90 km/h (midway between 60-120)
        rake.set_gps_speed(90.0)
        self.assertAlmostEqual(rake.path_search_rate(), 35.0, places=1)  # (20+50)/2
        self.assertAlmostEqual(rake.tracking_bandwidth(), 160.0, places=1)  # (120+200)/2

    def test_013_nmea0183_parsing(self):
        num_fingers = 4
        delays = [0, 10, 20, 30]
        gains = [1.0, 0.8, 0.6, 0.4]
        pattern_length = 42

        rake = rake_receiver.rake_receiver_cc(
            num_fingers, delays, gains, pattern_length
        )
        self.assertIsNotNone(rake)

        rake.set_adaptive_mode(True)

        # Test GPRMC parsing
        gprmc = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A"
        result = rake.parse_nmea0183(gprmc)
        self.assertTrue(result)
        # 22.4 knots * 1.852 = 41.4848 km/h
        self.assertAlmostEqual(rake.gps_speed(), 41.4848, places=1)

    def test_014_gpsd_parsing(self):
        num_fingers = 4
        delays = [0, 10, 20, 30]
        gains = [1.0, 0.8, 0.6, 0.4]
        pattern_length = 42

        rake = rake_receiver.rake_receiver_cc(
            num_fingers, delays, gains, pattern_length
        )
        self.assertIsNotNone(rake)

        rake.set_adaptive_mode(True)

        # Test GPSD TPV message
        gpsd_tpv = (
            '{"class":"TPV","device":"/dev/ttyUSB0",'
            '"time":"2024-01-01T12:00:00.000Z","lat":48.123,'
            '"lon":11.456,"speed":12.5}'
        )
        result = rake.parse_gpsd(gpsd_tpv)
        self.assertTrue(result)
        # 12.5 m/s * 3.6 = 45.0 km/h
        self.assertAlmostEqual(rake.gps_speed(), 45.0, places=1)

    def test_015_gps_data_auto_parse(self):
        num_fingers = 4
        delays = [0, 10, 20, 30]
        gains = [1.0, 0.8, 0.6, 0.4]
        pattern_length = 42

        rake = rake_receiver.rake_receiver_cc(
            num_fingers, delays, gains, pattern_length
        )
        self.assertIsNotNone(rake)

        rake.set_adaptive_mode(True)

        # Test auto-detection of GPSD
        gpsd = '{"class":"TPV","speed":10.0}'
        result = rake.parse_gps_data(gpsd)
        self.assertTrue(result)
        # 10 m/s = 36 km/h
        self.assertAlmostEqual(rake.gps_speed(), 36.0, places=1)


if __name__ == "__main__":
    gr_unittest.run(qa_rake_receiver_cc)
