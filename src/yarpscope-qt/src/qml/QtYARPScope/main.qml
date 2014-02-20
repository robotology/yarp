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
import QtQuick.Window 2.1

// Import the YarpScope module
import robotology.yarp.scope 1.0
// Imports the resources of the YarpScope Module.
// This serves in case the the module has its
// own qml defined in the resources
import "qrc:/YarpScope/"

ApplicationWindow {
    id: window
    width: 500
    height: 500

    /*************************************************/
    menuBar: YarpScopeMenu{
        id: menu
    }

    toolBar: YarpScopeToolBar{
        id: toolBar
    }

    QtYARPScopePlugin{
        anchors.fill: parent
        id: graph
        objectName: "YarpScope1"
    }  
    /*************************************************/

    Connections{
        target: toolBar
        onPlayPressed:{
            graph.playPressed(pressed)
        }
        onClear:{
            graph.clear()
        }
        onRescale:{
            graph.rescale()
        }
        onChangeInterval:{
            graph.changeInterval(interval)
        }
    }

    Connections{
        target: menu
        onPlayPressed:{
            graph.playPressed(pressed)
        }
        onClear:{
            graph.clear()
        }
        onRescale:{
            graph.rescale()
        }

        onAbout:{
            aboutDlg.visibility = Window.Windowed
        }
    }


    Connections{
        target: graph
        onIntervalLoaded:{
            toolBar.refreshInterval(interval)
        }
        onSetWindowTitle:{
            window.title = title
        }
        onSetWindowPosition:{
            window.x = x
            window.y = y
        }
        onSetWindowSize:{
            window.width = w
            window.height = h
        }
    }

    function parseParameters(params){
        var ret = graph.parseParameters(params)
        return ret
    }

    YarpScopeAbout{
        id: aboutDlg
        visibility: Window.Hidden
    }

}
