# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT rake_receiver_sources)
    message(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT rake_receiver_sources)

########################################################################
# Check for pygccxml
########################################################################
gr_python_check_module_raw("pygccxml" "import pygccxml" PYGCCXML_FOUND)

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND rake_receiver_python_files python_bindings.cc rake_receiver_cc_bindings.cc)

gr_pybind_make_oot(rake_receiver ../../.. gr::rake_receiver "${rake_receiver_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(
    TARGET rake_receiver_python
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:rake_receiver_python>
            ${PROJECT_BINARY_DIR}/test_modules/gnuradio/rake_receiver/)

install(
    TARGETS rake_receiver_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/rake_receiver
    COMPONENT pythonapi)
