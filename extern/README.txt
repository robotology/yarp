External libraries bundled with YARP go here.  Ideally, to make life
easier for packagers, there should always be a CMake option to use
system-installed versions of these libraries.

Suggested layout is a little verbose:
  extern/libraryname/README.txt
    Give information about the source of the library, include in 
    particular its exact version number (and separately any
    known constraints on the acceptable version number).
    As parts of YARP are added that depend on a library, please
    note that here.
  extern/libraryname/libraryname (or similar subdirectory)
    the actual library, ideally untouched (although files may be 
    omitted).
  extern/libraryname/CMakelists.txt
    CMake script to build library.
  extern/libraryname/patches (optional)
    Patches applied to upstream source,
  extern/libraryname/LICENSES
    Licenses relative to the source code in the subdirectory

The extra subdirectory is to give space for wrapper code/scripts
without mixing it with the original code.

