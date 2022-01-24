
depthimage_to_vector_portmonitor plugin
======================================================================
Portmonitor plugin for visualizing the content of a depth image in a human readable format
(i.e. float numbers instead of bytes)

Usage:
-----

yarp read /test
yarp connect /depthCamera/depthImage:o /test tcp+send.portmonitor+file.depthimage_to_vector+type.dll
