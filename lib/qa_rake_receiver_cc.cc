/*
 * Copyright 2024
 *
 * This file is part of gr-rake_receiver
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/attributes.h>
#include <gnuradio/rake_receiver/rake_receiver_cc.h>
#include <gnuradio/blocks/vector_source.h>
#include <gnuradio/blocks/vector_sink.h>
#include <gnuradio/top_block.h>
#include <boost/test/unit_test.hpp>
#include <vector>
#include <complex>

namespace gr {
namespace rake_receiver {

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_make)
{
    int num_fingers = 3;
    std::vector<int> delays = {0, 10, 20};
    std::vector<float> gains = {1.0f, 0.8f, 0.6f};
    int pattern_length = 16;

    auto rake = rake_receiver_cc::make(num_fingers, delays, gains, pattern_length);
    BOOST_REQUIRE(rake != nullptr);
    BOOST_CHECK_EQUAL(rake->num_fingers(), num_fingers);
}

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_num_fingers_limits)
{
    std::vector<int> delays = {0};
    std::vector<float> gains = {1.0f};

    BOOST_CHECK_THROW(
        rake_receiver_cc::make(0, delays, gains, 16), std::invalid_argument);
    BOOST_CHECK_THROW(
        rake_receiver_cc::make(6, delays, gains, 16), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_set_delays)
{
    int num_fingers = 2;
    std::vector<int> delays = {0, 10};
    std::vector<float> gains = {1.0f, 0.8f};
    int pattern_length = 16;

    auto rake = rake_receiver_cc::make(num_fingers, delays, gains, pattern_length);
    BOOST_REQUIRE(rake != nullptr);

    std::vector<int> new_delays = {5, 15};
    rake->set_delays(new_delays);
    BOOST_CHECK_EQUAL(rake->num_fingers(), num_fingers);
}

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_set_gains)
{
    int num_fingers = 2;
    std::vector<int> delays = {0, 10};
    std::vector<float> gains = {1.0f, 0.8f};
    int pattern_length = 16;

    auto rake = rake_receiver_cc::make(num_fingers, delays, gains, pattern_length);
    BOOST_REQUIRE(rake != nullptr);

    std::vector<float> new_gains = {0.9f, 0.7f};
    rake->set_gains(new_gains);
    BOOST_CHECK_EQUAL(rake->num_fingers(), num_fingers);
}

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_set_pattern)
{
    int num_fingers = 2;
    std::vector<int> delays = {0, 10};
    std::vector<float> gains = {1.0f, 0.8f};
    int pattern_length = 16;

    auto rake = rake_receiver_cc::make(num_fingers, delays, gains, pattern_length);
    BOOST_REQUIRE(rake != nullptr);

    std::vector<gr_complex> pattern(pattern_length, gr_complex(1.0f, 0.0f));
    rake->set_pattern(pattern);
    BOOST_CHECK_EQUAL(rake->num_fingers(), num_fingers);
}

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_basic_flow)
{
    int num_fingers = 2;
    std::vector<int> delays = {0, 5};
    std::vector<float> gains = {1.0f, 0.8f};
    int pattern_length = 8;

    auto rake = rake_receiver_cc::make(num_fingers, delays, gains, pattern_length);
    BOOST_REQUIRE(rake != nullptr);

    std::vector<gr_complex> input_data(100, gr_complex(1.0f, 0.0f));
    auto source = blocks::vector_source_c::make(input_data, false);
    auto sink = blocks::vector_sink_c::make();

    auto tb = gr::make_top_block("test");
    tb->connect(source, 0, rake, 0);
    tb->connect(rake, 0, sink, 0);

    tb->start();
    tb->wait();
    tb->stop();

    auto output = sink->data();
    BOOST_CHECK_GT(output.size(), 0);
}

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_invalid_delays_size)
{
    int num_fingers = 2;
    std::vector<int> delays = {0};
    std::vector<float> gains = {1.0f, 0.8f};
    int pattern_length = 16;

    BOOST_CHECK_THROW(
        rake_receiver_cc::make(num_fingers, delays, gains, pattern_length),
        std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_invalid_gains_size)
{
    int num_fingers = 2;
    std::vector<int> delays = {0, 10};
    std::vector<float> gains = {1.0f};
    int pattern_length = 16;

    BOOST_CHECK_THROW(
        rake_receiver_cc::make(num_fingers, delays, gains, pattern_length),
        std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_max_fingers)
{
    int num_fingers = 5;
    std::vector<int> delays = {0, 10, 20, 30, 40};
    std::vector<float> gains = {1.0f, 0.8f, 0.6f, 0.4f, 0.2f};
    int pattern_length = 16;

    auto rake = rake_receiver_cc::make(num_fingers, delays, gains, pattern_length);
    BOOST_REQUIRE(rake != nullptr);
    BOOST_CHECK_EQUAL(rake->num_fingers(), num_fingers);
}

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_adaptive_parameters)
{
    int num_fingers = 4;
    std::vector<int> delays = {0, 10, 20, 30};
    std::vector<float> gains = {1.0f, 0.8f, 0.6f, 0.4f};
    int pattern_length = 42;

    auto rake = rake_receiver_cc::make(num_fingers, delays, gains, pattern_length);
    BOOST_REQUIRE(rake != nullptr);

    // Test default values
    BOOST_CHECK_EQUAL(rake->path_search_rate(), 20.0f);
    BOOST_CHECK_EQUAL(rake->tracking_bandwidth(), 120.0f);
    BOOST_CHECK_EQUAL(rake->path_detection_threshold(), 0.5f);
    BOOST_CHECK_EQUAL(rake->lock_threshold(), 0.7f);
    BOOST_CHECK_EQUAL(rake->reassignment_period(), 1.0f);
    BOOST_CHECK_EQUAL(rake->gps_speed(), -1.0f);
    BOOST_CHECK_EQUAL(rake->adaptive_mode(), false);

    // Test setting parameters
    rake->set_path_search_rate(50.0f);
    BOOST_CHECK_EQUAL(rake->path_search_rate(), 50.0f);

    rake->set_tracking_bandwidth(200.0f);
    BOOST_CHECK_EQUAL(rake->tracking_bandwidth(), 200.0f);

    rake->set_path_detection_threshold(0.6f);
    BOOST_CHECK_EQUAL(rake->path_detection_threshold(), 0.6f);

    rake->set_lock_threshold(0.8f);
    BOOST_CHECK_EQUAL(rake->lock_threshold(), 0.8f);

    rake->set_reassignment_period(0.5f);
    BOOST_CHECK_EQUAL(rake->reassignment_period(), 0.5f);
}

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_gps_speed_adaptive)
{
    int num_fingers = 4;
    std::vector<int> delays = {0, 10, 20, 30};
    std::vector<float> gains = {1.0f, 0.8f, 0.6f, 0.4f};
    int pattern_length = 42;

    auto rake = rake_receiver_cc::make(num_fingers, delays, gains, pattern_length);
    BOOST_REQUIRE(rake != nullptr);

    // Enable adaptive mode
    rake->set_adaptive_mode(true);
    BOOST_CHECK_EQUAL(rake->adaptive_mode(), true);

    // Test stationary mode (0-5 km/h) - exact boundary
    rake->set_gps_speed(5.0f);
    BOOST_CHECK_EQUAL(rake->gps_speed(), 5.0f);
    BOOST_CHECK_CLOSE(rake->path_search_rate(), 5.0f, 0.1f);
    BOOST_CHECK_CLOSE(rake->tracking_bandwidth(), 50.0f, 0.1f);
    BOOST_CHECK_CLOSE(rake->reassignment_period(), 2.0f, 0.1f);

    // Test pedestrian mode (5-15 km/h) - exact boundary
    rake->set_gps_speed(15.0f);
    BOOST_CHECK_CLOSE(rake->path_search_rate(), 10.0f, 0.1f);
    BOOST_CHECK_CLOSE(rake->tracking_bandwidth(), 100.0f, 0.1f);
    BOOST_CHECK_CLOSE(rake->reassignment_period(), 1.0f, 0.1f);

    // Test interpolation at 10 km/h (midway between 5-15)
    rake->set_gps_speed(10.0f);
    BOOST_CHECK_CLOSE(rake->path_search_rate(), 7.5f, 0.1f); // (5+10)/2
    BOOST_CHECK_CLOSE(rake->tracking_bandwidth(), 75.0f, 0.1f); // (50+100)/2

    // Test vehicle low-speed mode (15-60 km/h) - exact boundary
    rake->set_gps_speed(60.0f);
    BOOST_CHECK_CLOSE(rake->path_search_rate(), 20.0f, 0.1f);
    BOOST_CHECK_CLOSE(rake->tracking_bandwidth(), 120.0f, 0.1f);
    BOOST_CHECK_CLOSE(rake->reassignment_period(), 1.0f, 0.1f);

    // Test interpolation at 37.5 km/h (midway between 15-60)
    rake->set_gps_speed(37.5f);
    BOOST_CHECK_CLOSE(rake->path_search_rate(), 15.0f, 0.1f); // (10+20)/2
    BOOST_CHECK_CLOSE(rake->tracking_bandwidth(), 110.0f, 0.1f); // (100+120)/2

    // Test vehicle high-speed mode (60-120 km/h) - exact boundary
    rake->set_gps_speed(120.0f);
    BOOST_CHECK_CLOSE(rake->path_search_rate(), 50.0f, 0.1f);
    BOOST_CHECK_CLOSE(rake->tracking_bandwidth(), 200.0f, 0.1f);
    BOOST_CHECK_CLOSE(rake->reassignment_period(), 0.5f, 0.1f);

    // Test interpolation at 90 km/h (midway between 60-120)
    rake->set_gps_speed(90.0f);
    BOOST_CHECK_CLOSE(rake->path_search_rate(), 35.0f, 0.1f); // (20+50)/2
    BOOST_CHECK_CLOSE(rake->tracking_bandwidth(), 160.0f, 0.1f); // (120+200)/2

    // Test very high-speed mode (>120 km/h) - at 200 km/h cap
    rake->set_gps_speed(200.0f);
    BOOST_CHECK_CLOSE(rake->path_search_rate(), 100.0f, 0.1f);
    BOOST_CHECK_CLOSE(rake->tracking_bandwidth(), 300.0f, 0.1f);
    BOOST_CHECK_CLOSE(rake->reassignment_period(), 0.25f, 0.1f);
}

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_nmea0183_parsing)
{
    int num_fingers = 4;
    std::vector<int> delays = {0, 10, 20, 30};
    std::vector<float> gains = {1.0f, 0.8f, 0.6f, 0.4f};
    int pattern_length = 42;

    auto rake = rake_receiver_cc::make(num_fingers, delays, gains, pattern_length);
    BOOST_REQUIRE(rake != nullptr);

    rake->set_adaptive_mode(true);

    // Test GPRMC parsing (speed in knots)
    std::string gprmc = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A";
    bool result = rake->parse_nmea0183(gprmc);
    BOOST_CHECK(result);
    // 22.4 knots * 1.852 = 41.4848 km/h
    BOOST_CHECK_CLOSE(rake->gps_speed(), 41.4848f, 0.1f);

    // Test GPVTG parsing (speed in km/h)
    std::string gpvtg = "$GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48";
    result = rake->parse_nmea0183(gpvtg);
    BOOST_CHECK(result);
    BOOST_CHECK_CLOSE(rake->gps_speed(), 10.2f, 0.1f);
}

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_gpsd_parsing)
{
    int num_fingers = 4;
    std::vector<int> delays = {0, 10, 20, 30};
    std::vector<float> gains = {1.0f, 0.8f, 0.6f, 0.4f};
    int pattern_length = 42;

    auto rake = rake_receiver_cc::make(num_fingers, delays, gains, pattern_length);
    BOOST_REQUIRE(rake != nullptr);

    rake->set_adaptive_mode(true);

    // Test GPSD TPV message parsing (speed in m/s)
    std::string gpsd_tpv =
        "{\"class\":\"TPV\",\"device\":\"/dev/ttyUSB0\",\"time\":\"2024-01-01T12:00:00.000Z\","
        "\"lat\":48.123,\"lon\":11.456,\"speed\":12.5}";
    bool result = rake->parse_gpsd(gpsd_tpv);
    BOOST_CHECK(result);
    // 12.5 m/s * 3.6 = 45.0 km/h
    BOOST_CHECK_CLOSE(rake->gps_speed(), 45.0f, 0.1f);
}

BOOST_AUTO_TEST_CASE(test_rake_receiver_cc_gps_data_auto_parse)
{
    int num_fingers = 4;
    std::vector<int> delays = {0, 10, 20, 30};
    std::vector<float> gains = {1.0f, 0.8f, 0.6f, 0.4f};
    int pattern_length = 42;

    auto rake = rake_receiver_cc::make(num_fingers, delays, gains, pattern_length);
    BOOST_REQUIRE(rake != nullptr);

    rake->set_adaptive_mode(true);

    // Test auto-detection of NMEA0183
    std::string nmea = "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A";
    bool result = rake->parse_gps_data(nmea);
    BOOST_CHECK(result);

    // Test auto-detection of GPSD
    std::string gpsd = "{\"class\":\"TPV\",\"speed\":10.0}";
    result = rake->parse_gps_data(gpsd);
    BOOST_CHECK(result);
    BOOST_CHECK_CLOSE(rake->gps_speed(), 36.0f, 0.1f); // 10 m/s = 36 km/h
}

} /* namespace rake_receiver */
} /* namespace gr */
