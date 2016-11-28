# use PARENT_SCOPE to make the following variables available at the 'parent' cmake

set(cbw2_core_srcs      src/devices/ControlBoardWrapper/ControlBoardWrapper.cpp
                        src/devices/ControlBoardWrapper/RPCMessagesParser.cpp
                        src/devices/ControlBoardWrapper/StreamingMessagesParser.cpp
                        src/devices/ControlBoardWrapper/SubDevice.cpp
                        PARENT_SCOPE)

set(cbw2_core_hrds      src/devices/ControlBoardWrapper/ControlBoardWrapper.h
                        src/devices/ControlBoardWrapper/RPCMessagesParser.h
                        src/devices/ControlBoardWrapper/StreamingMessagesParser.h
                        src/devices/ControlBoardWrapper/SubDevice.h
                        src/devices/ControlBoardWrapper/yarpRosHelper.h
                        PARENT_SCOPE)


set(cbw2_core_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
