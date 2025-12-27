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

#include "gps_parser.h"
#include <sstream>
#include <algorithm>
#include <cmath>

namespace gr {
namespace rake_receiver {

bool is_nmea0183(const std::string& data)
{
    if (data.empty()) {
        return false;
    }
    return data[0] == '$' || (data.length() > 0 && data.find("$") != std::string::npos);
}

bool is_gpsd_json(const std::string& data)
{
    if (data.empty()) {
        return false;
    }
    std::string trimmed = data;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    return trimmed[0] == '{' || trimmed.find("\"class\"") != std::string::npos;
}

float parse_nmea0183_speed(const std::string& nmea_message)
{
    if (nmea_message.empty() || nmea_message[0] != '$') {
        return -1.0f;
    }

    // Parse GPRMC (Recommended Minimum Course) - contains speed
    if (nmea_message.find("$GPRMC") == 0 || nmea_message.find("$GNRMC") == 0) {
        std::vector<std::string> fields;
        std::stringstream ss(nmea_message);
        std::string field;

        while (std::getline(ss, field, ',')) {
            fields.push_back(field);
        }

        // GPRMC format: $GPRMC,time,status,lat,N/S,lon,E/W,speed_knots,course,date,mag_var,E/W*checksum
        // Field 7 (index 6) is speed in knots
        if (fields.size() >= 8) {
            try {
                float speed_knots = std::stof(fields[7]);
                // Convert knots to km/h: 1 knot = 1.852 km/h
                return speed_knots * 1.852f;
            } catch (...) {
                return -1.0f;
            }
        }
    }

    // Parse GPVTG (Track Made Good and Ground Speed) - contains speed
    if (nmea_message.find("$GPVTG") == 0 || nmea_message.find("$GNVTG") == 0) {
        std::vector<std::string> fields;
        std::stringstream ss(nmea_message);
        std::string field;

        while (std::getline(ss, field, ',')) {
            fields.push_back(field);
        }

        // GPVTG format: $GPVTG,course1,T,course2,M,speed_knots,N,speed_kmh,K*checksum
        // Field 7 (index 6) is speed in km/h
        if (fields.size() >= 8) {
            try {
                return std::stof(fields[7]);
            } catch (...) {
                return -1.0f;
            }
        }
    }

    return -1.0f;
}

float parse_gpsd_speed(const std::string& gpsd_json)
{
    if (gpsd_json.empty()) {
        return -1.0f;
    }

    // Simple JSON parsing for GPSD format
    // GPSD TPV (Time-Position-Velocity) message format:
    // {"class":"TPV","device":"/dev/ttyUSB0","time":"2024-01-01T12:00:00.000Z","lat":...,"lon":...,"speed":...}

    // Look for "speed" field
    size_t speed_pos = gpsd_json.find("\"speed\"");
    if (speed_pos == std::string::npos) {
        return -1.0f;
    }

    // Find the colon after "speed"
    size_t colon_pos = gpsd_json.find(':', speed_pos);
    if (colon_pos == std::string::npos) {
        return -1.0f;
    }

    // Find the value (skip whitespace)
    size_t value_start = colon_pos + 1;
    while (value_start < gpsd_json.length() &&
           (gpsd_json[value_start] == ' ' || gpsd_json[value_start] == '\t')) {
        value_start++;
    }

    // Find the end of the value (comma, }, or whitespace)
    size_t value_end = value_start;
    while (value_end < gpsd_json.length() && gpsd_json[value_end] != ',' &&
           gpsd_json[value_end] != '}' && gpsd_json[value_end] != ' ' &&
           gpsd_json[value_end] != '\t' && gpsd_json[value_end] != '\n') {
        value_end++;
    }

    if (value_end <= value_start) {
        return -1.0f;
    }

    try {
        std::string speed_str = gpsd_json.substr(value_start, value_end - value_start);
        float speed_ms = std::stof(speed_str);
        // GPSD speed is in m/s, convert to km/h: 1 m/s = 3.6 km/h
        return speed_ms * 3.6f;
    } catch (...) {
        return -1.0f;
    }
}

float parse_gps_speed(const std::string& gps_data)
{
    if (gps_data.empty()) {
        return -1.0f;
    }

    // Try NMEA0183 first
    if (is_nmea0183(gps_data)) {
        float speed = parse_nmea0183_speed(gps_data);
        if (speed >= 0.0f) {
            return speed;
        }
    }

    // Try GPSD JSON
    if (is_gpsd_json(gps_data)) {
        float speed = parse_gpsd_speed(gps_data);
        if (speed >= 0.0f) {
            return speed;
        }
    }

    return -1.0f;
}

} // namespace rake_receiver
} // namespace gr
