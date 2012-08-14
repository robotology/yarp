# Copyright: (C) 2011 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

Steps for generating binary packages.

For personal work, it suffices just to do "make package" in YARP.
You don't need anything in here.

===

Preparation:
 * Be on a debian/ubuntu machine.
 * Install debootstrap
 * Copy conf/compilers.sh to conf/compilers_local.sh and list all
   platforms you want to compile on.  Name needs to match that
   used by debootstrap ("ls /usr/share/debootstrap/scripts/")

How to use:
 * Find a checkout of YARP.
 * Go into the packaging/linux directory.
 * Create a build directory, say "build"
 * Do ../manage.sh [or whatever the path to manage.sh is]
 * Select one of the "bundle" names listed, e.g. bundle-2-3-4
 * Do ../manage.sh bundle-2-3-4
 * You should now have a makefile chock-full of targets.
   Read the makefile to see how targets are built.
 * If life is good, you may just need to do "make all"
