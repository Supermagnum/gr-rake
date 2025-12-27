/*
 * Copyright 2024
 *
 * This file is part of gr-rake_receiver
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/rake_receiver/rake_receiver_cc.h>

void bind_rake_receiver_cc(py::module& m)
{
    using rake_receiver_cc = gr::rake_receiver::rake_receiver_cc;

    py::class_<rake_receiver_cc,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<rake_receiver_cc>>(m, "rake_receiver_cc")

        .def(py::init(&rake_receiver_cc::make),
             py::arg("num_fingers"),
             py::arg("delays"),
             py::arg("gains"),
             py::arg("pattern_length"),
             "Make a RAKE receiver block")

        .def("set_delays",
             &rake_receiver_cc::set_delays,
             py::arg("delays"),
             "Set the delays for each finger")

        .def("set_gains",
             &rake_receiver_cc::set_gains,
             py::arg("gains"),
             "Set the gains for each finger")

        .def("num_fingers",
             &rake_receiver_cc::num_fingers,
             "Get the current number of fingers")

        .def("set_pattern",
             &rake_receiver_cc::set_pattern,
             py::arg("pattern"),
             "Set the correlation pattern")

        .def("set_gps_speed",
             &rake_receiver_cc::set_gps_speed,
             py::arg("speed_kmh"),
             "Set GPS speed for adaptive parameter adjustment (km/h)")

        .def("gps_speed",
             &rake_receiver_cc::gps_speed,
             "Get current GPS speed setting")

        .def("set_path_search_rate",
             &rake_receiver_cc::set_path_search_rate,
             py::arg("rate_hz"),
             "Set path search rate (Hz)")

        .def("path_search_rate",
             &rake_receiver_cc::path_search_rate,
             "Get current path search rate")

        .def("set_tracking_bandwidth",
             &rake_receiver_cc::set_tracking_bandwidth,
             py::arg("bandwidth_hz"),
             "Set tracking bandwidth (Hz)")

        .def("tracking_bandwidth",
             &rake_receiver_cc::tracking_bandwidth,
             "Get current tracking bandwidth")

        .def("set_path_detection_threshold",
             &rake_receiver_cc::set_path_detection_threshold,
             py::arg("threshold"),
             "Set path detection threshold (fraction of peak correlation)")

        .def("path_detection_threshold",
             &rake_receiver_cc::path_detection_threshold,
             "Get current path detection threshold")

        .def("set_lock_threshold",
             &rake_receiver_cc::set_lock_threshold,
             py::arg("threshold"),
             "Set lock detector threshold (correlation value)")

        .def("lock_threshold",
             &rake_receiver_cc::lock_threshold,
             "Get current lock threshold")

        .def("set_reassignment_period",
             &rake_receiver_cc::set_reassignment_period,
             py::arg("period_s"),
             "Set reassignment period (seconds)")

        .def("reassignment_period",
             &rake_receiver_cc::reassignment_period,
             "Get current reassignment period")

        .def("set_adaptive_mode",
             &rake_receiver_cc::set_adaptive_mode,
             py::arg("enable"),
             "Enable or disable adaptive mode based on GPS speed")

        .def("adaptive_mode",
             &rake_receiver_cc::adaptive_mode,
             "Check if adaptive mode is enabled")

        .def("parse_gps_data",
             &rake_receiver_cc::parse_gps_data,
             py::arg("gps_data"),
             "Parse GPS data from NMEA0183 or GPSD format and update speed")

        .def("parse_nmea0183",
             &rake_receiver_cc::parse_nmea0183,
             py::arg("nmea_message"),
             "Parse NMEA0183 message and update GPS speed")

        .def("parse_gpsd",
             &rake_receiver_cc::parse_gpsd,
             py::arg("gpsd_json"),
             "Parse GPSD JSON message and update GPS speed");
}
