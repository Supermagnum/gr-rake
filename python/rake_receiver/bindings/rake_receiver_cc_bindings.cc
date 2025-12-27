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
             "Set the correlation pattern");
}
