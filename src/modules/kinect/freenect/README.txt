
# Copyright (C) 2010 RobotCub Consortium
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


Compilation tips.  Note, freenect is changing rapidly, so you may
need to make fixes on any given day.

 sudo apt-get install libusb-1.0-0-dev
 git clone https://github.com/OpenKinect/libfreenect.git
 cd libfreenect/
 mkdir build
 cd build
 cmake ..
 make

 Now go to YARP, turn on device module compilation, turn on kinect and
 primesensecamera, and recompile.

