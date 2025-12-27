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
#include <vector>

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

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace rake_receiver
} // namespace gr

#endif /* INCLUDED_RAKE_RECEIVER_RAKE_RECEIVER_CC_IMPL_H */
