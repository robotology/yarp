# use PARENT_SCOPE to make the following variables available at the 'parent' cmake

set(cbw2_core_srcs      src/modules/ControlBoardWrapper/ControlBoardWrapper.cpp
                        src/modules/ControlBoardWrapper/RPCMessagesParser.cpp
                        src/modules/ControlBoardWrapper/StreamingMessagesParser.cpp
                        src/modules/ControlBoardWrapper/SubDevice.cpp
                        PARENT_SCOPE)

set(cbw2_core_hrds      src/modules/ControlBoardWrapper/ControlBoardWrapper.h
                        src/modules/ControlBoardWrapper/RPCMessagesParser.h
                        src/modules/ControlBoardWrapper/StreamingMessagesParser.h
                        src/modules/ControlBoardWrapper/SubDevice.h
                        PARENT_SCOPE)


set(cbw2_core_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
