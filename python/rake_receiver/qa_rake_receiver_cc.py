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


if __name__ == "__main__":
    gr_unittest.run(qa_rake_receiver_cc)
