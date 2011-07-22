
#OPENNI Lib MESSAGE("$ENV{OPEN_NI_INCLUDE}")
INCLUDE_DIRECTORIES($ENV{OPEN_NI_INCLUDE})
LINK_DIRECTORIES($ENV{OPEN_NI_LIB})
LINK_LIBRARIES("OpenNI.lib")

#NITE Lib MESSAGE("$ENV{XN_NITE_INSTALL_PATH}")
INCLUDE_DIRECTORIES($ENV{XN_NITE_INSTALL_PATH}/Include)
LINK_DIRECTORIES($ENV{XN_NITE_INSTALL_PATH}/Lib)
LINK_LIBRARIES("XnVNITE.lib")

# Search for source code.
FILE(GLOB folder_source *.cpp *.cc *.c)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

# Automatically add include directories if needed.
FOREACH(header_file ${folder_header})
  GET_FILENAME_COMPONENT(p ${header_file} PATH)
ENDFOREACH(header_file ${folder_header})

# Set up our main executable.
IF (folder_source)
  ADD_EXECUTABLE(KinectDeviceDriverServer ${folder_source} ${folder_header})
ELSE (folder_source)
  MESSAGE(FATAL_ERROR "No source code files found. Please add something")
ENDIF (folder_source)

TARGET_LINK_LIBRARIES(KinectDeviceDriverServer YARP_init ${CORELIBS} ${YARP_LIBRARIES} )
