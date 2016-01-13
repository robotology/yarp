# Try to find the QCustomPlot librairies
#  QCustomPlot_FOUND - system has QCustomPlot lib
#  QCustomPlot_INCLUDE_DIR - the GMP include directory
#  QCustomPlot_LIBRARIES - Libraries needed to use QCustomPlot

# Copyright (c) 2013, Anton Gladky <gladk@debian.org>
#
# Redistribution and use is allowed according to the terms of the GPL-3 license.


IF (QCustomPlot_INCLUDE_DIR AND QCustomPlot_LIBRARIES)
  SET(QCustomPlot_FIND_QUIETLY TRUE)
ENDIF (QCustomPlot_INCLUDE_DIR AND QCustomPlot_LIBRARIES)

FIND_PATH(QCustomPlot_INCLUDE_DIR NAMES qcustomplot.h )
FIND_LIBRARY(QCustomPlot_LIBRARIES NAMES qcustomplot )

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(QCustomPlot DEFAULT_MSG QCustomPlot_INCLUDE_DIR QCustomPlot_LIBRARIES)
