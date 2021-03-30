fix_race_condition_yarp_scan_plugin {#yarp_3_4}
--------------

## libraries

## yarp_os

* Fixed error in race condition during the scan of plugins in YarpPluginSelector, avoided concurrent access to variables.
