PROJECT(libYARP_OS_compat)

INCLUDE_DIRECTORIES(include ../libYARP_OS/include ${CMAKE_SOURCE_DIR}/src/libYARP_OS_compat/include ${CMAKE_SOURCE_DIR}/src/libYARP_OS/include ${ACE_INCLUDE_DIR})

# We just intend to use cmake to create visual studio project files -
# so ok not to manually include list of source code
AUX_SOURCE_DIRECTORY(src libcode)

INCLUDE(YarpReqLib)

ADD_LIBRARY(YARP_OS_compat ${libcode})

LINK_LIBRARIES(YARP_OS_compat YARP_OS)

AUX_SOURCE_DIRECTORY(harness harnesscode)
ADD_EXECUTABLE(harness_os_compat ${harnesscode})

GET_TARGET_PROPERTY(EXEC harness_os_compat LOCATION)

# add in our tests
FOREACH(X ${harnesscode})
	  GET_FILENAME_COMPONENT(XN ${X} NAME_WE)
	  ADD_TEST(YARP_OS_compat::${XN} ${EXEC} regression ${XN})
ENDFOREACH(X $(harnesscode))

