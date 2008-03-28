# This is a small project for the mei control
SET(PROJECTNAME PumaController)
PROJECT(${PROJECTNAME})


# We need YARP
FIND_PACKAGE(YARP)


# Find source code files
FILE(GLOB folder_header *.h)
FILE(GLOB folder_source *.cpp)

# Lay the files out in folders in environments where that makes sense
# This is completely optional
SOURCE_GROUP("Header Files" FILES ${folder_header})
SOURCE_GROUP("Source Files" FILES ${folder_source})

SET(PROJECTNAME PumaController)

# Add our include files into compiler's source path
SET(MODULE_PATH W:\\)
INCLUDE_DIRECTORIES(
${PROJECT_SOURCE_DIR}/include 
${YARP_DIR}/src/libYARP_math/include 
${YARP_DIR}/src/libYARP_sig/include 
${YARP_DIR}/src/libYARP_os/include 
${MODULE_PATH}/meiMotionControl/include
${MODULE_PATH}/meiMotionControl/common
${YARP_DIR}/gsl-1.8/gsl
)

LINK_LIBRARIES(
${MODULE_PATH}/meiMotionControl/winnt/dd_orig/lib/medvc60f
${YARP_DIR}/gsl-1.8/lib/gsl
${YARP_DIR}/gsl-1.8/lib/gslcblas
)

LINK_LIBRARIES(YARP_devd YARP_OSd YARP_sigd YARP_mathd )


# Create our executable
ADD_EXECUTABLE(PumaController ${folder_source} ${folder_header})

