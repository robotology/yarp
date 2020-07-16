fix_bug_checkNearToLocation {#yarp_3_3}
----------------------

### devices

#### Navigation2DClient
* The method `checkNearToLocation` now correctly checks if two orientations are similar (below a certain threshold),
also considering the critical points 0,180,360,-180,-180 etc.

### libYARP_dev

#### Navigation2DClientTest
* New tests have been added to check method `checkNearToLocation` in different cases.

