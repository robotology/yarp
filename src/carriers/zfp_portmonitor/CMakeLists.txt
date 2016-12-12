# Copyright (C) 2016 iCub Facility
# Authors: Nicolo' Genesio
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})

# probably not needed ... to be tested
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

include(YarpPlugin)
include(YarpInstallationHelpers)

# probably not needed ... plugins should set this automatically ... to be tested
set(YARP_FORCE_DYNAMIC_PLUGINS TRUE CACHE INTERNAL "yarp_pm_zfp is always built with dynamic plugins")

yarp_configure_external_installation(yarp)

yarp_prepare_plugin(zfp        TYPE ZfpMonitorObject
                               INCLUDE zfpPortmonitor.h
                               CATEGORY portmonitor
                               DEPENDS "CREATE_OPTIONAL_CARRIERS;ENABLE_yarpcar_portmonitor" 
                               )

if(NOT SKIP_zfp)

	find_package(ZFP REQUIRED)

	if (NOT ZFP_FOUND)
		message("-- Did not find ZFP library.")

	else(NOT ZFP_FOUND)

	get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
	get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
	include_directories(${YARP_OS_INCLUDE_DIRS}
		      ${YARP_sig_INCLUDE_DIRS} ${ZFP_INCLUDE_DIR})

	yarp_add_plugin(yarp_pm_zfp
		        zfpPortmonitor.cpp
		        zfpPortmonitor.h)
	target_link_libraries(yarp_pm_zfp YARP_OS YARP_sig ${ZFP_LIBRARY})

	yarp_install(TARGETS yarp_pm_zfp
		     EXPORT YARP
		     COMPONENT runtime
		     LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
		     ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

	yarp_install(FILES zfp.ini
		     COMPONENT runtime
		     DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
	endif (NOT ZFP_FOUND)
endif(NOT SKIP_zfp)
