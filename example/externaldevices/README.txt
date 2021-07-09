# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

This is an example to show how you can write external devices.

Here you find a directory structure with two bundles of devices
(foo and baz) which contains fooBot, fooDevice, bazBot, bazDevice;
these are just examples.

Run cmake, and then the cmake GUI (ccmake on Unix, CMakeSetup.exe on
Windows).  You should see a list of the available devices: bazDevice,
bazBot, fooDevice and fooBot. Turn them on and finish the cmake
process to generate your project/make files.  Then compile, and you
should get a "foobazdev" program that works just like the "yarpdev"
program.  In fact, if you do:
  ./foobazdev --list
you should see all the devices in yarpdev plus the new ones you have
just added.

If you want these devices to show up in yarpdev itself,
you can create the file in $YARP_ROOT/cmake/ExternalModules.cmake
following the template in  $YARP_ROOT/cmake/templates/ExternalModules.cmake.template,
or else add subdirectories in $YARP_ROOT/src/modules (edit the
CMakeLists.txt to enumerate those subdirectories).
