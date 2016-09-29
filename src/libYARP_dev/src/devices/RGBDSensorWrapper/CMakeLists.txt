# use PARENT_SCOPE to make the following variables available at the 'parent' cmake

set(RGBDWrap_srcs      src/devices/RGBDSensorWrapper/RGBDSensorWrapper.cpp
                        src/devices/RGBDSensorWrapper/RGBDSensorWrapper_RPCMsgParser.cpp
#                         src/devices/RGBDSensorWrapper/StreamingMessagesParser.cpp
                        PARENT_SCOPE)

set(RGBDWrap_hrds      src/devices/RGBDSensorWrapper/RGBDSensorWrapper.h
                        src/devices/RGBDSensorWrapper/RGBDSensorWrapper_RPCMsgParser.h
#                         src/devices/RGBDSensorWrapper/StreamingMessagesParser.h
#                         src/devices/RGBDSensorWrapper/yarpRosHelper.h
                        PARENT_SCOPE)


set(RGBDWrap_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
