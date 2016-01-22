# use PARENT_SCOPE to make the following variables available at the 'parent' cmake

set(RGBDWrap_srcs      src/modules/RGBDSensorWrapper/RGBDSensorWrapper.cpp
                        src/modules/RGBDSensorWrapper/RGBDSensor_RPCMsgParser.cpp
#                         src/modules/RGBDSensorWrapper/StreamingMessagesParser.cpp
                        PARENT_SCOPE)

set(RGBDWrap_hrds      src/modules/RGBDSensorWrapper/RGBDSensorWrapper.h
                        src/modules/RGBDSensorWrapper/RGBDSensor_RPCMsgParser.h
#                         src/modules/RGBDSensorWrapper/StreamingMessagesParser.h
#                         src/modules/RGBDSensorWrapper/yarpRosHelper.h
                        PARENT_SCOPE)


set(RGBDWrap_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
