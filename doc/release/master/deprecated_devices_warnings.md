deprecated_devices_warning {#master}
--------------------------

* The following devices are now deprecated:
  * `test_grabber`
  * `fakeMotor`
  * `test_motor`
  * `fakebot`

* The following devices will be replaced by NWS/NWC in the next release, and
  print a warning when opened:
  * `controlboardwrapper2` (replaced by `controlboardremapper` + `controlBoard_nws_yarp`)
  * `RGBDSensorWrapper` (replaced by `rgbdSensor_nws_yarp`)
  * `Rangefinder2DWrapper` (replaced by `rangefinder2D_nws_yarp`)
  * `grabberDual` (replaced by `frameGrabber_nws_yarp`, and eventually `frameGrabberCropper`)
  * `inertial` (replaced by `multipleanalogsensorsremapper` + `multipleanalogsensorsserver` + `IMURosPublisher`)
  * `localization2DServer` (replaced by `localization2D_nws_yarp`)
  * `map2DServer` (replaced by `map2D_nws_yarp`)
  * `transformClient` (replaced by `frameTransformClient`)
  * `transformServer` (replaced by `frameTransformServer`)
