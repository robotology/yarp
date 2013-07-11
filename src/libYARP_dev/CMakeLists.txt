# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(YARP_dev)

set(YARP_dev_HDRS include/yarp/dev/all.h
                  include/yarp/dev/api.h
                  include/yarp/dev/AudioGrabberInterfaces.h
                  include/yarp/dev/AudioVisualInterfaces.h
                  include/yarp/dev/CalibratorInterfaces.h
                  include/yarp/dev/CanBusInterface.h
                  include/yarp/dev/CartesianControl.h
                  include/yarp/dev/ControlBoardHelper.h
                  include/yarp/dev/ControlBoardHelpers.h
                  include/yarp/dev/ControlBoardInterfaces.h
                  include/yarp/dev/ControlBoardInterfacesImpl.h
                  include/yarp/dev/ControlBoardPid.h
                  include/yarp/dev/DataSource.h
                  include/yarp/dev/DeviceDriver.h
                  include/yarp/dev/DeviceGroup.h
                  include/yarp/dev/DevicePipe.h
                  include/yarp/dev/DriverLinkCreator.h
                  include/yarp/dev/Drivers.h
                  include/yarp/dev/FrameGrabberInterfaces.h
                  include/yarp/dev/GazeControl.h
                  include/yarp/dev/GenericSensorInterfaces.h
                  include/yarp/dev/GPUInterface.h
                  include/yarp/dev/IAnalogSensor.h
                  include/yarp/dev/IControlLimits2.h
                  include/yarp/dev/IControlLimits2Impl.h
                  include/yarp/dev/IControlMode.h
                  include/yarp/dev/IEncoders.h
                  include/yarp/dev/IEncodersTimed.h
                  include/yarp/dev/IImpedanceControl.h
                  include/yarp/dev/IKinectDeviceDriver.h
                  include/yarp/dev/ImplementControlBoardInterfaces.h
                  include/yarp/dev/ImplementControlMode.h
                  include/yarp/dev/ImplementEncodersTimed.h
                  include/yarp/dev/ImplementImpedanceControl.h
                  include/yarp/dev/ImplementOpenLoopControl.h
                  include/yarp/dev/ImplementTorqueControl.h
                  include/yarp/dev/IOpenLoopControl.h
                  include/yarp/dev/IPositionControl2.h
                  include/yarp/dev/IPositionControl2Impl.h
                  include/yarp/dev/IPositionControl.h
                  include/yarp/dev/IPositionDirect.h
                  include/yarp/dev/IPositionDirectImpl.h
                  include/yarp/dev/ITorqueControl.h
                  include/yarp/dev/IVelocityControl2.h
                  include/yarp/dev/IVelocityControl2Impl.h
                  include/yarp/dev/IVelocityControl.h
                  include/yarp/dev/IVirtualAnalogSensor.h
                  include/yarp/dev/PolyDriverDescriptor.h
                  include/yarp/dev/PolyDriver.h
                  include/yarp/dev/PolyDriverList.h
                  include/yarp/dev/PreciselyTimed.h
                  include/yarp/dev/RemoteFrameGrabberDC1394.h
                  include/yarp/dev/RemoteFrameGrabber.h
                  include/yarp/dev/SerialInterfaces.h
                  include/yarp/dev/ServerFrameGrabber.h
                  include/yarp/dev/ServerInertial.h
                  include/yarp/dev/ServerSerial.h
                  include/yarp/dev/ServerSoundGrabber.h
                  include/yarp/dev/ServiceInterfaces.h
                  include/yarp/dev/TestFrameGrabber.h
                  include/yarp/dev/TestMotor.h
                  include/yarp/dev/Wrapper.h
                  include/yarp/dev/ControlBoardInterfacesImpl.inl)

set(YARP_dev_SRCS src/AnalogSensorClient.cpp
                  src/ControlBoardInterfacesImpl.cpp
                  src/ControlBoardPid.cpp
                  src/ControlCalibrationImpl.cpp
                  src/ControlModeImpl.cpp
                  src/DeviceDriver.cpp
                  src/DeviceGroup.cpp
                  src/DevicePipe.cpp
                  src/Drivers.cpp
                  src/Dummy.cpp
                  src/IControlLimit2sImpl.cpp
                  src/IEncodersTimedImpl.cpp
                  src/ImpedanceControlImpl.cpp
                  src/IPositionControl2Impl.cpp
                  src/IPositionDirectImpl.cpp
                  src/IVelocityControl2Impl.cpp
                  src/OpenLoopControlImpl.cpp
                  src/PolyDriver.cpp
                  src/PolyDriverDescriptor.cpp
                  src/PolyDriverList.cpp
                  src/PopulateDrivers.cpp
                  src/RemoteControlBoard.cpp
                  src/RemoteFrameGrabber.cpp
                  src/RemoteFrameGrabberDC1394.cpp
                  src/ServerControlBoard.cpp
                  src/ServerFrameGrabber.cpp
                  src/TestFrameGrabber.cpp
                  src/TorqueControlImpl.cpp)

source_group("Source Files" FILES ${YARP_init_SRCS})
source_group("Header Files" FILES ${YARP_init_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(${ACE_INCLUDE_DIRS})

add_library(YARP_dev ${YARP_dev_SRCS} ${YARP_dev_HDRS})
target_link_libraries(YARP_dev LINK_PRIVATE YARP_OS)
target_link_libraries(YARP_dev LINK_PRIVATE YARP_sig)
target_link_libraries(YARP_dev LINK_PRIVATE ${ACE_LIBRARIES})
install(TARGETS YARP_dev EXPORT YARP COMPONENT runtime DESTINATION lib${LIB_SUFFIX})
set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_dev)

set_property(TARGET YARP_dev PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})

install(FILES ${YARP_dev_HDRS}
        COMPONENT development
        DESTINATION include/yarp/dev)
