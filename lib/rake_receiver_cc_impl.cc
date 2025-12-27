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
      d_gains(d_num_fingers)
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

} /* namespace rake_receiver */
} /* namespace gr */
