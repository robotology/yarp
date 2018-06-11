
# Copyright: (C) 2010 Renaud Detry
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE

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

