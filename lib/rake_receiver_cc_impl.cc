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

#include <gnuradio/io_signature.h>
#include <gnuradio/gr_complex.h>
#include <pmt/pmt.h>
#include "rake_receiver_cc_impl.h"

namespace gr {
namespace rake_receiver {

rake_receiver_cc::sptr rake_receiver_cc::make(int num_fingers,
                                              const std::vector<int>& delays,
                                              const std::vector<float>& gains,
                                              int pattern_length)
{
    return gnuradio::make_block_sptr<rake_receiver_cc_impl>(
        num_fingers, delays, gains, pattern_length);
}

rake_receiver_cc_impl::rake_receiver_cc_impl(int num_fingers,
                                               const std::vector<int>& delays,
                                               const std::vector<float>& gains,
                                               int pattern_length)
    : gr::sync_block("rake_receiver_cc",
                     gr::io_signature::make(1, 1, sizeof(gr_complex)),
                     gr::io_signature::make(1, 1, sizeof(gr_complex))),
      d_num_fingers(std::min(num_fingers, 5)),
      d_pattern_length(pattern_length),
      d_delays(d_num_fingers),
      d_gains(d_num_fingers),
      d_gps_speed_kmh(-1.0f),
      d_path_search_rate_hz(20.0f),
      d_tracking_bandwidth_hz(120.0f),
      d_path_detection_threshold(0.5f),
      d_lock_threshold(0.7f),
      d_reassignment_period_s(1.0f),
      d_adaptive_mode(false),
      d_sample_rate(1.0f),
      d_gps_source("none"),
      d_serial_device("/dev/ttyUSB0"),
      d_serial_baud_rate(4800),
      d_gpsd_host("localhost"),
      d_gpsd_port(2947),
      d_gps_running(false)
{
    if (d_num_fingers < 1 || d_num_fingers > 5) {
        throw std::invalid_argument("Number of fingers must be between 1 and 5");
    }

    if (delays.size() != static_cast<size_t>(d_num_fingers)) {
        throw std::invalid_argument("Number of delays must match number of fingers");
    }

    if (gains.size() != static_cast<size_t>(d_num_fingers)) {
        throw std::invalid_argument("Number of gains must match number of fingers");
    }

    int max_delay = 0;
    for (int i = 0; i < d_num_fingers; i++) {
        d_delays[i] = delays[i];
        d_gains[i] = gains[i];
        if (delays[i] > max_delay) {
            max_delay = delays[i];
        }
    }

    set_history(max_delay + d_pattern_length + 1);
    set_output_multiple(1);

    d_pattern.resize(d_pattern_length, gr_complex(1.0f, 0.0f));

    // Set default to 4 fingers if not specified
    if (d_num_fingers == 0) {
        d_num_fingers = 4;
    }

    // Register message input port for GPS data
    message_port_register_in(pmt::mp("gps"));
    set_msg_handler(pmt::mp("gps"),
                    [this](pmt::pmt_t msg) { this->handle_gps_message(msg); });
}

rake_receiver_cc_impl::~rake_receiver_cc_impl() {}

void rake_receiver_cc_impl::set_delays(const std::vector<int>& delays)
{
    if (delays.size() != static_cast<size_t>(d_num_fingers)) {
        throw std::invalid_argument("Number of delays must match number of fingers");
    }

    int max_delay = 0;
    for (int i = 0; i < d_num_fingers; i++) {
        d_delays[i] = delays[i];
        if (delays[i] > max_delay) {
            max_delay = delays[i];
        }
    }

    set_history(max_delay + d_pattern_length + 1);
}

void rake_receiver_cc_impl::set_gains(const std::vector<float>& gains)
{
    if (gains.size() != static_cast<size_t>(d_num_fingers)) {
        throw std::invalid_argument("Number of gains must match number of fingers");
    }

    for (int i = 0; i < d_num_fingers; i++) {
        d_gains[i] = gains[i];
    }
}

int rake_receiver_cc_impl::num_fingers() const { return d_num_fingers; }

void rake_receiver_cc_impl::set_pattern(const std::vector<gr_complex>& pattern)
{
    if (pattern.size() != static_cast<size_t>(d_pattern_length)) {
        throw std::invalid_argument("Pattern length must match pattern_length parameter");
    }
    d_pattern = pattern;
}

int rake_receiver_cc_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    const gr_complex* in = (const gr_complex*)input_items[0];
    gr_complex* out = (gr_complex*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        gr_complex combined = gr_complex(0.0f, 0.0f);

        for (int finger = 0; finger < d_num_fingers; finger++) {
            int delay = d_delays[finger];
            gr_complex finger_output = gr_complex(0.0f, 0.0f);

            if (static_cast<unsigned int>(i + delay + d_pattern_length) <= history() - 1) {
                const gr_complex* delayed_input = &in[i + delay];

                for (int j = 0; j < d_pattern_length; j++) {
                    finger_output += delayed_input[j] * std::conj(d_pattern[j]);
                }
            }

            combined += d_gains[finger] * finger_output;
        }

        out[i] = combined;
    }

    return noutput_items;
}

void rake_receiver_cc_impl::set_gps_speed(float speed_kmh)
{
    d_gps_speed_kmh = speed_kmh;
    if (d_adaptive_mode && speed_kmh >= 0.0f) {
        update_adaptive_parameters();
    }
}

float rake_receiver_cc_impl::gps_speed() const { return d_gps_speed_kmh; }

void rake_receiver_cc_impl::set_path_search_rate(float rate_hz)
{
    d_path_search_rate_hz = rate_hz;
}

float rake_receiver_cc_impl::path_search_rate() const { return d_path_search_rate_hz; }

void rake_receiver_cc_impl::set_tracking_bandwidth(float bandwidth_hz)
{
    d_tracking_bandwidth_hz = bandwidth_hz;
}

float rake_receiver_cc_impl::tracking_bandwidth() const { return d_tracking_bandwidth_hz; }

void rake_receiver_cc_impl::set_path_detection_threshold(float threshold)
{
    d_path_detection_threshold = threshold;
}

float rake_receiver_cc_impl::path_detection_threshold() const
{
    return d_path_detection_threshold;
}

void rake_receiver_cc_impl::set_lock_threshold(float threshold)
{
    d_lock_threshold = threshold;
}

float rake_receiver_cc_impl::lock_threshold() const { return d_lock_threshold; }

void rake_receiver_cc_impl::set_reassignment_period(float period_s)
{
    d_reassignment_period_s = period_s;
}

float rake_receiver_cc_impl::reassignment_period() const { return d_reassignment_period_s; }

void rake_receiver_cc_impl::set_adaptive_mode(bool enable)
{
    d_adaptive_mode = enable;
    if (enable && d_gps_speed_kmh >= 0.0f) {
        update_adaptive_parameters();
    }
}

bool rake_receiver_cc_impl::adaptive_mode() const { return d_adaptive_mode; }

void rake_receiver_cc_impl::update_adaptive_parameters()
{
    if (!d_adaptive_mode || d_gps_speed_kmh < 0.0f) {
        return;
    }
    apply_speed_category(d_gps_speed_kmh);
}

void rake_receiver_cc_impl::apply_speed_category(float speed_kmh)
{
    if (speed_kmh < 0.0f) {
        return;
    }

    // Speed category boundaries
    const float speed_stationary = 5.0f;
    const float speed_pedestrian = 15.0f;
    const float speed_low = 60.0f;
    const float speed_high = 120.0f;

    // Parameter values for each category
    struct SpeedParams {
        float path_search_rate;
        float tracking_bandwidth;
        float reassignment_period;
        int num_fingers;
    };

    SpeedParams stationary = {5.0f, 50.0f, 2.0f, 3};
    SpeedParams pedestrian = {10.0f, 100.0f, 1.0f, 3};
    SpeedParams low_speed = {20.0f, 120.0f, 1.0f, 4};
    SpeedParams high_speed = {50.0f, 200.0f, 0.5f, 4};
    SpeedParams very_high = {100.0f, 300.0f, 0.25f, 4};

    SpeedParams lower, upper;
    float lower_speed, upper_speed;
    float alpha; // Interpolation factor (0.0 to 1.0)

    // Determine which category range we're in and interpolate
    if (speed_kmh <= speed_stationary) {
        // Stationary (0-5 km/h) - no interpolation needed
        d_path_search_rate_hz = stationary.path_search_rate;
        d_tracking_bandwidth_hz = stationary.tracking_bandwidth;
        d_reassignment_period_s = stationary.reassignment_period;
        d_num_fingers = stationary.num_fingers;
    } else if (speed_kmh <= speed_pedestrian) {
        // Interpolate between Stationary and Pedestrian (5-15 km/h)
        lower = stationary;
        upper = pedestrian;
        lower_speed = speed_stationary;
        upper_speed = speed_pedestrian;
        alpha = (speed_kmh - lower_speed) / (upper_speed - lower_speed);
        d_path_search_rate_hz = lower.path_search_rate +
                                alpha * (upper.path_search_rate - lower.path_search_rate);
        d_tracking_bandwidth_hz = lower.tracking_bandwidth +
                                  alpha * (upper.tracking_bandwidth - lower.tracking_bandwidth);
        d_reassignment_period_s = lower.reassignment_period +
                                  alpha * (upper.reassignment_period - lower.reassignment_period);
        d_num_fingers = (speed_kmh < (lower_speed + upper_speed) / 2.0f) ? lower.num_fingers
                                                                         : upper.num_fingers;
    } else if (speed_kmh <= speed_low) {
        // Interpolate between Pedestrian and Low-Speed (15-60 km/h)
        lower = pedestrian;
        upper = low_speed;
        lower_speed = speed_pedestrian;
        upper_speed = speed_low;
        alpha = (speed_kmh - lower_speed) / (upper_speed - lower_speed);
        d_path_search_rate_hz = lower.path_search_rate +
                                alpha * (upper.path_search_rate - lower.path_search_rate);
        d_tracking_bandwidth_hz = lower.tracking_bandwidth +
                                  alpha * (upper.tracking_bandwidth - lower.tracking_bandwidth);
        d_reassignment_period_s = lower.reassignment_period +
                                  alpha * (upper.reassignment_period - lower.reassignment_period);
        d_num_fingers = (speed_kmh < (lower_speed + upper_speed) / 2.0f) ? lower.num_fingers
                                                                         : upper.num_fingers;
    } else if (speed_kmh <= speed_high) {
        // Interpolate between Low-Speed and High-Speed (60-120 km/h)
        lower = low_speed;
        upper = high_speed;
        lower_speed = speed_low;
        upper_speed = speed_high;
        alpha = (speed_kmh - lower_speed) / (upper_speed - lower_speed);
        d_path_search_rate_hz = lower.path_search_rate +
                                alpha * (upper.path_search_rate - lower.path_search_rate);
        d_tracking_bandwidth_hz = lower.tracking_bandwidth +
                                  alpha * (upper.tracking_bandwidth - lower.tracking_bandwidth);
        d_reassignment_period_s = lower.reassignment_period +
                                  alpha * (upper.reassignment_period - lower.reassignment_period);
        d_num_fingers = upper.num_fingers; // Both use 4 fingers
    } else {
        // Interpolate between High-Speed and Very High-Speed (120+ km/h)
        lower = high_speed;
        upper = very_high;
        lower_speed = speed_high;
        upper_speed = 200.0f; // Cap interpolation at 200 km/h for very high speeds
        float clamped_speed = (speed_kmh > upper_speed) ? upper_speed : speed_kmh;
        alpha = (clamped_speed - lower_speed) / (upper_speed - lower_speed);
        if (alpha > 1.0f)
            alpha = 1.0f;
        d_path_search_rate_hz = lower.path_search_rate +
                                alpha * (upper.path_search_rate - lower.path_search_rate);
        d_tracking_bandwidth_hz = lower.tracking_bandwidth +
                                  alpha * (upper.tracking_bandwidth - lower.tracking_bandwidth);
        d_reassignment_period_s = lower.reassignment_period +
                                  alpha * (upper.reassignment_period - lower.reassignment_period);
        d_num_fingers = upper.num_fingers; // Both use 4 fingers
    }
}

bool rake_receiver_cc_impl::parse_gps_data(const std::string& gps_data)
{
    float speed = parse_gps_speed(gps_data);
    if (speed >= 0.0f) {
        set_gps_speed(speed);
        return true;
    }
    return false;
}

bool rake_receiver_cc_impl::parse_nmea0183(const std::string& nmea_message)
{
    float speed = parse_nmea0183_speed(nmea_message);
    if (speed >= 0.0f) {
        set_gps_speed(speed);
        return true;
    }
    return false;
}

bool rake_receiver_cc_impl::parse_gpsd(const std::string& gpsd_json)
{
    float speed = parse_gpsd_speed(gpsd_json);
    if (speed >= 0.0f) {
        set_gps_speed(speed);
        return true;
    }
    return false;
}

void rake_receiver_cc_impl::handle_gps_message(pmt::pmt_t msg)
{
    // Extract string from PMT message
    if (pmt::is_symbol(msg)) {
        std::string gps_data = pmt::symbol_to_string(msg);
        parse_gps_data(gps_data);
    } else if (pmt::is_u8vector(msg)) {
        // Handle byte vector (common for serial data)
        std::vector<uint8_t> vec = pmt::u8vector_elements(msg);
        std::string gps_data(vec.begin(), vec.end());
        parse_gps_data(gps_data);
    } else {
        // Try to convert to string
        try {
            std::string gps_data = pmt::symbol_to_string(msg);
            parse_gps_data(gps_data);
        } catch (...) {
            // If conversion fails, try as string representation
            std::string gps_data = pmt::write_string(msg);
            parse_gps_data(gps_data);
        }
    }
}

void rake_receiver_cc_impl::set_gps_source(const std::string& source_type)
{
    d_gps_source = source_type;
    if (source_type == "none" && d_gps_running) {
        stop_gps();
    }
}

std::string rake_receiver_cc_impl::gps_source() const { return d_gps_source; }

void rake_receiver_cc_impl::set_serial_device(const std::string& device_path)
{
    d_serial_device = device_path;
}

std::string rake_receiver_cc_impl::serial_device() const { return d_serial_device; }

void rake_receiver_cc_impl::set_serial_baud_rate(int baud_rate)
{
    d_serial_baud_rate = baud_rate;
}

int rake_receiver_cc_impl::serial_baud_rate() const { return d_serial_baud_rate; }

void rake_receiver_cc_impl::set_gpsd_host(const std::string& host)
{
    d_gpsd_host = host;
}

std::string rake_receiver_cc_impl::gpsd_host() const { return d_gpsd_host; }

void rake_receiver_cc_impl::set_gpsd_port(int port)
{
    d_gpsd_port = port;
}

int rake_receiver_cc_impl::gpsd_port() const { return d_gpsd_port; }

bool rake_receiver_cc_impl::start_gps()
{
    // Note: Actual GPS connection is typically handled by external blocks
    // This method is provided for compatibility and future extension
    // Users should use the message port or external GPS blocks
    d_gps_running = true;
    return true;
}

void rake_receiver_cc_impl::stop_gps() { d_gps_running = false; }

} /* namespace rake_receiver */
} /* namespace gr */
