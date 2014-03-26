project(QtYARPScopePlugin)

file(GLOB QtYARPScopePlugin_SRCS *.cpp)
file(GLOB QtYARPScopePlugin_HDRS *.h)
file(GLOB QtYARPScopePlugin_QRC_FILES *.qrc)

qt5_add_resources(QtYARPScopePlugin_QRC_GEN_SRCS ${QtYARPScopePlugin_QRC_FILES})

source_group("Source Files" FILES ${QtYARPScopePlugin_SRCS})
source_group("Header Files" FILES ${QtYARPScopePlugin_HDRS})
source_group("Resources Files" FILES ${QtYARPScopePlugin_QRC_FILES})
source_group("Generated Files" FILES ${QtYARPScopePlugin_QRC_GEN_SRCS})

set(QtYARPScopePlugin_QMLDIR "${CMAKE_INSTALL_LIBDIR}/qml/imports/robotology/yarp/scope")
# file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${QtYARPScopePlugin_QMLDIR})

add_library(QtYARPScopePlugin MODULE ${QtYARPScopePlugin_SRCS} ${QtYARPScopePlugin_HDRS} ${QtYARPScopePlugin_QRC_GEN_SRCS} )
target_link_libraries(QtYARPScopePlugin LINK_PRIVATE YARP::YARP_OS YARP::YARP_init Qt5::Widgets Qt5::Qml Qt5::Quick Qt5::Xml Qt5::PrintSupport)
target_compile_definitions(QtYARPScopePlugin PRIVATE -DQT_PLUGIN)
set_target_properties(QtYARPScopePlugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${QtYARPScopePlugin_QMLDIR})
configure_file(qmldir ${CMAKE_BINARY_DIR}/${QtYARPScopePlugin_QMLDIR}/qmldir COPYONLY)

install(TARGETS QtYARPScopePlugin DESTINATION ${QtYARPScopePlugin_QMLDIR})
install(FILES qmldir DESTINATION ${QtYARPScopePlugin_QMLDIR})
