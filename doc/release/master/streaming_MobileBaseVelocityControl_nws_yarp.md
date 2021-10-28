streaming_MobileBaseVelocityControl_nws_yarp {#master}
-----------

Important Changes
-----------------

### Devices

#### `MobileBaseVelocityControl_nws_yarp`
* Added input streaming port as an alternative to rpc command `applyVelocityCommandRPC()`
* Added `subdevice` option.
* Added the possibility to view the device through the interface `yarp::dev::Nav2D::INavigation2DVelocityActions`