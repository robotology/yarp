streaming_MobileBaseVelocityControl_nws_yarp {#master}
-----------

Important Changes
-----------------

### Devices

#### `MobileBaseVelocityControl_nws_yarp`
* Added input streaming port as an alternative to rpc command `applyVelocityCommandRPC()`
* Added `subdevice` option.

#### `MobileBaseVelocityControl_nws_ros`
* Removed periodic thread in favor of callback.
* Added `subdevice` option.
