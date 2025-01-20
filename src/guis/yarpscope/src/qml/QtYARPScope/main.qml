/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Window 2.0
import QtQuick.Window 2.1

// Import the YARPScope module
import robotology.yarp.scope 1.0
// Imports the resources of the YARPScope Module.
// This serves in case the the module has its
// own qml defined in the resources
import "qrc:/YARPScope/"

ApplicationWindow {
    id: window
    width: 500
    height: 500

    /*************************************************/
    menuBar: YARPScopeMenu{
        id: menu
    }

    toolBar: YARPScopeToolBar{
        id: toolBar
    }

    QtYARPScopePlugin {
        anchors.fill: parent
        id: graph
        objectName: "YARPScope1"
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
        onQuit:{
            Qt.quit();
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

    YARPScopeAbout{
        id: aboutDlg
        visibility: Window.Hidden
    }

}
