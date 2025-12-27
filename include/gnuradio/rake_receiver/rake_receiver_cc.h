/*
 * Copyright 2024
 *
 * This file is part of gr-rake_receiver
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_RAKE_RECEIVER_RAKE_RECEIVER_CC_H
#define INCLUDED_RAKE_RECEIVER_RAKE_RECEIVER_CC_H

#include <gnuradio/rake_receiver/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/gr_complex.h>

namespace gr {
namespace rake_receiver {

/*!
 * \brief RAKE Receiver with configurable number of fingers
 * \ingroup rake_receiver
 *
 * This block implements a RAKE receiver that combines multiple delayed
 * versions of a signal. The number of fingers is configurable (max 5).
 * Each finger correlates the input signal with a known pattern at a
 * specific delay and combines the results.
 */
class RAKE_RECEIVER_API rake_receiver_cc : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<rake_receiver_cc> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of rake_receiver::rake_receiver_cc.
     *
     * \param num_fingers Number of RAKE fingers (1-5)
     * \param delays Vector of delay values for each finger (in samples)
     * \param gains Vector of gain values for each finger (for combining)
     * \param pattern_length Length of the correlation pattern
     */
    static sptr make(int num_fingers,
                     const std::vector<int>& delays,
                     const std::vector<float>& gains,
                     int pattern_length);

    /*!
     * \brief Set the delays for each finger
     *
     * \param delays Vector of delay values (in samples)
     */
    virtual void set_delays(const std::vector<int>& delays) = 0;

    /*!
     * \brief Set the gains for each finger
     *
     * \param gains Vector of gain values
     */
    virtual void set_gains(const std::vector<float>& gains) = 0;

    /*!
     * \brief Get the current number of fingers
     *
     * \return Number of active fingers
     */
    virtual int num_fingers() const = 0;

    /*!
     * \brief Set the correlation pattern
     *
     * \param pattern Vector of complex values representing the correlation pattern
     */
    virtual void set_pattern(const std::vector<gr_complex>& pattern) = 0;
};

} // namespace rake_receiver
} // namespace gr

#endif /* INCLUDED_RAKE_RECEIVER_RAKE_RECEIVER_CC_H */
