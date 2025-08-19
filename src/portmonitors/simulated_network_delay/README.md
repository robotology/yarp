
simulated_network_delay_portmonitor plugin
======================================================================
This portmonitor adds a user defined delay (in milliseconds) to the connection

Usage:
-----
yarp connect /sender/data:o /receiver/data:i tcp+recv.portmonitor+file.simulated_network_delay+delay_ms.10+type.dll
