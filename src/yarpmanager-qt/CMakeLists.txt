project(QtYarpManager)

include_directories(${YARP_MANAGER_INCLUDE_DIRS})

file(GLOB QtYarpManager_SRCS *.cpp)
file(GLOB QtYarpManager_HDRS *.h)
file(GLOB QtYarpManager_QRC_FILES *.qrc)
file(GLOB QtYarpManager_UI_FILES *.ui)

qt5_add_resources(QtYarpManager_QRC_GEN_SRCS ${QtYarpManager_QRC_FILES})
qt5_wrap_ui(QtYarpManager_UI_GEN_SRCS ${QtYarpManager_UI_FILES})

source_group("Source Files" FILES ${QtYarpManager_SRCS})
source_group("Header Files" FILES ${QtYarpManager_HDRS})
source_group("Resources Files" FILES ${QtYarpManager_QRC_FILES})
source_group("UI Files" FILES ${QtYarpManager_UI_FILES})
source_group("Generated Files" FILES ${QtYarpManager_QRC_GEN_SRCS} ${QtYarpManager_UI_GEN_SRCS})

add_executable(QtYarpManager WIN32 ${QtYarpManager_SRCS} ${QtYarpManager_HDRS} ${QtYarpManager_QRC_GEN_SRCS} ${QtYarpManager_UI_GEN_SRCS})
target_link_libraries(QtYarpManager YARP::YARP_OS YARP::YARP_init YARP::YARP_manager Qt5::Widgets Qt5::Gui)

install(TARGETS QtYarpManager COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
