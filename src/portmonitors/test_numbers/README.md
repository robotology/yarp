
test_numbers_portmonitor plugin
======================================================================
These portmonitors are used for testing purposes

Usage:
-----

This adds 1 to any integer number received on the connection
yarp connect /sender /receiver fast_tcp+send.portmonitor+type.dll+file.test_numbers1+type.dll

This multiplies by 2 every integer number received on the connection
yarp connect /sender /receiver fast_tcp+send.portmonitor+type.dll+file.test_numbers2+type.dll
