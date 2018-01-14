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

StatusBar {
    id: root
    height: 78
    property string avgFps: "0.0"
    property string minFps: "0.0"
    property string maxFps: "0.0"

    property string displayAvgFps: "0.0"
    property string displayMinFps: "0.0"
    property string displayMaxFps: "0.0"

    property string name: "/name"

    function setName(name){
        root.name = name
    }
    signal connectPort(string source, string carrier)


    Column {
        id: column1
        spacing: 5

        Row {
            transformOrigin: Item.Center
            antialiasing: false


            TextField {
                id: portField
                placeholderText: qsTr("port")
            }

            TextField {
                id: carrierField

                placeholderText: qsTr("carrier")
            }
            Button {
                id: connect_button
                text: qsTr("Connect")
                onClicked:
                {
                    root.connectPort(portField.displayText, carrierField.displayText)
                }
            }
        }

        Column {
            spacing: 2

            Label{
                id: fps

                //anchors.bottom: displayFps.top

                text: "Port: " + root.avgFps +
                      " (min:" + root.minFps +
                      " max:" + root.maxFps +") fps"
                fontSizeMode: Text.VerticalFit
            }

            Label{
                id: displayFps

                //anchors.bottom: name.top


                text: "Display: " + root.displayAvgFps +
                      " (min:" + root.displayMinFps +
                      " max:" + root.displayMaxFps +") fps"
                fontSizeMode: Text.VerticalFit

            }

            Label{
                id: name
                text: root.name
                fontSizeMode: Text.VerticalFit

            }
        }
    }

}
