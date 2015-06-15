#!/bin/bash

DEPENDENCIES_COMMON="qtbase5-dev qtdeclarative5-dev qtmultimedia5-dev libqt5svg5 libgoocanvasmm-dev libtinyxml-dev libgsl0-dev libgtkmm-2.4-dev libace-dev subversion cmake dpkg wget"
# Leave empty if you don't use dependancies from backports otherwise fill the following with the line that add backports in the distro sources.list (platform dependant)
DEPENDENCIES_wheezy="qml-module-qtquick2 qml-module-qtquick-window2 qml-module-qtmultimedia qml-module-qtquick-dialogs qml-module-qtquick-controls"
DEPENDENCIES_jessie="qml-module-qtquick2 qml-module-qtquick-window2 qml-module-qtmultimedia qml-module-qtquick-dialogs qml-module-qtquick-controls libgtkdataboxmm-dev"
DEPENDENCIES_trusty="qtdeclarative5-qtquick2-plugin qtdeclarative5-window-plugin qtdeclarative5-qtmultimedia-plugin qtdeclarative5-controls-plugin qtdeclarative5-dialogs-plugin libgtkdataboxmm-dev "
DEPENDENCIES_utopic="qml-module-qtquick2 qml-module-qtquick-window2 qml-module-qtmultimedia qml-module-qtquick-dialogs qml-module-qtquick-controls libgtkdataboxmm-dev"
DEPENDENCIES_vivid="qml-module-qtquick2 qml-module-qtquick-window2 qml-module-qtmultimedia qml-module-qtquick-dialogs qml-module-qtquick-controls libgtkdataboxmm-dev"

BACKPORTS_URL_wheezy="http://http.debian.net/debian"
export YARP_VERSION=trunk
#export YARP_REVISION=
# always use a revision number >=1
export YARP_DEB_REVISION=1
