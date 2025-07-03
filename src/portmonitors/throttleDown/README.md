
ThrottleDown plugin
======================================================================
Portmonitor plugin for reducing the throughput of a port connection, decimating the transmitted messages.
For example, this portmonitor can be used to reduce a port streamed data from 10 to 1 message per second,
reducing the overall bandwidth usage. Please note that this portmonitor can be attached to both the sender and the receiver port,
however bandwidth consumption will be reduced only if the portmonitor is attached to the sender port.

Usage:
-----

yarp write /test --period 0.01
yarp read ... /in
yarp connect  /test /in tcp+send.portmonitor+type.dll+file.throttleDown+period_ms.500
