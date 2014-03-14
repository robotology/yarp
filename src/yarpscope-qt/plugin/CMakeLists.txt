project(QtYARPScopePlugin)

file(GLOB QtYARPScopePlugin_SRCS *.cpp)
file(GLOB QtYARPScopePlugin_HDRS *.h)
file(GLOB QtYARPScopePlugin_QRC_FILES *.qrc)

qt5_add_resources(QtYARPScopePlugin_QRC_GEN_SRCS ${QtYARPScopePlugin_QRC_FILES})

source_group("Source Files" FILES ${QtYARPScopePlugin_SRCS})
source_group("Header Files" FILES ${QtYARPScopePlugin_HDRS})
source_group("Resources Files" FILES ${QtYARPScopePlugin_QRC_FILES})
source_group("Generated Files" FILES ${QtYARPScopePlugin_QRC_GEN_SRCS})

add_library(QtYARPScopePlugin MODULE ${QtYARPScopePlugin_SRCS} ${QtYARPScopePlugin_HDRS} ${QtYARPScopePlugin_QRC_GEN_SRCS} )
target_link_libraries(QtYARPScopePlugin ${YARP_LIBRARIES} Qt5::Widgets Qt5::Qml Qt5::Quick Qt5::Xml Qt5::PrintSupport)
target_compile_definitions(QtYARPScopePlugin PRIVATE -DQT_PLUGIN)

install(TARGETS QtYARPScopePlugin DESTINATION ${CMAKE_INSTALL_LIBDIR}/qml/imports/robotology/yarp/scope)
install(FILES qmldir DESTINATION ${CMAKE_INSTALL_LIBDIR}/qml/imports/robotology/yarp/scope)
