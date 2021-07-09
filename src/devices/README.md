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

* ControlBoardRemapper
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) controlboardremapper
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) remotecontrolboardremapper

* ControlBoardWrapper
  * ![#cc6600](https://via.placeholder.com/15/cc6600/000000?text=+) *controlboardwrapper2*
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) controlBoard_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) controlBoard_nws_ros

* DeviceGroup
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) group

* DevicePipe
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) pipe

* DynamixelAX12Ftdi
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) dynamixelAX12Ftdi

* JoypadControlClient
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **JoypadControlClient**

* ![#a9a9a9](https://via.placeholder.com/15/a9a9a9/000000?text=+) JoypadControlNetUtils

* JoypadControlServer
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **JoypadControlServer**

* ![#a9a9a9](https://via.placeholder.com/15/a9a9a9/000000?text=+) RGBDRosConversionUtils

* RGBDSensorClient
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **RGBDSensorClient**

* RGBDSensorFromRosTopic
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **RGBDSensorFromRosTopic**

* RGBDSensorWrapper
  * ![#cc6600](https://via.placeholder.com/15/cc6600/000000?text=+) *RGBDSensorWrapper*
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) rgbdSensor_nws_ros
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) rgbdSensor_nws_yarp

* RGBDToPointCloudSensorWrapper
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) RGBDToPointCloudSensor_nws_ros

* Rangefinder2DClient
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **Rangefinder2DClient**

* Rangefinder2DWrapper
  * ![#cc6600](https://via.placeholder.com/15/cc6600/000000?text=+) *Rangefinder2DWrapper*
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) rangefinder2D_nws_ros
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) rangefinder2D_nws_yarp

* RemoteControlBoard
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **remote_controlboard**

* RemoteFrameGrabber
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) frameGrabber_nwc_yarp

* RobotDescriptionClient
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **robotDescriptionClient**

* RobotDescriptionServer
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **robotDescriptionServer**

* SDLJoypad
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) SDLJoypad

* SerialServoBoard
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) SerialServoBoard

* ServerFrameGrabber
  * ![#882200](https://via.placeholder.com/15/882200/000000?text=+) ~~grabber~~

* ServerFrameGrabberDual
  * ![#cc6600](https://via.placeholder.com/15/cc6600/000000?text=+) *grabberDual*
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) frameGrabber_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) frameGrabber_nws_ros

* ServerInertial
  * ![#cc6600](https://via.placeholder.com/15/cc6600/000000?text=+) *inertial*

* ServerSerial
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) serial

* ServerSoundGrabber
  * ![#882200](https://via.placeholder.com/15/882200/000000?text=+) ~~ServerSoundGrabber~~

* VirtualAnalogWrapper
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) virtualAnalogServer

* audioFromFileDevice
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) audioFromFileDevice

* audioPlayerWrapper
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **AudioPlayerWrapper**

* audioRecorderWrapper
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **AudioRecorderWrapper**

* audioToFileDevice
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) audioToFileDevice

* batteryClient
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **batteryClient**

* batteryWrapper
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **batteryWrapper**

* depthCamera
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) DepthCamera
    * *To do: Rename OpenNI*Something*

* fakeAnalogSensor
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeAnalogSensor

* fakeBattery
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeBattery

* fakeDepthCamera
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeDepthCamera

* fakeFrameGrabber
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeFrameGrabber
  * ![#cc6600](https://via.placeholder.com/15/cc6600/000000?text=+) *test_grabber*

* fakeIMU
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeIMU

* fakeLaser
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeLaser

* fakeLocalizerDevice
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeLocalizer

* fakeMicrophone
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeMicrophone

* fakeMotionControl
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeMotionControl

* fakeMotor
  * ![#cc6600](https://via.placeholder.com/15/cc6600/000000?text=+) *FakeMotor*
  * ![#cc6600](https://via.placeholder.com/15/cc6600/000000?text=+) *test_motor*

* fakeNavigationDevice
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeNavigation

* fakeSpeaker
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) fakeSpeaker

* fakebot
  * ![#882200](https://via.placeholder.com/15/882200/000000?text=+) ~~fakebot~~

* ffmpeg
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) ffmpeg_grabber
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) ffmpeg_writer

* frameGrabberCropper
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) frameGrabberCropper

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

* frameTransformUtils
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) frameTransformContainer

* ![#a9a9a9](https://via.placeholder.com/15/a9a9a9/000000?text=+) framegrabber_protocol

* imuBosch_BNO055
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) imuBosch_BNO055

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
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **localization2DClient**

* localization2DServer
  * ![#cc6600](https://via.placeholder.com/15/cc6600/000000?text=+) *localization2DServer*
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) localization2D_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) localization2D_nws_ros

* map2DClient
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **map2DClient**

* map2DServer
  * ![#cc6600](https://via.placeholder.com/15/cc6600/000000?text=+) *map2DServer*
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) map2D_nws_yarp
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) map2D_nws_ros

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

* navigation2DClient
  * ![#00ff00](https://via.placeholder.com/15/00ff00/000000?text=+) **navigation2DClient**
    * *To do: PathPlanner2DClient + navigation2DClient*

* navigation2DServer
  * ![#00ffff](https://via.placeholder.com/15/00ffff/000000?text=+) **navigation2DServer**
    * *To do: PathPlanner2DServer*

* opencv
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) opencv_grabber

* portaudio
  * ![#882200](https://via.placeholder.com/15/882200/000000?text=+) ~~portaudio~~

* portaudioPlayer
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) portaudioPlayer

* portaudioRecorder
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) portaudioRecorder

* rpLidar
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) rpLidar

* rpLidar2
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) rpLidar2

* serialport
  * ![#ffff00](https://via.placeholder.com/15/ffff00/000000?text=+) serialport

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
