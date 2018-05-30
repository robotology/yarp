# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# QCustomPlot
project(YARP_priv_qcustomplot)

include(YarpUseQt5)

set(qcustomplot_SRCS qcustomplot/qcustomplot.cpp)

set(qcustomplot_HDRS qcustomplot/qcustomplot.h)

add_library(YARP_priv_qcustomplot STATIC ${qcustomplot_SRCS})

target_link_libraries(YARP_priv_qcustomplot Qt5::Widgets)

set_property(TARGET YARP_priv_qcustomplot PROPERTY FOLDER "Libraries/External")

set(QCustomPlot_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot PARENT_SCOPE)
set(QCustomPlot_LIBRARIES "YARP_priv_qcustomplot" PARENT_SCOPE)
set(QCustomPlot_DEFINITIONS "" PARENT_SCOPE)

install(TARGETS YARP_priv_qcustomplot
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
