# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# QCustomPlot
project(YARP_priv_qcustomplot)

include(YarpUseQt5)

add_library(YARP_priv_qcustomplot STATIC)

set(qcustomplot_SRCS qcustomplot/qcustomplot.cpp)

set(qcustomplot_HDRS qcustomplot/qcustomplot.h)

target_sources(YARP_priv_qcustomplot PRIVATE ${qcustomplot_SRCS})

target_include_directories(YARP_priv_qcustomplot PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot>)

target_link_libraries(YARP_priv_qcustomplot PUBLIC Qt5::Widgets)

set_property(TARGET YARP_priv_qcustomplot PROPERTY FOLDER "Libraries/External")

set(QCustomPlot_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot PARENT_SCOPE)
set(QCustomPlot_LIBRARIES "YARP_priv_qcustomplot" PARENT_SCOPE)
set(QCustomPlot_DEFINITIONS "" PARENT_SCOPE)

install(TARGETS YARP_priv_qcustomplot
        EXPORT YARP_priv_qcustomplot
        COMPONENT YARP_priv_qcustomplot
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

if(NOT CREATE_SHARED_LIBS)
  include(YarpInstallBasicPackageFiles)
  yarp_install_basic_package_files(YARP_priv_qcustomplot)
endif()
