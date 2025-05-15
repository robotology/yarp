## YARP devices

<br/>

---


This README file shows the current status of the `device` folder (link to the documentation: [Devices](https://www.yarp.it/latest/group__dev__impl.html) ). Colored squares and font weight and style represents the status of each device in the aforementioned folder

<br/>
The tables shown hereunder report all the information needed to understand the following devices list


| Color tag                                                          | Font weight | Font style   | Status            |
|--------------------------------------------------------------------|-------------|--------------|-------------------|
| -                                                                  | Normal      | Normal       | Done              |
| -                                                                  | **Bold**    | Normal       | To do             |
| ![#cc6600](https://via.placeholder.com/15/cc6600/000000?text=+)    | Normal      | *Italic*     | To be deprecated  |
| ![#882200](https://via.placeholder.com/15/882200/000000?text=+)    | Normal      | ~~Striked~~  | Deprecated        |



| Color Tag                                                                 | Type                         |
|---------------------------------------------------------------------------|------------------------------|
| ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+)           | NWC                          |
| ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+)           | NWS                          |
| ![#ff00ff](https://via.placeholder.com/15/ff00ff/000000?text=+)           | Multiplexer                  |
| ![#a9a9a9](https://via.placeholder.com/15/a9a9a9/000000?text=+)           | Library/Protocol             |
| ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+)           | Logic/Virtual/regular device |
<br/>

---

<br/>

### Devices list ordered by folder

<br/>

* AnalogSensorClient
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **analogsensorclient**

* AnalogWrapper
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **analogServer**

* audioFromFileDevice
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) audioFromFileDevice

* audioPlayerWrapper
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **AudioPlayerWrapper**

* audioRecorderWrapper
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+)  **AudioRecorderWrapper**

* audioToFileDevice
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) audioToFileDevice

 * battery_nwc
   * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) battery_nwc_yarp

* battery_nws
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) battery_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) battery_nws_ros2

* ControlBoardRemapper
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) controlboardremapper
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) remotecontrolboardremapper

* controlBoard_nws
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) controlBoard_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) controlBoard_nws_ros
  *  ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) controlBoard_nws_ros2

* DynamixelAX12Ftdi
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) dynamixelAX12Ftdi

* fake devices
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeAnalogSensor
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeBattery
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeDepthCamera
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeFrameGrabber
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeIMU
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeJoypad
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeLaser
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeLaserWithMotor
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeLocalizer
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeMicrophone
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeMotionControl
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeNavigation
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeOdometry2D
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakePositionSensor
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeSerialPort
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeSpeaker
  * ![#882200](https://via.placeholder.com/15/882200/000000?text=+) ~~fakebot~~

* ffmpeg
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) ffmpeg_grabber
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) ffmpeg_writer

* frameGrabberCropper
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) frameGrabberCropper

* ![#a9a9a9](https://via.placeholder.com/15/a9a9a9/000000?text=+) framegrabber_protocol

* frameGrabber_nwc_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) frameGrabber_nwc_yarp

* frameGrabber_nws
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+)  frameGrabber_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) frameGrabber_nws_ros

* frameTransformClient
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) frameTransformClient

* frameTransformGet
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) frameTransformGet_nws_yarp
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) frameTransformGet_nwc_yarp
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) frameTransformGet_nwc_ros
  * ![#ff00ff](https://via.placeholder.com/15/ff00ff/000000?text=+) frameTransformGetMultiplexer

* frameTransformSet
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) frameTransformSet_nws_yarp
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) frameTransformSet_nwc_yarp
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) frameTransformSet_nwc_ros
  * ![#ff00ff](https://via.placeholder.com/15/ff00ff/000000?text=+) frameTransformSetMultiplexer

* frameTransformStorage
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) frameTransformStorage

* ![#a9a9a9](https://via.placeholder.com/15/a9a9a9/000000?text=+) frameTransformStorageMsgs

* imuBosch_BNO055
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) imuBosch_BNO055

* JoypadControlClient
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **JoypadControlClient**

* ![#a9a9a9](https://via.placeholder.com/15/a9a9a9/000000?text=+) JoypadControlNetUtils

* JoypadControlServer
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **JoypadControlServer**

* laserFromDepth
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) laserFromDepth

* laserFromExternalPort
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) laserFromExternalPort

* laserFromPointCloud
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) laserFromPointCloud

* laserFromRosTopic
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **laserFromRosTopic**

* laserHokuyo
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) laserHokuyo

* localization2DClient
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) localization2D_nwc_yarp

* localization2D_nws
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) localization2D_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) localization2D_nws_ros
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) localization2D_nws_ros2

* map2D_nwc_yarp
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) map2D_nwc_yarp

* map2D_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) map2D_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) map2D_nws_ros
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) map2D_nws_ros2

* map2DStorage
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) map2DStorage

* ![#a9a9a9](https://via.placeholder.com/15/a9a9a9/000000?text=+) multipleAnalogSensorsMsgs

* multipleAnalogSensorsRosPublishers
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **IMURosPublisher**
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **WrenchStampedRosPublisher**
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **TemperatureRosPublisher**
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **PoseStampedRosPublisher**
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **MagneticFieldRosPublisher**

* multipleanalogsensorsclient
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **multipleanalogsensorsclient**

* multipleanalogsensorsremapper
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) multipleanalogsensorsremapper

* multipleanalogsensorsserver
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **multipleanalogsensorsserver**

* navigation2D_nwc_yarp
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) navigation2D_nwc_yarp

* navigation2D_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) navigation2D_nws_yarp

* odometry2D_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) odometry2D_nws_yarp

* opencv
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) opencv_grabber

*  openNI2DepthCamera
   * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) openNI2DepthCamera

* portaudio
  * ![#882200](https://via.placeholder.com/15/882200/000000?text=+) ~~portaudio~~

* portaudioPlayer
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) portaudioPlayer

* portaudioRecorder
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) portaudioRecorder

* Rangefinder2DClient
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **Rangefinder2DClient**

* Rangefinder2D_nws
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) rangefinder2D_nws_ros
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) rangefinder2D_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) rangefinder2D_nws_ros2

* RemoteControlBoard
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **remote_controlboard**

* ![#a9a9a9](https://via.placeholder.com/15/a9a9a9/000000?text=+) RGBDRosConversionUtils

* rgbdSensor_nws
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) rgbdSensor_nws_ros
  *   ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) rgbdSensor_nws_ros2
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) rgbdSensor_nws_yarp

 * RGBDSensorClient
   * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **RGBDSensorClient**

* RobotDescription_nwc_yarp
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) robotDescription_nwc_yarp

* RobotDescriptionS_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) robotDescription_nws_yarp

* rpLidar
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) rpLidar

* rpLidar2
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) rpLidar2

* SDLJoypad
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) SDLJoypad

* serialport
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) serialport

* serialPort_nwc_yarp
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) serialPort_nwc_yarp

* serialPort_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) serialPort_nws_yarp

* SerialServoBoard
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) SerialServoBoard

* test_nop
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) test_nop

* test_segfault
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) test_segfault

* transformClient
  * ![#cc6600](https://via.placeholder.com/15/cc6600/000000?text=+) *transformClient*

* transformServer
  * ![#cc6600](https://via.placeholder.com/15/cc6600/000000?text=+) *transformServer*

* upowerBattery
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) upowerBattery

* usbCamera
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) usbCamera
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) usbCameraRaw

* VirtualAnalogWrapper
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) virtualAnalogServer
