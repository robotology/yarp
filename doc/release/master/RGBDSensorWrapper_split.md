RGBDSensorWrapper_split {#master}
-------------------------

## Refactor

### Devices

#### `RGBDSensorWrapper`

* The wrapper has been divided in two in order to separate the YARP part from the ROS one. This should make the wrapper easier to extend (for example to add ROS2 compatibility).
* The original RGBDSensorWrapper was not modified (for backward compatibility)