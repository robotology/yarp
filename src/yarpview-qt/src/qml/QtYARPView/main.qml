/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Window 2.0

// Import the YarpView module
import robotology.yarp.view 1.0
// Imports the resources of the YarpView Module.
// This serves in case the the module has its
// own qml defined in the resources
import "qrc:/YarpView/" 1.0

ApplicationWindow {
    id: window

    x: vSurface.posX
    y: vSurface.posY
    width: vSurface.windowWidth
    height: vSurface.windowHeight
    property int menuH: 0
    title: vSurface.moduleTitle

    onActiveChanged:   {
        if(menuH != 0){
            return;
        }

        menuH = window.height - vSurface.height - statusBar.height
        vSurface.menuHeight =  menuH
    }


    /**************************************************/
    menuBar: YarpViewMenu{
        id: menu
    }

    statusBar: YarpViewStatusBar{
        id: statusBar
    }


    VideoSurface{
        id: vSurface
        objectName: "YarpVideoSurface"
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
        onSynch:{
            if(menu !== undefined){
                menu.enableSynch(check)
            }
        }
        onSetName:{
            statusBar.setName(name)
        }
    }

    Connections{
        target: menu
        onFreeze:{
            vSurface.freeze(checked)
        }
        onSetOriginalSize:{
            vSurface.setOriginalSize()
        }

        onSetOriginalAspectRatio:{
            vSurface.setOriginalAspectRatio()
        }

        onChangeRefreshInterval:{
            vSurface.changeRefreshInterval()
        }

        onSynchToDisplay:{
            vSurface.synchToDisplay(checked)
        }

        onSaveSingleImage:{
            vSurface.saveSingleImage(checked)
        }

        onSaveSetImages:{
            vSurface.saveSetImages(checked)

        }
        onAbout:{
            vSurface.about()
        }

        onQuit:{
            Qt.quit()
        }
    }
}
