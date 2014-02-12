project(QtYARPScope)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qtquick2applicationviewer)

file(GLOB_RECURSE folder_source *.cpp) 
file(GLOB_RECURSE folder_header *.h)
file(GLOB Resources *.qrc)

qt5_add_resources(ResourceSources ${Resources})

source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})
source_group("Resources Files" FILES ${Resources})
source_group("Generated Files" FILES ${ResourceSources})

add_executable(QtYARPScope WIN32 ${folder_source} ${folder_header} ${ResourceSources})
target_link_libraries(QtYARPScope ${YARP_LIBRARIES} Qt5::Widgets Qt5::Qml Qt5::Quick)

install(TARGETS QtYARPScope COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
