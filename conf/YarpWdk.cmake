# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

macro(WDK_SETUP WDK_BUILD WDK_TEMPLATE)
    set(WDK_SOURCE_LIST_RAW)
    set(WDK_INCLUDE_LIST_RAW)
    foreach(src ${ARGN})
        #message(STATUS "src is ${src}")
        file(GLOB src2 RELATIVE ${CMAKE_SOURCE_DIR} ${src})
        get_filename_component(ext ${src2} EXT)
        if(ext STREQUAL ".cpp")
            string(REPLACE "/" "_" src3 ${src2})
            configure_file(${src} ${WDK_BUILD}/${src3} COPYONLY IMMEDIATE)
            list(APPEND WDK_SOURCE_LIST_RAW ${src3})
        endif(ext STREQUAL ".cpp")
    endforeach(src)
    set(WDK_SOURCE_LIST "${WDK_SOURCE_LIST_RAW}")
    string(REPLACE ";" " " WDK_SOURCE_LIST "${WDK_SOURCE_LIST}")
    get_property(WDK_INCLUDE_LIST0 GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
    set(WDK_INCLUDE_LIST0 ${WDK_INCLUDE_LIST0} ${ACE_INCLUDE_DIRS})
    file(TO_NATIVE_PATH "${WDK_INCLUDE_LIST0}" WDK_INCLUDE_LIST)
    configure_file(${WDK_TEMPLATE} ${WDK_BUILD}/sources @ONLY IMMEDIATE)
endmacro(WDK_SETUP)

option(CREATE_WDK_BUILD "Create files for a WDK build" FALSE)

if(CREATE_WDK_BUILD)

  set(WDK_BASE_BUILD ${CMAKE_BINARY_DIR}/wdk)

  message(STATUS "WDK build: setting up yarpos")

  set(WDK_BUILD ${WDK_BASE_BUILD}/yarpos)

  # We will copy in source files, since WDK won't accept
  # source files lying outside of build root, and links
  # are dicey in windows.

  get_property(os_src TARGET YARP_OS PROPERTY SOURCES)
  get_property(sig_src TARGET YARP_sig PROPERTY SOURCES)
  get_property(dev_src TARGET YARP_dev PROPERTY SOURCES)
  get_property(init_src TARGET YARP_init PROPERTY SOURCES)
  #get_property(yarp_src TARGET yarp PROPERTY SOURCES)
  set(YARP_COPY_SRC ${gen_src} ${os_src} ${sig_src} ${dev_src} ${init_src}) # ${yarp_src})
  wdk_setup(${WDK_BUILD} ${CMAKE_SOURCE_DIR}/conf/wdk/sources_yarp_os.in ${YARP_COPY_SRC})

  configure_file(${CMAKE_SOURCE_DIR}/conf/wdk/yarpos.def.in ${WDK_BUILD}/yarpos.def COPYONLY IMMEDIATE)

  message(STATUS "WDK build: setting up yarp executable")
  set(WDK_BUILD ${WDK_BASE_BUILD}/yarp)
  get_property(yarp_src TARGET yarp PROPERTY SOURCES)
  wdk_setup(${WDK_BUILD} ${CMAKE_SOURCE_DIR}/conf/wdk/sources_yarp.in ${yarp_src})

endif(CREATE_WDK_BUILD)
