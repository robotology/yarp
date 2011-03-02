Steps for generating binary packages.

For personal work, it suffices just to do "make package" in YARP.
You don't need anything in here.

===

Preparation:
 * Install cygwin
 *   You need: bash, wget, tar, grep, sed, subversion, find, gmake
 * Install Visual Studio compilers, as many as you can
 * Copy conf/compilers.sh to conf/compilers_local.sh and modify
   it to describe your compilers

How to use:

 * Open a cygwin shell, and find a checkout of YARP.
 * Go into the packaging/windows directory.
 * Create a build directory, say "build"
 * Do ../manage.sh [or whatever the path to manage.sh is]
 * Select one of the "bundle" names listed, e.g. bundle-yarp-trunk
 * Do ../manage.sh bundle-yarp-trunk
 * You should now have a makefile chock-full of targets.
   Read the makefile to see how targets are built.
 * If life is good, you may just need to do "make yarp.txt"
 * If you run into trouble, don't get hung up with the 
   automation.  Everything is built with CMake, so you can
   always search for the relevant *.sln file and open it
   to build manually.
 