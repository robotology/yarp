# use PARENT_SCOPE to make the following variables available at the 'parent' cmake

set(RGBDClient_srcs      src/modules/RGBDSensorClient/RGBDSensorClient.cpp
#                           src/modules/DepthSensorClient/RPCMessagesParser.cpp
                           src/modules/RGBDSensorClient/RGBDSensorClient_StreamingMsgParser.cpp
                          PARENT_SCOPE)

set(RGBDClient_hrds      src/modules/RGBDSensorClient/RGBDSensorClient.h
#                           src/modules/DepthSensorClient/RPCMessagesParser.h
                           src/modules/RGBDSensorClient/RGBDSensorClient_StreamingMsgParser.h
#                           src/modules/DepthSensorClient/yarpRosHelper.h
                          PARENT_SCOPE)


set(RGBDClient_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
