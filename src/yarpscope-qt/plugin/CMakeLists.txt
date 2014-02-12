project(QtYARPScopePlugin)    

add_definitions(-DQT_PLUGIN)

file(GLOB folder_source *.cpp)
file(GLOB folder_header *.h)
file(GLOB Resources *.qrc)

qt5_add_resources(ResourceSources ${Resources})

source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})
source_group("Resources Files" FILES ${Resources})
source_group("Generated Files" FILES ${ResourceSources})

add_library(QtYARPScopePlugin MODULE ${folder_source} ${folder_header} ${ResourceSources} )

target_link_libraries(QtYARPScopePlugin ${YARP_LIBRARIES} Qt5::Widgets Qt5::Qml Qt5::Quick Qt5::Xml Qt5::PrintSupport)

install(TARGETS QtYARPScopePlugin DESTINATION ${CMAKE_INSTALL_LIBDIR}/qml/imports/robotology/yarp/scope)
install(FILES qmldir DESTINATION ${CMAKE_INSTALL_LIBDIR}/qml/imports/robotology/yarp/scope)
