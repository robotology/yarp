import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Window 2.0
import robotology.yarp.view 1.0
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
