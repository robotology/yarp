# Copyright (C) 2013  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# QCustomPlot
project(YARP_priv_qcustomplot)

include(YarpUseQt5)

set(qcustomplot_SRCS qcustomplot/qcustomplot.cpp)

set(qcustomplot_HDRS qcustomplot/qcustomplot.h)

add_library(YARP_priv_qcustomplot STATIC ${qcustomplot_SRCS})

qtyarp_use_modules(YARP_priv_qcustomplot Widgets)

set(QCustomPlot_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot PARENT_SCOPE)
set(QCustomPlot_LIBRARIES "YARP_priv_qcustomplot" PARENT_SCOPE)
set(QCustomPlot_DEFINITIONS "" PARENT_SCOPE)

install(TARGETS YARP_priv_qcustomplot
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_priv_qcustomplot)
