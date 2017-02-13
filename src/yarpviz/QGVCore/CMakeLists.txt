
# all files which may be mocced
set(qgvlib_qt_HDRS
  private/QGVCore.cpp
  private/QGVGraphPrivate.cpp
  private/QGVEdgePrivate.cpp
  private/QGVGvcPrivate.cpp
  private/QGVNodePrivate.cpp
  QGVEdge.cpp
  QGVNode.cpp
  QGVScene.cpp
  QGVSubGraph.cpp
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    ${Qt5Widgets_INCLUDE_DIRS}
	${GRAPHVIZ_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/private
    ${CMAKE_CURRENT_BINARY_DIR}/private
)
# Activate Export macro for building the library itself
# if QGVCORE_LIB is not set or equal 0 (null), import macro is used
ADD_DEFINITIONS(-DQGVCORE_LIB -DWITH_CGRAPH)

source_group("Source Files" FILES ${qgvlib_qt_SRCS})
source_group("Header Files" FILES ${qgvlib_qt_HDRS})
source_group("Generated Files" FILES ${yarpviz_qt_QRC_GEN_SRCS}
                                     ${yarpviz_qt_UI_GEN_SRCS})

add_library(qgvcore SHARED
            ${qgvlib_qt_SRCS}
            ${qgvlib_qt_HDRS} )

target_link_libraries(qgvcore                      
                 	  ${GRAPHVIZ_GVC_LIBRARY}
                      ${GRAPHVIZ_CGRAPH_LIBRARY}
                      ${GRAPHVIZ_CDT_LIBRARY}
                      Qt5::Widgets
                      Qt5::Gui)
#qtyarp_use_modules(qgvcore Widgets Gui)
