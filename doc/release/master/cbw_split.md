cbw_split {#master}
---------

### Devices

* Added `controlBoard_nws_yarp` and `controlBoard_nws_ros` devices.
  These devices, together with `controlboardremapper` can be used to replace
  `controlboardwrapper2`.
  Important differences with `controlboardwrapper2`:
  * These devices do not handle networks, use `controlboardremapper` for that.
  * All deprecated parameters were removed
  * `controlBoard_nws_ros` requires that the `yarp::dev::IAxisInfo` is
    implemented in the device
  * Periods are in seconds, and not in milliseconds.
