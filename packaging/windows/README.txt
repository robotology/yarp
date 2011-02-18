Steps for generating binary packages.

For personal work, it suffices just to do "make package" in YARP.
You don't need anything in here.

===

Preparation:
 * Install cygwin
 *   You need: bash, wget, tar, grep, sed
 * Install Visual Studio compilers, as many as you can
 * Update conf/compilers.sh to describe your compilers

How to use:

 * Create a build directory, say "build"
 * Do ../manage.sh [or whatever the path to manage.sh is]
 * Select one of the "bundle" names listed, e.g. bundle-000
 * Do ../manage.sh bundle-000
 
 