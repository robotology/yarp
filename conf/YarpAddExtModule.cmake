#####
# Parse a list of devices, creates library which compiles 
# all devices together. Generates code for inclusion 
# in libYARP_dev.  
# modulename: the name of the library
# path: path to the module

# On exit it sets:
# - met_dependencies a list of libraries that must be included (full path)
# - unmet_dependencies a list of lib/packages that should be included but
#   could not be found
# - link_directories a list of link directories 

# April 07 -- nat

MACRO(YarpAddExtModule modulename path)

  # Start a project.
  PROJECT(${modulename})

  ## clean variables
  SET(tmp_sources ${})
  SET(tmp_headers ${})

  # Check if there are any plugin device subdirectories.
  # Device subdirectories contain a config.cmake file.
  FILE(GLOB_RECURSE devices_list "${path}/yarpdevice.cmake")
  IF (devices_list)
	SET(GEN "${CMAKE_BINARY_DIR}/generated_code/${modulename}")
	IF (NOT EXISTS ${GEN})
	  FILE(MAKE_DIRECTORY ${GEN})
	ENDIF (NOT EXISTS ${GEN})

	SET(devices "${devices_list}")

	# We have a cpp file and a header file that call/list the 
	# initialization methods for all devices
	SET(ADDER_CPP ${GEN}/${modulename}Adder.cpp)
	SET(ADDER_H ${GEN}/${modulename}Adder.h)
	WRITE_FILE(${ADDER_H} "")

	# Write some preamble for the cpp file and header file
	WRITE_FILE(${ADDER_CPP} "#include \"${modulename}Adder.h\"")
	WRITE_FILE(${ADDER_CPP} "void add${modulename}() {" APPEND)

	# For each device directory, create the appropriate files
	SET(met_dependencies ${})
	SET(unmet_dependencies ${})

	FOREACH(dev ${devices})
	  # pick up the configuration of the device
	  GET_FILENAME_COMPONENT(dev_path "${dev}" PATH)

	  SET(SAVE_PATH ${CMAKE_MODULE_PATH})
	  SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${dev_path})
	  IF (dev_path)
		INCLUDE(${dev_path}/yarpdevice.cmake)
	  ELSE (dev_path)
		INCLUDE(${dev})
	  ENDIF (dev_path)
	  SET(CMAKE_MODULE_PATH ${SAVE_PATH})

	  # make a flag for conditional compilation of the device
	  SET(ENABLE_${YARPDEV_NAME} FALSE CACHE BOOL "Do you want to use ${YARPDEV_NAME}?")
	  SET(ENABLE_YARPDEV_NAME 0)
	  IF (ENABLE_${YARPDEV_NAME})
		SET(ENABLE_YARPDEV_NAME 1)
	  ENDIF (ENABLE_${YARPDEV_NAME})

	  IF (ENABLE_YARPDEV_NAME)
		# write a quick cpp file to add an appropriate factory for the device
		CONFIGURE_FILE(${YARP_MODULE_PATH}/yarpdev_helper.cpp.in
		  ${GEN}/add_${YARPDEV_NAME}.cpp @ONLY  IMMEDIATE)

		# aggregate this into our global list
		WRITE_FILE(${ADDER_CPP} "add_${YARPDEV_NAME}();" APPEND)
		WRITE_FILE(${ADDER_H} "extern void add_${YARPDEV_NAME}();" APPEND)
		FILE(GLOB folder_dev_source ${dev_path}/*.cpp ${dev_path}/*.cc 
		  ${dev_path}/*.c)
		FILE(GLOB folder_dev_header ${dev_path}/*.h)

		SET(folder_common_source ${})
		SET(folder_common_header ${})

		SET(folder_plat_source ${})
		SET(folder_header ${})
		SET(YARPDEV_DEPENDENCIES ${})

		#now searches for specific directories:
		#common directory	  
		IF (EXISTS ${dev_path}/common)
		  FILE(GLOB folder_common_source ${dev_path}/common/*.cpp 
			${dev_path}/common/*.c ${dev_path}/common/*.cc)
		  FILE(GLOB folder_common_header ${dev_path}/common/*.h)
		ENDIF(EXISTS ${dev_path}/common)

		#platform specific folders
		FILE(GLOB folder_plat_source ${dev_path}/${OS_TAG}/*.cpp 
		  ${dev_path}/${OS_TAG}/*.c ${dev_path}/${OS_TAG}/*.cc)
		FILE(GLOB folder_plat_header ${dev_path}/winnt/*.h)
		
		#dependencies
		IF(EXISTS ${dev_path}/${OS_TAG}/dependencies.cmake)
		  INCLUDE(${dev_path}/${OS_TAG}/dependencies.cmake)
		ENDIF(EXISTS ${dev_path}/${OS_TAG}/dependencies.cmake)

		SET(tmp_sources ${tmp_sources} ${GEN}/add_${YARPDEV_NAME}.cpp 
		  ${folder_dev_source} ${folder_common_source} ${folder_plat_source})
		SET(tmp_headers ${tmp_headers} ${folder_dev_header} 
		  ${folder_common_header} ${folder_plat_header})

		#	MESSAGE(STATUS "DEV: ${folder_dev_header}")
		#	MESSAGE(STATUS "COMMON: ${folder_common_header}")
		#  	MESSAGE(STATUS "PLAT: ${folder_plat_header}")

		######### now try to meet dependencies
		FOREACH(DEP ${YARPDEV_DEPENDENCIES})
		  ## directly within the repository
		  SET(DEV_DEP ${dev_path}/${OS_TAG}/dd_orig/lib/${DEP})
		  IF (EXISTS ${DEV_DEP})
			SET(met_dependencies ${met_dependencies} ${DEV_DEP})
			MESSAGE(STATUS "${YARPDEV_NAME} requires ${DEP}, OK")
		  ELSE(EXISTS ${DEV_DEP})
			## now try as an external package
			#FIND_PACKAGE(${DEP})
			IF(PKGCONFIG_EXECUTABLE)
			  PKGCONFIG(${DEP} ${DEP}_INCLUDE_DIR ${DEP}_LINK_DIRECTORIES
				${DEP}_LIBRARIES ${DEP}_CFLAGS)
			  message(STATUS "inc: -${${DEP}_INCLUDE_DIR}")
			  message(STATUS "link flags: -${${DEP}_LINK_FLAGS}")
			  message(STATUS "cflags: -${${DEP}_CFLAGS}")
			  message(STATUS "link dir: -${${DEP}_LINK_DIR}")
			ENDIF(PKGCONFIG_EXECUTABLE)
			IF (${DEP}_LIBRARIES) ### check if found
			  SET(met_dependencies ${met_dependencies} ${${DEP}_LIBRARIES})
			  SET(ext_directories "${ext_directories}" ${${DEP}_LINK_DIRECTORIES})
			  MESSAGE(STATUS "${YARPDEV_NAME} requires ${DEP}, OK")
			  INCLUDE_DIRECTORIES(${${DEP}_INCLUDE_DIR})
			ELSE(${DEP}_LIBRARIES)
			  SET(unmet_dependencies ${unmet_dependencies} ${DEP})
			  MESSAGE(STATUS "${YARPDEV_NAME} requires ${DEP}, NOT found!")
			ENDIF(${DEP}_LIBRARIES)
		  ENDIF (EXISTS ${DEV_DEP})
		ENDFOREACH(DEP ${YARPDEV_DEPENDENCIES})

		# make sure this device directory is included in our header
		# file path
		INCLUDE_DIRECTORIES(${dev_path})
	  ELSE (ENABLE_YARPDEV_NAME)
		MESSAGE(STATUS "Device ${YARPDEV_NAME} is not enabled")	
	  ENDIF (ENABLE_YARPDEV_NAME)
	ENDFOREACH(dev)

	# finish up the list of devices
	WRITE_FILE(${ADDER_CPP} "}" APPEND)
	WRITE_FILE(${ADDER_H} "extern void add${modulename}();" APPEND)

	MESSAGE(STATUS "Generated ${ADDER_CPP}")
	MESSAGE(STATUS "Generated  ${ADDER_H}")

	SET(tmp_sources ${tmp_sources} ${ADDER_CPP})
	SET(tmp_headers ${tmp_headers} ${ADDER_H})

	# Automatically add include directories if needed.
	FOREACH(header_file ${tmp_headers})
	  GET_FILENAME_COMPONENT(p ${header_file} PATH)
	  #MESSAGE(ERROR ${p})
	  INCLUDE_DIRECTORIES(${p})
	ENDFOREACH(header_file ${folder_header})

	# Set up the lib
	ADD_LIBRARY(${modulename} ${tmp_sources} ${tmp_headers})

  ENDIF (devices_list)
ENDMACRO(YarpAddExtModule)