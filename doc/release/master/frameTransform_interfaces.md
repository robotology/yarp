frameTransform_interfaces {#master}
-----------------------

### Devices

* old devices with frame transform management functionality are called `transformServer` and `transformClient`. These devices should be now
  considered deprecated and they must be not confused with the new devices included in this PR.
* a yarp page documentation has been added to show the different possible configurations.

#### frameTransformClient

* Added new device `frameTransformClient`. This device is now a empty container for multiple plugins that can be combined together through a 
  yarprobotinterface xml file. Different configuration files can be used for different purposes, such as publishing/subscribing transforms
  on YARP, ROS, ROS2 etc. Please refer to the documentation for additional details. The new plugins components which can be inserted in a
  frameTransformClient device are:
  * `frameTransformGet` (nwc components for yarp, ros+ nws component for yarp + multiplexer)
  * `frameTransformSet` (nwc components for yarp, ros+ nws component for yarp + multiplexer)
  * `frameTransformStorage`
* Some functionalities are provided by the support classes:
  * `frameTransformStorageMsgs`
  * `frameTransformUtils`




