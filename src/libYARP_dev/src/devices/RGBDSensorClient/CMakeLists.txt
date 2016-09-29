# use PARENT_SCOPE to make the following variables available at the 'parent' cmake

set(RGBDClient_srcs      src/devices/RGBDSensorClient/RGBDSensorClient.cpp
#                           src/devices/DepthSensorClient/RPCMessagesParser.cpp
                           src/devices/RGBDSensorClient/RGBDSensorClient_StreamingMsgParser.cpp
                          PARENT_SCOPE)

set(RGBDClient_hrds      src/devices/RGBDSensorClient/RGBDSensorClient.h
#                           src/devices/DepthSensorClient/RPCMessagesParser.h
                           src/devices/RGBDSensorClient/RGBDSensorClient_StreamingMsgParser.h
#                           src/devices/DepthSensorClient/yarpRosHelper.h
                          PARENT_SCOPE)


set(RGBDClient_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
