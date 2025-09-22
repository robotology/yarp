# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_dev_tests)
add_library(YARP::YARP_dev_tests ALIAS YARP_dev_tests)

set(YARP_dev_test_HDRS
    IDummyTest.h
    IEncodersTimedTest.h
    IFrameGrabberControlsTest.h
    IFrameGrabberControlsDC1394Test.h
    IPositionControlTest.h
    ITorqueControlTest.h
    IAxisInfoTest.h
    IRGBDSensorTest.h
    IBatteryTest.h
    IJoypadControllerTest.h
    ILLMTest.h
    IChatBotTest.h
    IVelocityControlTest.h
    IPositionDirectTest.h
    IVelocityDirectTest.h
    IControlModeTest.h
    IImpedanceControlTest.h
    IInteractionModeTest.h
    ICurrentControlTest.h
    IPWMControlTest.h
    IPidControlTest.h
    IMotorTest.h
    IMotorEncodersTest.h
    IRemoteCalibratorTest.h
    IJointBrakeTest.h
    IJointFaultTest.h
    IControlLimitsTest.h
    Utils.h
)

set(YARP_dev_test_SRCS
    IDummyTest.cpp
    IEncodersTimedTest.cpp
    IFrameGrabberControlsTest.cpp
    IFrameGrabberControlsDC1394Test.cpp
    IPositionControlTest.cpp
    ITorqueControlTest.cpp
    IAxisInfoTest.cpp
    IRGBDSensorTest.cpp
    IBatteryTest.cpp
    IJoypadControllerTest.cpp
    ILLMTest.cpp
    IChatBotTest.cpp
    IVelocityControlTest.cpp
    IPositionDirectTest.cpp
    IVelocityDirectTest.cpp
    IControlModeTest.cpp
    IImpedanceControlTest.cpp
    IInteractionModeTest.cpp
    ICurrentControlTest.cpp
    IPWMControlTest.cpp
    IPidControlTest.cpp
    IMotorTest.cpp
    IMotorEncodersTest.cpp
    IRemoteCalibratorTest.cpp
    IJointBrakeTest.cpp
    IJointFaultTest.cpp
    IControlLimitsTest.cpp
    Utils.cpp
)

if(TARGET YARP::YARP_math)
  list(APPEND YARP_dev_test_HDRS
      IFrameGrabberImageTest.h
      IRgbVisualParamsTest.h
      IFrameTransformTest.h
      IMap2DTest.h
      INavigation2DTest.h
      IDummyTest.h
      IRangefinder2DTest.h
      IPositionControlTest.h
      ITorqueControlTest.h
      IAxisInfoTest.h
      IPositionSensorsTest.h
      IOrientationSensorsTest.h
      IOdometry2DTest.h
      IRGBDSensorTest.h
      ILocalization2DTest.h
      IFrameTransformStorageTest.h
      )
  list(APPEND YARP_dev_test_SRCS
      IFrameGrabberImageTest.cpp
      IRgbVisualParamsTest.cpp
      IFrameTransformTest.cpp
      INavigation2DTest.cpp
      IDummyTest.cpp
      IRangefinder2DTest.cpp
      IPositionControlTest.cpp
      ITorqueControlTest.cpp
      IAxisInfoTest.cpp
      IPositionSensorsTest.cpp
      IOrientationSensorsTest.cpp
      IOdometry2DTest.cpp
      IRGBDSensorTest.cpp
      IMap2DTest.cpp
      ILocalization2DTest.cpp
      IFrameTransformStorageTest.cpp
      )
endif()

target_sources(YARP_dev_tests
  PRIVATE
    ${YARP_dev_test_SRCS}
    ${YARP_dev_test_HDRS}
)

target_compile_definitions(YARP_dev_tests PUBLIC YARP_dev_EXPORTS=1)

target_link_libraries (YARP_dev_tests
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_catch2)

target_compile_features(YARP_dev_tests PRIVATE cxx_std_20)

set_property (
  TARGET YARP_dev_tests
  PROPERTY FOLDER "Test"
)

set_property(
  TARGET YARP_dev_tests
  PROPERTY
    PUBLIC_HEADER
      ${YARP_dev_test_HDRS}
)

set_property(TARGET YARP_dev_tests PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_dev_tests PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_dev_tests PROPERTY FOLDER "Test")

 install(
  TARGETS YARP_dev_tests
  EXPORT YARP_dev_tests
  RUNTIME
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT YARP_dev_tests
  LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT YARP_dev_tests
    NAMELINK_COMPONENT YARP_dev_tests-dev
  ARCHIVE
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT YARP_dev_tests-dev
  PUBLIC_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/dev/tests"
    COMPONENT YARP_dev_tests-dev
)

yarp_install_basic_package_files(YARP_dev_tests
  DEPENDENCIES ${YARP_dev_tests_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_dev_tests_PRIVATE_DEPS}
)
