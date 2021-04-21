
depthimage_compression_zlib_portmonitor plugin
======================================================================
Portmonitor plugin for compression and decompression of depth images using zlib library.

Usage:
-----

yarp connect /depthCamera/depthImage:o /view tcp+send.portmonitor+file.depthimage_compression_zlib+recv.portmonitor+file.depthimage_compression_zlib+type.dll
