project(QtYARPView)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qtquick2applicationviewer)

file(GLOB_RECURSE QtYARPScopePlugin_SRCS *.cpp)
file(GLOB_RECURSE QtYARPView_HDRS *.h)
file(GLOB QtYARPView_QRC_FILES *.qrc)

qt5_add_resources(QtYARPView_QRC_GEN_SRCS ${QtYARPView_QRC_FILES})

source_group("Source Files" FILES ${QtYARPScopePlugin_SRCS})
source_group("Header Files" FILES ${QtYARPView_HDRS})
source_group("Resources Files" FILES ${QtYARPView_QRC_FILES})
source_group("Generated Files" FILES ${QtYARPView_QRC_GEN_SRCS})

add_executable(QtYARPView WIN32 ${QtYARPScopePlugin_SRCS} ${QtYARPView_HDRS} ${QtYARPView_QRC_GEN_SRCS})
target_link_libraries(QtYARPView Qt5::Widgets Qt5::Qml Qt5::Quick)

install(TARGETS QtYARPView COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
