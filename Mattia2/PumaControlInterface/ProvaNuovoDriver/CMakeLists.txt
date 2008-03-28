# This is a small project for the puma wrapper
SET(PROJECTNAME ProvaNuovoDriver)
PROJECT(${PROJECTNAME})


# We need YARP
FIND_PACKAGE(YARP)


# Find source code files
# FILE(GLOB folder_header *.h)
FILE(GLOB folder_source *.cpp *.inl )

# Lay the files out in folders in environments where that makes sense
# This is completely optional
# SOURCE_GROUP("Header Files" FILES ${folder_header})
SOURCE_GROUP("Source Files" FILES ${folder_source})


# Add our include files into compiler's source path
SET(MODULE_PATH W:\\)
INCLUDE_DIRECTORIES(
${PROJECT_SOURCE_DIR}/include 
${YARP_DIR}/src/libYARP_sig/include 
${YARP_DIR}/src/libYARP_os/include 
${YARP_DIR}/src/libYARP_dev/include
${YARP_DIR}/src/libYARP_dev/src/mei/winnt/dd_orig/include
${YARP_DIR}/src/libYARP_dev/src/mei/common/yarp
${YARP_DIR}/Mattia2/PumaKinLib
${YARP_DIR}/gsl-1.8
${YARP_DIR}/src/libYARP_math/include
${YARP_DIR}/src/modules

 
# ${MODULE_PATH}/meiMotionControl/include
# ${MODULE_PATH}/meiMotionControl/common

)



 LINK_LIBRARIES(
 ${YARP_DIR}/src/libYARP_dev/src/mei/winnt/dd_orig/lib/medvc60f/medvc60f.lib
 ${YARP_DIR}/gsl-1.8/lib/gsl/gsl.lib
 ${YARP_DIR}/gsl-1.8/lib/gslcblas/gslcblas.lib
 ${YARP_DIR}/Mattia2/PumaKinLib/Release/Pumalib06.lib



#X:\\Mattia2/PumaKinLib/Debug/Pumalib06
)

LINK_LIBRARIES(YARP_dev YARP_OS YARP_sig)


# Create our executable
ADD_EXECUTABLE(PumaController ${folder_source}
#  ${folder_header}
)

