/*
 * Copyright 2024
 *
 * This file is part of gr-rake_receiver
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_RAKE_RECEIVER_GPS_PARSER_H
#define INCLUDED_RAKE_RECEIVER_GPS_PARSER_H

#include <string>
#include <vector>

namespace gr {
namespace rake_receiver {

/*!
 * \brief Parse NMEA0183 message and extract speed
 *
 * \param nmea_message NMEA0183 message string (e.g., "$GPRMC,...")
 * \return Speed in km/h, or -1.0 if parsing fails or speed not available
 */
float parse_nmea0183_speed(const std::string& nmea_message);

/*!
 * \brief Parse GPSD JSON message and extract speed
 *
 * \param gpsd_json GPSD JSON message string
 * \return Speed in km/h, or -1.0 if parsing fails or speed not available
 */
float parse_gpsd_speed(const std::string& gpsd_json);

/*!
 * \brief Parse GPS speed from either NMEA0183 or GPSD format
 *
 * \param gps_data GPS data string (NMEA0183 or GPSD JSON)
 * \return Speed in km/h, or -1.0 if parsing fails
 */
float parse_gps_speed(const std::string& gps_data);

/*!
 * \brief Check if string is NMEA0183 format
 *
 * \param data String to check
 * \return True if appears to be NMEA0183 format
 */
bool is_nmea0183(const std::string& data);

/*!
 * \brief Check if string is GPSD JSON format
 *
 * \param data String to check
 * \return True if appears to be GPSD JSON format
 */
bool is_gpsd_json(const std::string& data);

} // namespace rake_receiver
} // namespace gr

#endif /* INCLUDED_RAKE_RECEIVER_GPS_PARSER_H */
