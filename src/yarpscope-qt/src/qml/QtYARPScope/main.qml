import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Window 2.0
import QtQuick.Window 2.1
import robotology.yarp.scope 1.0
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
