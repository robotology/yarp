fake_laser_obstacle {#master}
-------------------------

## Important Changes

### Devices

#### fakeLaser
* added rpc port to device fakeLaser
* through the rpc is possible to dynamically create obstacles in the map.
* Use the rpc command 'help' to get additional information
* Improved the ray tracing algorithm: laser scans are now bound to the map size
