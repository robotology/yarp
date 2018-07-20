# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2010 Renaud Detry
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Written by: Renaud Detry
	http://renaud.webhop.net/

yayarpview lets you display in a WIN32/X11 window a
video stream from a YARP port.

You should ccmake this project with the "release" build type to be
sure to optimize the display routines.

CImg.h is needed. It is available on the CVS at
iCub/src/vvv/CImg/CImg.h. Simply copy it somewhere in your include path.

The display should natively work under win32. For this, you will
however have to modify the CMakeList.txt. See
http://cimg.sourceforge.net/reference/group__cimg__structure.html for
CImg win32 compile procedures.


Offline viewing:

[Term 1] ./yayarpview --name /USER/viewer --dump sequence%05d.ppm
[Term 2] yarp connect /james/cam/left /USER/viewer

and let it run for about 20 sec.
You should then have a bunch of sequenceXXXXX.ppm in the current dir.

After that:

[Term 1] ./yayarpview --name /USER/viewer
[Term 2] ./yayarpview --name /USER/broadcaster \
                --behave broadcast --dump sequence%05d.ppm
[Term 3] yarp connect /USER/broadcaster /USER/viewer

alternatively, Term 1 can be
[Term 1] yarpview --name /USER/viewer

