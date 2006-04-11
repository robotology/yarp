PROJECT(libYARP_OS_compat)

INCLUDE_DIRECTORIES(include ../libYARP_OS/include)

# We just intend to use cmake to create visual studio project files -
# so ok not to manually include list of source code
AUX_SOURCE_DIRECTORY(src libcode)
ADD_LIBRARY(YARP_OS_compat ${libcode})

FIND_LIBRARY(YARP_OS YARP_OS ../libYARP_OS)

IF(YARP_OS)
	LINK_LIBRARIES(YARP_OS_compat ${YARP_OS} ACE)
ENDIF(YARP_OS)

LINK_LIBRARIES(YARP_OS_compat ACE)

AUX_SOURCE_DIRECTORY(harness harnesscode)
ADD_EXECUTABLE(harness.exe ${harnesscode})
