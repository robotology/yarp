project(QtYARPScopePlugin)

file(GLOB QtYARPScopePlugin_SRCS *.cpp)
file(GLOB QtYARPScopePlugin_HDRS *.h)
file(GLOB QtYARPScopePlugin_QRC_FILES *.qrc)

qt5_add_resources(QtYARPScopePlugin_QRC_GEN_SRCS ${QtYARPScopePlugin_QRC_FILES})

source_group("Source Files" FILES ${QtYARPScopePlugin_SRCS})
source_group("Header Files" FILES ${QtYARPScopePlugin_HDRS})
source_group("Resources Files" FILES ${QtYARPScopePlugin_QRC_FILES})
source_group("Generated Files" FILES ${QtYARPScopePlugin_QRC_GEN_SRCS})

set(QtYARPScopePlugin_QMLDIR "qml/imports/robotology/yarp/scope/")

add_library(QtYARPScopePlugin MODULE ${QtYARPScopePlugin_SRCS} ${QtYARPScopePlugin_HDRS} ${QtYARPScopePlugin_QRC_GEN_SRCS} )
target_link_libraries(QtYARPScopePlugin YARP::YARP_OS YARP::YARP_init)
qt5_use_modules(QtYARPScopePlugin Widgets Qml Quick Xml PrintSupport)

qtyarp_qml_plugin(QtYARPScopePlugin ${QtYARPScopePlugin_QMLDIR})

install(TARGETS QtYARPScopePlugin DESTINATION ${CMAKE_INSTALL_LIBDIR}/${QtYARPScopePlugin_QMLDIR})
install(FILES qmldir DESTINATION ${CMAKE_INSTALL_LIBDIR}/${QtYARPScopePlugin_QMLDIR})
