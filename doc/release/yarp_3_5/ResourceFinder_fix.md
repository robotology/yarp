ResourceFinder_fix {#yarp_3_5}
-----------

Important Changes
-----------------

### Libraries

##### `ResourceFinder`

* Use `mkdir_p` instead of `mkdir` when calling `getHomeContextPath` and `getHomeRobotPath`
  to correctly create the missing paths. 