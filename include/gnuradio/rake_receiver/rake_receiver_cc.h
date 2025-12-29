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

    /*!
     * \brief Set GPS speed for adaptive parameter adjustment (km/h)
     *
     * \param speed_kmh GPS speed in km/h. Negative values disable adaptive mode.
     */
    virtual void set_gps_speed(float speed_kmh) = 0;

    /*!
     * \brief Get current GPS speed setting
     *
     * \return Current GPS speed in km/h, or -1 if adaptive mode is disabled
     */
    virtual float gps_speed() const = 0;

    /*!
     * \brief Set path search rate (Hz)
     *
     * \param rate_hz Path search rate in Hz
     */
    virtual void set_path_search_rate(float rate_hz) = 0;

    /*!
     * \brief Get current path search rate
     *
     * \return Path search rate in Hz
     */
    virtual float path_search_rate() const = 0;

    /*!
     * \brief Set tracking bandwidth (Hz)
     *
     * \param bandwidth_hz Tracking bandwidth in Hz
     */
    virtual void set_tracking_bandwidth(float bandwidth_hz) = 0;

    /*!
     * \brief Get current tracking bandwidth
     *
     * \return Tracking bandwidth in Hz
     */
    virtual float tracking_bandwidth() const = 0;

    /*!
     * \brief Set path detection threshold (fraction of peak correlation)
     *
     * \param threshold Threshold value (default 0.5)
     */
    virtual void set_path_detection_threshold(float threshold) = 0;

    /*!
     * \brief Get current path detection threshold
     *
     * \return Threshold value
     */
    virtual float path_detection_threshold() const = 0;

    /*!
     * \brief Set lock detector threshold (correlation value)
     *
     * \param threshold Lock threshold (default 0.7)
     */
    virtual void set_lock_threshold(float threshold) = 0;

    /*!
     * \brief Get current lock threshold
     *
     * \return Lock threshold value
     */
    virtual float lock_threshold() const = 0;

    /*!
     * \brief Set reassignment period (seconds)
     *
     * \param period_s Reassignment period in seconds
     */
    virtual void set_reassignment_period(float period_s) = 0;

    /*!
     * \brief Get current reassignment period
     *
     * \return Reassignment period in seconds
     */
    virtual float reassignment_period() const = 0;

    /*!
     * \brief Enable or disable adaptive mode based on GPS speed
     *
     * \param enable True to enable adaptive mode, false to use fixed parameters
     */
    virtual void set_adaptive_mode(bool enable) = 0;

    /*!
     * \brief Check if adaptive mode is enabled
     *
     * \return True if adaptive mode is enabled
     */
    virtual bool adaptive_mode() const = 0;

    /*!
     * \brief Parse GPS data from NMEA0183 or GPSD format and update speed
     *
     * \param gps_data GPS data string (NMEA0183 or GPSD JSON)
     * \return True if parsing successful and speed updated
     */
    virtual bool parse_gps_data(const std::string& gps_data) = 0;

    /*!
     * \brief Parse NMEA0183 message and update GPS speed
     *
     * \param nmea_message NMEA0183 message string
     * \return True if parsing successful
     */
    virtual bool parse_nmea0183(const std::string& nmea_message) = 0;

    /*!
     * \brief Parse GPSD JSON message and update GPS speed
     *
     * \param gpsd_json GPSD JSON message string
     * \return True if parsing successful
     */
    virtual bool parse_gpsd(const std::string& gpsd_json) = 0;

    /*!
     * \brief Set GPS source type
     *
     * \param source_type "serial" for serial GPS, "gpsd" for GPSD, "none" to disable
     */
    virtual void set_gps_source(const std::string& source_type) = 0;

    /*!
     * \brief Get current GPS source type
     *
     * \return Current GPS source type
     */
    virtual std::string gps_source() const = 0;

    /*!
     * \brief Set serial GPS device path
     *
     * \param device_path Serial device path (e.g., "/dev/ttyUSB0")
     */
    virtual void set_serial_device(const std::string& device_path) = 0;

    /*!
     * \brief Get current serial device path
     *
     * \return Serial device path
     */
    virtual std::string serial_device() const = 0;

    /*!
     * \brief Set serial baud rate
     *
     * \param baud_rate Baud rate (e.g., 4800, 9600)
     */
    virtual void set_serial_baud_rate(int baud_rate) = 0;

    /*!
     * \brief Get current serial baud rate
     *
     * \return Baud rate
     */
    virtual int serial_baud_rate() const = 0;

    /*!
     * \brief Set GPSD host
     *
     * \param host GPSD hostname or IP address (e.g., "localhost")
     */
    virtual void set_gpsd_host(const std::string& host) = 0;

    /*!
     * \brief Get current GPSD host
     *
     * \return GPSD host
     */
    virtual std::string gpsd_host() const = 0;

    /*!
     * \brief Set GPSD port
     *
     * \param port GPSD port number (e.g., 2947)
     */
    virtual void set_gpsd_port(int port) = 0;

    /*!
     * \brief Get current GPSD port
     *
     * \return GPSD port number
     */
    virtual int gpsd_port() const = 0;

    /*!
     * \brief Start GPS connection (if configured)
     *
     * \return True if connection started successfully
     */
    virtual bool start_gps() = 0;

    /*!
     * \brief Stop GPS connection
     */
    virtual void stop_gps() = 0;
};

} // namespace rake_receiver
} // namespace gr

#endif /* INCLUDED_RAKE_RECEIVER_RAKE_RECEIVER_CC_H */
