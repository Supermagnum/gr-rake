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

} /* namespace rake_receiver */
} /* namespace gr */
