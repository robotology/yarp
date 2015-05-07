/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
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
    id: bar
    height: 60
    property string avgFps: "0.0"
    property string minFps: "0.0"
    property string maxFps: "0.0"

    property string displayAvgFps: "0.0"
    property string displayMinFps: "0.0"
    property string displayMaxFps: "0.0"

    property string name: "/name"

    function setName(name){
        bar.name = name
    }

    Column{
        anchors.fill: parent
        spacing: 2

        Label{
            id: fps

            //anchors.bottom: displayFps.top

            text: "Port: " + bar.avgFps +
                  " (min:" + bar.minFps +
                  " max:" + bar.maxFps +") fps"
            fontSizeMode: Text.VerticalFit
            height: parent.height/3 - 2
        }

        Label{
            id: displayFps

            //anchors.bottom: name.top


            text: "Display: " + bar.displayAvgFps +
                  " (min:" + bar.displayMinFps +
                  " max:" + bar.displayMaxFps +") fps"
            fontSizeMode: Text.VerticalFit
            height: parent.height/3 - 2


        }

        Label{
            id: name


            text: bar.name
            fontSizeMode: Text.VerticalFit
            height: parent.height/3 - 2


        }
    }


}
