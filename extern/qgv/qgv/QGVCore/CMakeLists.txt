# enable all QT-Stuff we need
# see http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4 for a complete list

SET(qgvlib_TS
)

SET(qgvlib_UI
  
)

# all files which may be mocced
SET(qgvlib_CPP
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

SET(TARGET_NAME "QGVCore")

# include with QT_USE selected library parts
INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
	${GRAPHVIZ_INCLUDE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/private
  ${CMAKE_CURRENT_BINARY_DIR}/private
  
)
# Activate Export macro for building the library itself
# if QGVCORE_LIB is not set or equal 0 (null), import macro is used
ADD_DEFINITIONS(-DQGVCORE_LIB -D_PACKAGE_ast -D_dll_import -D_BLD_cdt -D_DLL_BLD)

IF(BINDINGS_QT4)
	# QT5 does not declare and does not need QT_INCLUDES
	SET(INCLUDE_DIRECTORIES ${INCLUDES_DIRECTORIES} ${QT_INCLUDES})
	INCLUDE(${QT_USE_FILE})
ENDIF(BINDINGS_QT4)
ADD_DEFINITIONS(-DQVGCORE_LIB)

IF(BINDINGS_QT4)
	QT4_AUTOMOC(${qgvlib_CPP})
	QT4_WRAP_UI(qgvlib_UI_H ${qgvlib_UI})
	QT4_ADD_TRANSLATION(qgvlib_TRANS ${qgvlib_TS})
	# -binary is not working
	# QT4_ADD_RESOURCES(RCCS "${QRCS}" OPTIONS "-binary")
	# QT4_ADD_RESOURCES(qgvlib_RCCS ${qgvlib_QRC})
ENDIF(BINDINGS_QT4)
IF(BINDINGS_QT5)
	QT5_WRAP_UI(qgvlib_UI_H ${qgvlib_UI})
	QT5_ADD_TRANSLATION(qgvlib_TRANS ${qgvlib_TS})
	QT5_ADD_RESOURCES(qgvlib_RCCS ${qgvlib_QRC})
ENDIF(BINDINGS_QT5)

ADD_LIBRARY(qgvcore SHARED
  ${qgvlib_CPP}
  ${qgvlib_UI_H}
  ${qgvlib_TRANS}
)

TARGET_LINK_LIBRARIES(qgvcore
  ${QT_LIBRARIES}
	${GRAPHVIZ_GVC_LIBRARY}
  ${GRAPHVIZ_CGRAPH_LIBRARY}
  ${GRAPHVIZ_CDT_LIBRARY}
)

IF(BINDINGS_QT5)
	qt5_use_modules(QVGCore Core Widgets)
ENDIF(BINDINGS_QT5)

SET_TARGET_PROPERTIES(qgvcore PROPERTIES
	OUTPUT_NAME "QGVCore"
	VERSION ${TARGET_VERSION_MAJOR}.${TARGET_VERSION_MINOR}.${${PROJECT_NAME}_PATCH_LEVEL}
	SOVERSION ${TARGET_VERSION_MAJOR})
IF(WIN32)
	# this extra install command is needed by the nsis installer
	INSTALL(TARGETS qgvcore
		RUNTIME DESTINATION ${LIB_INSTALL_DIR} 
		COMPONENT library 
	)
ELSE(WIN32)
	INSTALL(TARGETS qgvcore
		ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
		LIBRARY DESTINATION ${LIB_INSTALL_DIR}
		COMPONENT library
	)
ENDIF(WIN32)

# The COMPONENT option only defines the internal belonging of the INSTALL'ed thing
# INSTALL(
  # FILES ${qgvlib_TRANS}
  # DESTINATION ${qm_DIR}  
  # COMPONENT library
# )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}.pc.in
                ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.pc @ONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}-uninstalled.pc.in
                ${CMAKE_BINARY_DIR}/${TARGET_NAME}-uninstalled.pc @ONLY)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.pc
                DESTINATION lib/pkgconfig)

IF(WIN32)
	# QuaZip
	# FIND_FILE(PACK_zlib zlib.dll PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)
	# INSTALL(FILES ${QUAZIP_LIBRARIES} ${PACK_zlib} DESTINATION ${bin_DIR} COMPONENT library)

	# Qt itself
	FIND_FILE(PACK_qtcore QtCore4.dll PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)
	FIND_FILE(PACK_qtgui QtGui4.dll PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)
	
	FIND_FILE(PACK_mingwm10 mingwm10.dll PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)
	FIND_FILE(PACK_libgcc_s_dw libgcc_s_dw2-1.dll PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)
	
	INSTALL(
		FILES 
			${PACK_qtcore}
			${PACK_qtgui}
			# ${PACK_qtnetwork}
			# ${PACK_qtsql}
			# ${PACK_libmysql}
			# ${PACK_qtxml}
			# ${PACK_qtxml_patterns}
			${PACK_mingwm10}
			${PACK_libgcc_s_dw}
      DESTINATION ${bin_DIR}
		COMPONENT qtlibs
	)
  
  INSTALL(
    FILES
      ${GRAPHVIZ_GVC_LIBRARY}
      ${GRAPHVIZ_CGRAPH_LIBRARY}
      ${GRAPHVIZ_CDT_LIBRARY}
      DESTINATION ${bin_DIR}
    COMPONENT graphvizlibs
  )
ENDIF(WIN32)

IF(BUILD_DEV)
	FILE(GLOB header "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
	INSTALL(
		FILES ${header}
		DESTINATION ${include_DIR}
		COMPONENT headerfiles
	)
ENDIF(BUILD_DEV)
