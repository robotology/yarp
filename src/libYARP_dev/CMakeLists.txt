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
                  include/yarp/dev/IControlMode2.h
                  include/yarp/dev/IEncoders.h
                  include/yarp/dev/IEncodersTimed.h
                  include/yarp/dev/IMotorEncoders.h
                  include/yarp/dev/IMotor.h
                  include/yarp/dev/IImpedanceControl.h
                  include/yarp/dev/IKinectDeviceDriver.h
                  include/yarp/dev/ImplementControlBoardInterfaces.h
                  include/yarp/dev/ImplementControlMode.h
                  include/yarp/dev/ImplementControlMode2.h
                  include/yarp/dev/ImplementEncodersTimed.h
                  include/yarp/dev/ImplementMotorEncoders.h
                  include/yarp/dev/ImplementMotor.h
                  include/yarp/dev/ImplementImpedanceControl.h
                  include/yarp/dev/ImplementOpenLoopControl.h
                  include/yarp/dev/ImplementTorqueControl.h
                  include/yarp/dev/IInteractionMode.h
                  include/yarp/dev/IInteractionModeImpl.h
                  include/yarp/dev/IOpenLoopControl.h
                  include/yarp/dev/IOpenNI2DeviceDriver.h
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
                  include/yarp/dev/ServerSerial.h
                  include/yarp/dev/ServerSoundGrabber.h
                  include/yarp/dev/ServiceInterfaces.h
                  include/yarp/dev/TestFrameGrabber.h
                  include/yarp/dev/TestMotor.h
                  include/yarp/dev/Wrapper.h
                  include/yarp/dev/ControlBoardInterfacesImpl.inl)

set(YARP_dev_IMPL_HDRS )

set(YARP_dev_SRCS src/ControlBoardInterfacesImpl.cpp
                  src/ControlBoardPid.cpp
                  src/ControlCalibrationImpl.cpp
                  src/RemoteCalibratorImpl.cpp
                  src/ControlModeImpl.cpp
                  src/ControlMode2Impl.cpp
                  src/DeviceDriver.cpp
                  src/DeviceGroup.cpp
                  src/DevicePipe.cpp
                  src/Drivers.cpp
                  src/Dummy.cpp
                  src/IControlLimit2sImpl.cpp
                  src/IEncodersTimedImpl.cpp
                  src/IMotorEncodersImpl.cpp
                  src/IMotorImpl.cpp
                  src/ImpedanceControlImpl.cpp
                  src/IInteractionModeImpl.cpp
                  src/IPositionControl2Impl.cpp
                  src/IPositionDirectImpl.cpp
                  src/IVelocityControl2Impl.cpp
                  src/OpenLoopControlImpl.cpp
                  src/PolyDriver.cpp
                  src/PolyDriverDescriptor.cpp
                  src/PolyDriverList.cpp
                  src/PopulateDrivers.cpp
                  src/RemoteFrameGrabber.cpp
                  src/RemoteFrameGrabberDC1394.cpp
                  src/ServerControlBoard.cpp
                  src/ServerFrameGrabber.cpp
                  src/ServerSerial.cpp
                  src/TestFrameGrabber.cpp
                  src/TorqueControlImpl.cpp)

set(YARP_devices_SRCS   src/modules/AnalogWrapper/AnalogWrapper.cpp
                        src/modules/AnalogSensorClient/AnalogSensorClient.cpp
                        src/modules/ControlBoardWrapper/ControlBoardWrapper.cpp
                        src/modules/VirtualAnalogWrapper/VirtualAnalogWrapper.cpp)

set(YARP_devices_HDRS   src/modules/AnalogWrapper/AnalogWrapper.h
                        src/modules/AnalogSensorClient/AnalogSensorClient.h
                        src/modules/ControlBoardWrapper/ControlBoardWrapper.h
                        src/modules/VirtualAnalogWrapper/VirtualAnalogWrapper.h)
                        
# handle the ros / yarp thrift messages
# the variables cbw2_msgs_srcs, cbw2_msgs_hrds and cbw2_msgs_incl_dirs will be set inside
add_subdirectory(src/modules/msgs)


# handle the source file for the controlboardwrapper2 device
# the variable cbw2_core_srcs, cbw2_core_hrds, cbw2_core_incl_dirs will be set inside
add_subdirectory(src/modules/ControlBoardWrapper)

# handle the source file for the RemoteControlBoard device
# the variables rmc_core_srcs and rmc_core_hrds and rmc_core_incl_dirs will be set inside
add_subdirectory(src/modules/RemoteControlBoard)

# add controlboardwrapper2 and RemoteControlBoard devices
set( YARP_devices_SRCS ${YARP_devices_SRCS} ${cbw2_core_srcs} ${rmc_core_srcs} ${cbw2_msgs_srcs})
set( YARP_devices_HDRS ${YARP_devices_HDRS} ${cbw2_core_hrds} ${rmc_core_hrds} ${cbw2_msgs_hrds})

# add ServerInertial device
add_subdirectory(src/modules/ServerInertial)
set( YARP_devices_SRCS ${YARP_devices_SRCS} ${serverInertial_srcs} ${inertial_msgs_srcs})
set( YARP_devices_HDRS ${YARP_devices_HDRS} ${serverInertial_hdrs} ${inertial_msgs_hdrs})

#message(INFO "\n other_files is ${other_files}")
include_directories(${cbw2_core_incl_dirs} ${rmc_core_incl_dirs} ${cbw2_msgs_incl_dirs} ${serverInertial_incl_dirs})
#message(STATUS "CMAKE_CURRENT_BINARY_DIR is ${CMAKE_CURRENT_BINARY_DIR} ||")
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src/modules/ControlBoardWrapper/jointstate )

source_group("Source Files" FILES ${YARP_dev_SRCS})
source_group("Header Files" FILES ${YARP_dev_HDRS})
source_group("Implementation Header Files" FILES ${YARP_dev_IMPL_HDRS})
source_group("Devices Files" FILES ${YARP_devices_SRCS}
                                   ${YARP_devices_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/modules/AnalogSensorClient)
include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(SYSTEM ${ACE_INCLUDE_DIRS})

add_library(YARP_dev ${YARP_dev_SRCS} ${YARP_devices_SRCS} ${YARP_dev_HDRS})
#add_dependencies(YARP_dev yarpidl_rosmsg yarpidl_thrift )

target_link_libraries(YARP_dev LINK_PRIVATE YARP_OS)
target_link_libraries(YARP_dev LINK_PRIVATE YARP_sig)

set_property(TARGET YARP_dev PROPERTY PUBLIC_HEADER ${YARP_dev_HDRS})
set_property(TARGET YARP_dev PROPERTY PRIVATE_HEADER ${YARP_dev_IMPL_HDRS})

if(YARP_HAS_ACE)
    target_link_libraries(YARP_dev LINK_PRIVATE ${ACE_LIBRARIES})
endif(YARP_HAS_ACE)

install(TARGETS YARP_dev
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/dev"
        PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/dev/impl")

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_dev)
set_property(TARGET YARP_dev PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
