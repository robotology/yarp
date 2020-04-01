featureIPositionSensors {#master}
-----------------------

### Libraries

#### `dev`

* Added new interface `yarp::dev::IPositionSensors`.


### Devices

#### `multipleAnalogSensorsMsgs`

* `multipleAnalogSensorsSerializations.thrift` have been extended to handle
  position sensors.

#### `multipleanalogsensorsserver`

* Added handlers for `IPositionSensors` interface.

#### `multipleanalogsensorsclient`

* Added handlers for `IPositionSensors` interface.

#### `multipleanalogsensorsremapper`

* Added handlers for `IPositionSensors` interface.
