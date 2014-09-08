project(QtYARPManager)

include_directories(${YARP_MANAGER_INCLUDE_DIRS})

file(GLOB QtYARPManager_SRCS *.cpp)
file(GLOB QtYARPManager_HDRS *.h)
file(GLOB QtYARPManager_QRC_FILES *.qrc)
file(GLOB QtYARPManager_UI_FILES *.ui)

qt5_add_resources(QtYARPManager_QRC_GEN_SRCS ${QtYARPManager_QRC_FILES})
qt5_wrap_ui(QtYARPManager_UI_GEN_SRCS ${QtYARPManager_UI_FILES})

source_group("Source Files" FILES ${QtYARPManager_SRCS})
source_group("Header Files" FILES ${QtYARPManager_HDRS})
source_group("Resources Files" FILES ${QtYARPManager_QRC_FILES})
source_group("UI Files" FILES ${QtYARPManager_UI_FILES})
source_group("Generated Files" FILES ${QtYARPManager_QRC_GEN_SRCS} ${QtYARPManager_UI_GEN_SRCS})

add_executable(QtYARPManager WIN32 ${QtYARPManager_SRCS} ${QtYARPManager_HDRS} ${QtYARPManager_QRC_GEN_SRCS} ${QtYARPManager_UI_GEN_SRCS})
target_link_libraries(QtYARPManager YARP::YARP_OS YARP::YARP_init YARP::YARP_manager)
qt5_use_modules(QtYARPManager Widgets Gui)
qtyarp_deprecate_with_cmake_version(2.8.11)
if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    qt5_use_modules(QtYARPManager Qt5::WinMain)
endif()

install(TARGETS QtYARPManager COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
