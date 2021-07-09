/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Window 2.1

// Import the YARPView module
import robotology.yarp.view 1.0
// Imports the resources of the YARPView Module.
// This serves in case the the module has its
// own qml defined in the resources
import "qrc:/YARPView/" 1.0

ApplicationWindow {
    id: window

    x: vSurface.posX
    y: vSurface.posY
    width: vSurface.windowWidth
    height: vSurface.windowHeight
    property int menuH: 0
    title: vSurface.moduleTitle

    function calc()    {
        if(menuH != 0){
            return;
        }

        menuH = window.height - vSurface.height - statusBar.height
        vSurface.menuHeight =  menuH
    }

    /**************************************************/

    VideoSurface{
        id: vSurface
        objectName: "YARPVideoSurface"
        anchors.fill: parent
        dataArea: statusBar
        menuHeight: menuH
    }
    /**************************************************/

    Connections{
        target: vSurface
        onChangeWindowSize:{
            window.width = w
            window.height = h
        }
        onSynchRate:{
            if(menu !== undefined){
                menu.enableSynch(check)
            }
        }
        onAutosize:{
            if(menu !== undefined){
                menu.enableAutosize(check)
            }
        }
        onSetName:{
            title = name
        }
    }
}
