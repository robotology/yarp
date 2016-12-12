
zfp_portmonitor plugin 
======================================================================
Portmonitor plugin for compression and decompression of depth images using zfp library.

Usage:
-----

yarp connect /depthCamera/depthImage:o /view tcp+send.portmonitor+file.zfp+recv.portmonitor+file.zfp+type.dll


