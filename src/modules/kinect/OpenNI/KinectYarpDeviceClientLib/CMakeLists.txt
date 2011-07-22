
# Search for source code.
FILE(GLOB folder_source *.cpp *.cc *.c)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

# Automatically add include directories if needed.
FOREACH(header_file ${folder_header})
  GET_FILENAME_COMPONENT(p ${header_file} PATH)
ENDFOREACH(header_file ${folder_header})

ADD_LIBRARY (KinectYarpDeviceClientLib ${folder_source} ${folder_header})
TARGET_LINK_LIBRARIES(KinectYarpDeviceClientLib ${YARP_LIBRARIES})
