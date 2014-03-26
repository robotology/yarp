project(ContainerExample)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qtquick2applicationviewer)

file(GLOB_RECURSE ContainerExample_SRCS *.cpp)
file(GLOB_RECURSE ContainerExample_HDRS *.h)
file(GLOB ContainerExample_QRC_FILES *.qrc)

qt5_add_resources(ContainerExample_QRC_GEN_FILES ${ContainerExample_QRC_FILES})

source_group("Source Files" FILES ${ContainerExample_SRCS})
source_group("Header Files" FILES ${ContainerExample_HDRS})
source_group("Resources Files" FILES ${ContainerExample_QRC_FILES})
source_group("Generated Files" FILES ${ContainerExample_QRC_GEN_FILES})

add_executable(ContainerExample WIN32 ${ContainerExample_SRCS} ${ContainerExample_HDRS} ${ContainerExample_QRC_GEN_FILES})
qt5_use_modules(ContainerExample Widgets Qml Quick)

install(TARGETS ContainerExample COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
