/*
 * Copyright 2024
 *
 * This file is part of gr-rake_receiver
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_RAKE_RECEIVER_RAKE_RECEIVER_CC_IMPL_H
#define INCLUDED_RAKE_RECEIVER_RAKE_RECEIVER_CC_IMPL_H

#include <gnuradio/rake_receiver/rake_receiver_cc.h>
#include <gnuradio/gr_complex.h>
#include "gps_parser.h"
#include <vector>
#include <string>

namespace gr {
namespace rake_receiver {

class rake_receiver_cc_impl : public rake_receiver_cc
{
private:
    int d_num_fingers;
    int d_pattern_length;
    std::vector<int> d_delays;
    std::vector<float> d_gains;
    std::vector<gr_complex> d_pattern;

    // Adaptive parameters
    float d_gps_speed_kmh;
    float d_path_search_rate_hz;
    float d_tracking_bandwidth_hz;
    float d_path_detection_threshold;
    float d_lock_threshold;
    float d_reassignment_period_s;
    bool d_adaptive_mode;
    float d_sample_rate;

    // Helper methods
    void update_adaptive_parameters();
    void apply_speed_category(float speed_kmh);

public:
    rake_receiver_cc_impl(int num_fingers,
                          const std::vector<int>& delays,
                          const std::vector<float>& gains,
                          int pattern_length);
    ~rake_receiver_cc_impl();

    void set_delays(const std::vector<int>& delays) override;
    void set_gains(const std::vector<float>& gains) override;
    int num_fingers() const override;
    void set_pattern(const std::vector<gr_complex>& pattern) override;

    void set_gps_speed(float speed_kmh) override;
    float gps_speed() const override;
    void set_path_search_rate(float rate_hz) override;
    float path_search_rate() const override;
    void set_tracking_bandwidth(float bandwidth_hz) override;
    float tracking_bandwidth() const override;
    void set_path_detection_threshold(float threshold) override;
    float path_detection_threshold() const override;
    void set_lock_threshold(float threshold) override;
    float lock_threshold() const override;
    void set_reassignment_period(float period_s) override;
    float reassignment_period() const override;
    void set_adaptive_mode(bool enable) override;
    bool adaptive_mode() const override;

    bool parse_gps_data(const std::string& gps_data) override;
    bool parse_nmea0183(const std::string& nmea_message) override;
    bool parse_gpsd(const std::string& gpsd_json) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace rake_receiver
} // namespace gr

#endif /* INCLUDED_RAKE_RECEIVER_RAKE_RECEIVER_CC_IMPL_H */
