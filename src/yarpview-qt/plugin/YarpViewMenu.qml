import QtQuick 2.0
import QtQuick.Controls 1.1


MenuBar {



    signal quit()
    signal about()
    signal freeze(bool checked)
    signal setOriginalSize()
    signal setOriginalAspectRatio()
    signal synchToDisplay(bool checked)
    signal changeRefreshInterval()
    signal saveSingleImage(bool checked)
    signal saveSetImages(bool checked)

    function enableSynch(check){
        if(check === true){
            synchItem.checked = true
        }else{
            synchItem.checked = false
        }
    }


    Menu {

        title: "File"

        MenuItem { id: saveSingle
                   text: "Save single image..."
                   checkable: true
                   onTriggered: {
                       saveSingleImage(saveSingle.checked)
                       saveSet.checked = false
                       saveSetImages(saveSet.checked)
                   }}
        MenuItem {  id: saveSet
                    text: "Save a set of images..."
                    checkable: true
                    onTriggered: {
                        saveSetImages(saveSet.checked)
                        saveSingle.checked = false
                        saveSingleImage(saveSingle.checked)
                    }}
        MenuSeparator{}
        MenuItem { text: "Quit"
            onTriggered: {
                quit()
            }}
    }

    Menu {
        title: "Image"
        MenuItem { text: "Original Size"
                   onTriggered: {
                       setOriginalSize()
                   }}
        MenuItem { text: "Original aspect ratio"
                    onTriggered: {
                        setOriginalAspectRatio()
                    }}
        MenuSeparator{}
        MenuItem { id: freezeItem
                   text: "Freeze"
                   checkable: true
                   onTriggered: {
                       freeze(freezeItem.checked)
                   }}
        MenuSeparator{}
        MenuItem { id: synchItem
                   text: "Synch display"
                   checkable: true
                   onTriggered: {
                       synchToDisplay(synchItem.checked)
                   }}
        MenuSeparator{}
        MenuItem { text: "Change refresh interval"
                    onTriggered: {
                        changeRefreshInterval()
                    }}
    }
    Menu {
        title: "Help"
        MenuItem { text: "About..."
            onTriggered: {
                about()
                   }
        }
    }

}

