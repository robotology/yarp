project(QtYARPScope)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/qtquick2applicationviewer)

file(GLOB_RECURSE QtYARPScope_SRCS *.cpp)
file(GLOB_RECURSE QtYARPScope_HDRS *.h)
file(GLOB QtYARPScope_QRC_FILES *.qrc)

qt5_add_resources(QtYARPScope_QRC_GEN_SRCS ${QtYARPScope_QRC_FILES})

source_group("Source Files" FILES ${QtYARPScope_SRCS})
source_group("Header Files" FILES ${QtYARPScope_HDRS})
source_group("Resource Files" FILES ${QtYARPScope_QRC_FILES})
source_group("Generated Files" FILES ${QtYARPScope_QRC_GEN_SRCS})

add_executable(QtYARPScope WIN32 ${QtYARPScope_SRCS} ${QtYARPScope_HDRS} ${QtYARPScope_QRC_GEN_SRCS})
qt5_use_modules(QtYARPScope Widgets Qml Quick)
qtyarp_deprecate_with_cmake_version(2.8.11)
if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    qt5_use_modules(QtYARPScope Qt5::WinMain)
endif()

install(TARGETS QtYARPScope COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
