project(QtYarpManager)

include_directories(${YARP_MANAGER_INCLUDE_DIRS})

file(GLOB folder_source *.cpp)
file(GLOB folder_header *.h)
file(GLOB Resources *.qrc)
file(GLOB UI_FILES *.ui)

qt5_add_resources(ResourceSources ${Resources})
qt5_wrap_ui(UI_SRCS ${UI_FILES})

source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})
source_group("Resources Files" FILES ${Resources})
source_group("UI Files" FILES ${UI_FILES})
source_group("Generated Files" FILES ${MOC_HEADERS} ${ResourceSources} ${UI_SRCS})

add_executable(QtYarpManager WIN32 ${folder_source} ${folder_header} ${ResourceSources} ${UI_SRCS})
target_link_libraries(QtYarpManager ${YARP_LIBRARIES} ${YARP_MANAGER_LIBRARIES} Qt5::Widgets Qt5::Gui)

install(TARGETS QtYarpManager COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
