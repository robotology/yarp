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


MenuBar {

    signal quit()
    signal about()
    signal clear()
    signal rescale()
    signal playPressed(bool pressed)




    Menu {

        title: "File"
        MenuItem { text: "Quit"
            onTriggered: {
                quit()
            }}
    }

    Menu {
        title: "Actions"
        MenuItem { id: startStop
                   text: startStop.checked == true ? "Start" : "Stop"
                   checkable: true
                   checked: true
                   onTriggered: {
                       playPressed(startStop.checked)
                   }}
        MenuItem { text: "Clear"
                    onTriggered: {
                        clear()
                    }}

        MenuItem { text: "AutoRescale"
                   onTriggered: {
                        rescale()
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

