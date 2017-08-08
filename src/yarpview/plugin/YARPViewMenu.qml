/*
 * Copyright (C) 2014 Istituto Italiano di Tecnologia (IIT)
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

import QtQuick 2.0
import QtQuick.Controls 1.1


MenuBar {

    signal quit()
    signal about()
    signal freeze(bool checked)
    signal setOriginalSize()
    signal setOriginalAspectRatio()
    signal synchDisplayPeriod(bool checked)
    signal synchDisplaySize(bool checked)
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

    function enableAutosize(check){
        if(check === true){
            autosizeItem.checked = true
        }else{
            autosizeItem.checked = false
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
                       synchDisplayPeriod(synchItem.checked)
                   }}
        MenuSeparator{}
        MenuItem { id: autosizeItem
                   text: "Auto resize"
                   checkable: true
                   onTriggered: {
                       synchDisplaySize(autosizeItem.checked)
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

