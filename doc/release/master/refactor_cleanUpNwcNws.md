refactor_cleanUpNwcNws {#master}
-------------------

### Devices

#### `yarp::dev`

* Refactor and cleanup of all the currently available nws and nws (see [#2441](https://github.com/robotology/yarp/discussions/2441) for clarifications about nws/nws architecture).
* The modified nws/nwc are the following:
  * ControlBoard_nws_ros
  * ControlBoard_nws_yarp
  * rgbdSensor_nws_ros
  * rgbdSensor_nws_yarp
  * RGBDToPointCloudSensor_nws_ros
  * rangefinder2D_nws_ros
  * rangefinder2D_nws_yarp
  * FrameGrabber_nwc_yarp
  * frameGrabber_nws_ros
  * frameGrabber_nws_yarp
  * localization2D_nws_ros
  * localization2D_nws_yarp
  * map2D_nws_ros
  * map2D_nws_yarp
* For clarity sake, the commits of the branch have been divided into "code refactoring" and "file/folder names modifications" for each device. This should help the review process
