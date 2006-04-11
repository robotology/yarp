PROJECT(libYARP_sig)

INCLUDE_DIRECTORIES(include ../libYARP_OS/include)

# We just intend to use cmake to create visual studio project files -
# so ok not to manually include list of source code
AUX_SOURCE_DIRECTORY(src libcode)
ADD_LIBRARY(YARP_sig ${libcode})

FIND_LIBRARY(YARP_OS YARP_OS ../libYARP_OS)

IF(YARP_OS)
	LINK_LIBRARIES(YARP_sig ${YARP_OS})
ENDIF(YARP_OS)

LINK_LIBRARIES(YARP_sig ACE)

AUX_SOURCE_DIRECTORY(harness harnesscode)
ADD_EXECUTABLE(harness.exe ${harnesscode})
