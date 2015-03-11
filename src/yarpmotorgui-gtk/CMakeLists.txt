# 
# Copyright (C) 2010 RobotCub Consortium, European Commission FP6 Project IST-004370
# Author: Francesco Nori
# email:  francesco.nori@iit.it
# website: www.robotcub.org
# Permission is granted to copy, distribute, and/or modify this program
# under the terms of the GNU General Public License, version 2 or any
# later version published by the Free Software Foundation.
#
# A copy of the license can be found at
# http://www.robotcub.org/icub/license/gpl.txt
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
# Public License for more details
#

SET(PROJECTNAME robotMotorGui)
PROJECT(${PROJECTNAME})

FILE(GLOB_RECURSE folder_header src/include/allPartsWindow.h 
                                src/include/gtkMessages.h
                                src/include/guiControl.h
                                src/include/guiPid2.h
                                src/include/partMover.h
                                src/include/robotMotorGui.h
                                src/include/windowsTools.h
)
FILE(GLOB_RECURSE folder_source src/gtkMessages.cpp
                                src/allPartsWindow.cpp
                                src/guicontrol.cpp
                                src/guiPid2.cpp
                                src/main.cpp
                                src/multipleJointWindow.cpp
                                src/partMover.cpp
                                src/singleJointWindow.cpp
                                src/tableWindow.cpp
                                src/windowTools.cpp

)
SOURCE_GROUP("Header Files" FILES ${folder_header})
SOURCE_GROUP("Source Files" FILES ${folder_source})

ADD_DEFINITIONS(${GTK2_DEFINITIONS})

INCLUDE_DIRECTORIES(
                    ${GTK2_INCLUDE_DIRS}
                    ${YARP_INCLUDE_DIRS})

ADD_EXECUTABLE(${PROJECTNAME} ${folder_header} ${folder_source})

TARGET_LINK_LIBRARIES(${PROJECTNAME} 
                      icubmod
                      ${GTK2_LIBRARIES}
                      ${YARP_LIBRARIES})

INSTALL(TARGETS ${PROJECTNAME} DESTINATION bin)

