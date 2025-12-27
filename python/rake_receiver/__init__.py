#
# Copyright 2008,2009 Free Software Foundation, Inc.
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

# The presence of this file turns this directory into a Python package

"""
This is the GNU Radio RAKE_RECEIVER module. Place your Python package
description here (python/__init__.py).
"""
# import pybind11 generated symbols into the rake_receiver namespace
try:
    # this might fail if the module is python-only
    from .rake_receiver_python import *  # noqa: F403, F401
except ModuleNotFoundError:
    pass

# import any pure python here
#
