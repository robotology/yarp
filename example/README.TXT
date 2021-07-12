# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

This directory is reserved for examples of the use of YARP and related
libraries.

* hello

    Writes "hello world", with a small delay between the words.
    Just to test that the YARP library can be found.

* ace_check

    A project that doesn't use YARP at all, just to test that
    ACE is correctly set up.  Contains the FindAce macro, and stripped
    down versions of some of YARP's clases.

* framerate

    Read images from a framegrabber and display the frame rate.
    *UPDATE* now works with any port in standard network format.

* os

    Some examples of communication code

* imagemagick

    Interface with the C++ Magick library

* property

    Reading config files and command line arguments in a way
    that is compatible with configuring drivers and sending
    network messages.

* remote_check

    Tests the remote framegrabber device

* dev_check

    Tests the YARP factories for producing interfaces to
    device drivers.

* game

    A port of a simple network game to YARP

* matlab
    Examples that show how to call YARP from Matlab

* opencv
    An example of image sharing between opencv and yarp

* matrix
    Examples of how to use matrices and vectors in YARP.

* log
    A small test for the logging methods in YARP.
