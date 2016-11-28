# use PARENT_SCOPE to make the following variables available at the 'parent' cmake

set(RGBDWrap_srcs      src/devices/RGBDSensorWrapper/RGBDSensorWrapper.cpp
                        PARENT_SCOPE)

set(RGBDWrap_hrds      src/devices/RGBDSensorWrapper/RGBDSensorWrapper.h
                        PARENT_SCOPE)


set(RGBDWrap_incl_dirs ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
