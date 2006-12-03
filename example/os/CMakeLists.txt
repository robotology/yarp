PROJECT (Examples)

# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)

## these definitions should be make automatically now
## if they are not, please report it as a bug:
##   http://sourceforge.net/tracker/?atid=500492&group_id=62418&func=browse
#IF (WIN32 AND NOT CYGWIN)
#  ADD_DEFINITIONS(-DWIN32 -D_WINDOWS)
#ENDIF (WIN32 AND NOT CYGWIN)
#
#IF(WIN32 AND CYGWIN)
#  ADD_DEFINITIONS(-DCYGWIN)
#ENDIF(WIN32 AND CYGWIN)

ADD_EXECUTABLE(bottle_add bottle_add.cpp)
ADD_EXECUTABLE(simple_sender simple_sender.cpp)
ADD_EXECUTABLE(simple_receiver simple_receiver.cpp)
ADD_EXECUTABLE(internal_monologue internal_monologue.cpp)
ADD_EXECUTABLE(buffered_port buffered_port.cpp)
ADD_EXECUTABLE(portable_pair portable_pair.cpp)
ADD_EXECUTABLE(summer summer.cpp)
ADD_EXECUTABLE(image_source image_source.cpp)
ADD_EXECUTABLE(database database.cpp)
ADD_EXECUTABLE(threads threads.cpp)
ADD_EXECUTABLE(image_process image_process.cpp)
ADD_EXECUTABLE(ratethread ratethread.cpp)
ADD_EXECUTABLE(make_count make_count.cpp)
ADD_EXECUTABLE(view_count view_count.cpp)

