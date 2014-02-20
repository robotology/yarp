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

Rectangle {

    signal playPressed(bool pressed);
    signal clear()
    signal rescale()
    signal changeInterval(int interval)

    Connections{
        target: spinBox1
        onEditingFinished:{
            changeInterval(spinBox1.value)
        }
    }

    function refreshInterval(interval){
        spinBox1.value = interval
    }


    function setPlayState(){
        playPauseBtn.checked = true
    }


    height: 70
    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#ffffff"
        }

        GradientStop {
            position: 1
            color: "#000000"
        }
    }

    SpinBox {
        id: spinBox1
        x: 14
        y: 19
        width: 65
        height: 24
        minimumValue: 5
        maximumValue: 1000

    }

    Label {
        id: label1
        x: 14
        y: 49
        width: 65
        height: 13
        text: "Interval"
        horizontalAlignment: Text.AlignHCenter
    }

    ToolButton {
        id: playPauseBtn
        x: 98
        y: 8
        width: 68
        height: 35
        checkable: true
        checked: true
        iconSource: playPauseBtn.checked == true ? "qrc:/YarpScope/images/pause-icon.png" : "qrc:/YarpScope/images/play-icon.png"
        onClicked: {
            playPressed(playPauseBtn.checked)
        }
    }

    ToolButton {
        id: clearBtn
        x: 187
        y: 8
        width: 68
        height: 35
        iconSource: "qrc:/YarpScope/images/clear-icon.png"
        onClicked: {
            clear()
        }
    }

    ToolButton {
        id: rescaleBtn
        x: 276
        y: 8
        width: 68
        height: 35
        iconSource: "qrc:/YarpScope/images/rescale-icon.png"
        onClicked: {
            rescale()
        }
    }

    Label {
        id: label2
        x: 98
        y: 49
        width: 68
        height: 13
        text: playPauseBtn.checked == true ? "Stop" : "Play"
        horizontalAlignment: Text.AlignHCenter
    }

    Label {
        id: label3
        x: 188
        y: 49
        width: 68
        height: 13
        text: "Clear"
        horizontalAlignment: Text.AlignHCenter
    }

    Label {
        id: label4
        x: 276
        y: 49
        width: 68
        height: 13
        text: "Auto Rescale"
        horizontalAlignment: Text.AlignHCenter
    }
}
