set(rmc_core_srcs src/modules/RemoteControlBoard/RemoteControlBoard.cpp)

set(rmc_core_hrds src/modules/RemoteControlBoard/RemoteControlBoard.h)


set(rmc_core_srcs ${rmc_core_srcs}
                  src/modules/RemoteControlBoard/stateExtendedReader.cpp)

set(rmc_core_hrds ${rmc_core_hrds}
                  src/modules/RemoteControlBoard//stateExtendedReader.hpp)


# using PARENT_SCOPE to make files visible from upper level

set(rmc_core_srcs ${rmc_core_srcs} PARENT_SCOPE)
set(rmc_core_hrds ${rmc_core_hrds} PARENT_SCOPE)


set(rmc_core_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
