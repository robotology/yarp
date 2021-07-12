# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

nameclient.c is a very basic UNIX example of how to talk to the
yarp name server without using yarp itself.  For brevity, it omits
some important steps in making tcp connections dependable.

There are better examples in the "c" subdirectory.  For python,
see the "python subdirectory.

tcl/control-motor.tcl controls a YARP motor controlboard from TCL.
Code written by someone who doesn't really know TCL very well,
so beware.  Assumes that you have run:
  yarpdev --device fakeMotionControl --name /controlboard --GENERAL::Joints 4
