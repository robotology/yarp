# enable all QT-Stuff we need
# see http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4 for a complete list

SET(qgv_TS
)

SET(qgv_UI
  MainWindow.ui
)

# all files which may be mocced
SET(qgv_CPP
	main.cpp
  MainWindow.cpp
  QGraphicsViewEc.cpp
)

# include with QT_USE selected library parts
INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
  ${libqgv_SOURCE_DIR}
  ${libqgv_BINARY_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
)

# make *.h out of *.ui
QT4_WRAP_UI(qgv_UI_H ${qgv_UI})

# moc classes
QT4_AUTOMOC(${qgv_CPP})

# add translations
QT4_ADD_TRANSLATION(qgv_TRANS ${qgv_TS})

SET(neededStuff
  ${qgv_CPP}
  ${qgv_UI_H}
  ${qgv_TRANS}
)

IF(UNIX)
 	ADD_EXECUTABLE(sample ${neededStuff})
ELSEIF(APPLE)
	# it's with gui, so MACOSX_BUNDLE is needed
	ADD_EXECUTABLE(sample MACOSX_BUNDLE ${neededStuff})
ELSEIF(WIN32)
  # it's with gui, so WIN32 is needed
  ADD_EXECUTABLE(sample WIN32 ${neededStuff})
ENDIF()
SET_TARGET_PROPERTIES(sample PROPERTIES
		OUTPUT_NAME "Sample")

TARGET_LINK_LIBRARIES(sample
  ${QT_LIBRARIES}
	qgvcore
)

IF(BINDINGS_QT5)
	qt5_use_modules(sample Core Widgets)
	SET(CMAKE_AUTOMOC ON)
ENDIF(BINDINGS_QT5)

# The COMPONENT option only defines the internal belonging of the INSTALL'ed thing
INSTALL(
  TARGETS sample
  RUNTIME DESTINATION ${bin_DIR}
  COMPONENT sample
)
