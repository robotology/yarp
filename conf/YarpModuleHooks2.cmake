#########################################################################
# Redefine ADD_LIBRARY
#
MACRO(ADD_LIBRARY LIBNAME)
  IF (NOT YARPY_DEVICES)
    # pass on call without looking at it
    _ADD_LIBRARY(${LIBNAME} ${ARGN})
  ELSE (NOT YARPY_DEVICES)
    set(IS_IMPORTED FALSE)
    foreach(arg ${ARGN})
      if ("${arg}" STREQUAL "IMPORTED")
        set(IS_IMPORTED TRUE)
      endif()
    endforeach()
    if (NOT IS_IMPORTED)
      _ADD_LIBRARY(${LIBNAME} ${ARGN})
      message(STATUS "Recording a device library, ${LIBNAME}")
      set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_LIBS ${LIBNAME})
    endif (NOT IS_IMPORTED)
  ENDIF (NOT YARPY_DEVICES)
ENDMACRO(ADD_LIBRARY LIBNAME)


#########################################################################
# YARP_PREPARE_DEVICES macro creates generated code for enumerating
# devices, and adds a library containing all device code.
#
MACRO(YARP_PREPARE_DEVICES)
  SET(YARP_LIB_NAME ${YARPY_MASTER_DEVICE})

  get_property(devs GLOBAL PROPERTY YARP_BUNDLE_DEVICES)
  SET(YARP_CODE_PRE)
  SET(YARP_CODE_POST)
  FOREACH(dev ${devs})
    SET(YARP_CODE_PRE "${YARP_CODE_PRE}\nextern void add_${dev}();")
    SET(YARP_CODE_POST "${YARP_CODE_POST}\n    add_${dev}();")
  ENDFOREACH()

  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_lib.cpp.in
    ${YARPY_DEV_GEN}/add_${YARPY_MASTER_DEVICE}_devices.cpp @ONLY  IMMEDIATE)
  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_lib.h.in
    ${YARPY_DEV_GEN}/add_${YARPY_MASTER_DEVICE}_devices.h @ONLY  IMMEDIATE)

  get_property(code GLOBAL PROPERTY YARP_BUNDLE_CODE)
  get_property(dirs GLOBAL PROPERTY YARP_BUNDLE_INCLUDE_DIRS)
  include_directories(${YARP_INCLUDE_DIRS} ${dirs})
  _ADD_LIBRARY(${YARPY_MASTER_DEVICE} ${code} ${YARPY_DEV_GEN}/add_${YARPY_MASTER_DEVICE}_devices.cpp)
  target_link_libraries(${YARPY_MASTER_DEVICE} ${YARP_LIBRARIES})
  get_property(libs GLOBAL PROPERTY YARP_BUNDLE_LIBS)
  target_link_libraries(${YARPY_MASTER_DEVICE} ${libs})
  set(${YARPY_MASTER_DEVICE}_LIBRARIES ${libs})
ENDMACRO(YARP_PREPARE_DEVICES)


#########################################################################
# END_DEVICE_LIBRARY macro calls YARP_PREPARE_DEVICES if this is
# the outermost device library block, otherwise it propagates
# all collected information to the device library block that wraps
# it.
#
MACRO(END_DEVICE_LIBRARY devname)
  MESSAGE(STATUS "ending device library: ${devname}")
  IF ("${devname}" STREQUAL "${YARPY_MASTER_DEVICE}")
    YARP_PREPARE_DEVICES()   # generate device library
    SET(YARPY_DEVICES FALSE) # neutralize redefined methods 
  ENDIF ("${devname}" STREQUAL "${YARPY_MASTER_DEVICE}")
ENDMACRO(END_DEVICE_LIBRARY devname)
