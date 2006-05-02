PROJECT(libYARP_sig)

INCLUDE_DIRECTORIES(include ../libYARP_OS/include ../libYARP_OS_compat/include ../libYARP_sig/include ${CMAKE_SOURCE_DIR}/src/libYARP_sig_compat/include ${CMAKE_SOURCE_DIR}/src/libYARP_OS/include ${CMAKE_SOURCE_DIR}/src/libYARP_OS_compat/include ${CMAKE_SOURCE_DIR}/src/libYARP_sig/include ${ACE_INCLUDE_DIR})

# We just intend to use cmake to create visual studio project files -
# so ok not to manually include list of source code
AUX_SOURCE_DIRECTORY(src libcode)
ADD_LIBRARY(YARP_sig_compat ${libcode})

FIND_LIBRARY(YARP_OS YARP_OS ../libYARP_OS)
FIND_LIBRARY(YARP_OS_compat YARP_OS_compat ../libYARP_OS_compat)
FIND_LIBRARY(YARP_sig YARP_sig ../libYARP_sig)

LINK_LIBRARIES(YARP_sig_compat YARP_OS YARP_OS_compat YARP_sig ACE)

AUX_SOURCE_DIRECTORY(harness harnesscode)
ADD_EXECUTABLE(harness_sig_compat ${harnesscode})

GET_TARGET_PROPERTY(EXEC harness_sig_compat LOCATION)

# add in our tests
FOREACH(X ${harnesscode})
	  GET_FILENAME_COMPONENT(XN ${X} NAME_WE)
	  ADD_TEST(YARP_sig_compat::${XN} ${EXEC} regression ${XN})
ENDFOREACH(X $(harnesscode))

