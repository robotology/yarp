
zfp_portmonitor plugin
======================================================================
Portmonitor plugin for compression and decompression of depth images using zfp library.

Compilation and installation:
Please download the 'ZFP' library version 0.5.1 **exactly** from 'http://computation.llnl.gov/projects/floating-point-compression'.
Use the CMake build system to build the library and install it.
Set the environment variable 'ZFP_ROOT' to the installation folder.
Note: the ZFP_ROOT has to point to the install directory, not to the build directory.

Usage:
-----

yarp connect /depthCamera/depthImage:o /view tcp+send.portmonitor+file.depthimage_compression_zfp+recv.portmonitor+file.depthimage_compression_zfp+type.dll
