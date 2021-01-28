/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
