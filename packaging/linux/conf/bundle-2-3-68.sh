#!/bin/bash

DEPENDENCIES_COMMON="qtbase5-dev qtdeclarative5-dev qtmultimedia5-dev libqt5svg5 libtinyxml-dev libgtkmm-2.4-dev libace-dev subversion cmake dpkg wget libeigen3-dev qml-module-qtquick2 qml-module-qtquick-window2 qml-module-qtmultimedia qml-module-qtquick-dialogs qml-module-qtquick-controls libgtkdataboxmm-dev"
# Leave empty if you don't use dependancies from backports otherwise fill the following with the line that add backports in the distro sources.list (platform dependant)
DEPENDENCIES_jessie="libgoocanvasmm-dev"
DEPENDENCIES_xenial=""
DEPENDENCIES_yakkety=""

#BACKPORTS_URL_wheezy="http://http.debian.net/debian"
export YARP_PACKAGE_VERSION=2.3.68
#use YARP source code revision to fetch a different version tag or trunk
#export YARP_SOURCES_VERSION="trunk"
# always use a revision number >=1
export YARP_DEB_REVISION=1

YARP_CMAKE_OPTIONS="\
 -DCREATE_GUIS=TRUE \
 -DCMAKE_INSTALL_PREFIX=/usr \
 -DCREATE_SHARED_LIBRARY=TRUE \
 -DCREATE_LIB_MATH=TRUE \
 -DCREATE_OPTIONAL_CARRIERS=TRUE \
 -DENABLE_yarpcar_tcpros=TRUE \
 -DENABLE_yarpcar_xmlrpc=TRUE \
 -DENABLE_yarpcar_bayer=TRUE \
 -DENABLE_yarpcar_priority=TRUE \
 -DENABLE_yarpcar_portmonitor=TRUE \
 -DENABLE_yarpcar_depthimage=TRUE \
 -DCREATE_DEVICE_LIBRARY_MODULES=TRUE \
 -DENABLE_yarpmod_fakebot=TRUE \
 -DENABLE_yarpmod_fakeMotionControl=TRUE \
 -DENABLE_yarpmod_fakeAnalogSensor=TRUE \
 -DENABLE_yarpmod_fakeIMU=TRUE \
 -DENABLE_yarpmod_fakeLaser=TRUE \
 -DENABLE_yarpmod_SerialServoBoard=TRUE \
 -DENABLE_yarpmod_serial=TRUE \
 -DENABLE_yarpmod_serialport=TRUE \
"
